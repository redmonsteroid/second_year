package main

import (
	"fmt"
	"io"
	"os"
	"strings"
	"syscall"
	"testing"
)
// Тестируем processArguments
func TestProcessArguments(t *testing.T) {
	args := []string{"program", "--file", "test.txt", "--query", "QPUSH test"}
	filename, query, err := processArguments(args)

	if err != nil {
		t.Errorf("Expected no error, got %v", err)
	}
	if filename != "test.txt" {
		t.Errorf("Expected filename to be 'test.txt', got %v", filename)
	}
	if query != "QPUSH test" {
		t.Errorf("Expected query to be 'QPUSH test', got %v", query)
	}
}

func TestProcessArgumentsMissingArgs(t *testing.T) {
	args := []string{"program", "--file", "test.txt"}
	_, _, err := processArguments(args)

	if err == nil {
		t.Errorf("Expected error for missing arguments, got nil")
	}
}

func TestPopEmptyQueue(t *testing.T) {
	queue := &Queue{} // Создаем пустую очередь

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем Pop на пустой очереди
	queue.Pop()

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что будет напечатано "Queue is empty."
	expected := "Queue is empty.\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

// Тестируем Pop с одним элементом, чтобы проверить установку q.end = nil
func TestPopSingleElementQueue(t *testing.T) {
	queue := &Queue{}
	queue.Push("item") // Добавляем один элемент в очередь

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем Pop на очереди с одним элементом
	queue.Pop()

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что будет напечатано "Popped: item"
	expected := "Popped: item\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Проверяем, что очередь пуста
	if queue.front != nil || queue.end != nil {
		t.Errorf("Expected queue to be empty, but got front: %v, end: %v", queue.front, queue.end)
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

// Тестируем executeQueueOperations
func TestExecuteQueueOperations(t *testing.T) {
	// Создаем временный файл
	tmpFile, err := os.CreateTemp("", "queue_test_*.txt")
	if err != nil {
		t.Fatalf("Failed to create temp file: %v", err)
	}
	defer os.Remove(tmpFile.Name())

	// Записываем данные в файл
	_, _ = tmpFile.WriteString("initial\n")
	_ = tmpFile.Close()

	// Выполняем операции
	err = executeQueueOperations(tmpFile.Name(), "QPUSH test")
	if err != nil {
		t.Errorf("Expected no error, got %v", err)
	}

	// Проверяем содержимое файла
	data, err := os.ReadFile(tmpFile.Name())
	if err != nil {
		t.Fatalf("Failed to read temp file: %v", err)
	}

	expected := "initial\ntest\n"
	if string(data) != expected {
		t.Errorf("Expected file content to be %q, got %q", expected, string(data))
	}
}

// Тестируем processCommand
func TestProcessCommand(t *testing.T) {
	queue := &Queue{}
	processCommand("QPUSH test", queue)
	processCommand("QPUSH example", queue)

	output := captureOutput(func() {
		processCommand("QPRINT", queue)
	})

	expected := "test example \n"
	if output != expected {
		t.Errorf("Expected %q, but got %q", expected, output)
	}

	processCommand("QPOP", queue)

	output = captureOutput(func() {
		processCommand("QPRINT", queue)
	})
	expected = "example \n"
	if output != expected {
		t.Errorf("Expected %q after pop, but got %q", expected, output)
	}
}

func captureOutput(f func()) string {
	old := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	f()

	_ = w.Close()
	os.Stdout = old

	var buf strings.Builder
	_, _ = io.Copy(&buf, r) // Используем io.Copy вместо ReadFrom
	return buf.String()
}



func TestMainWithError(t *testing.T) {
	// Мокируем os.Args
	os.Args = []string{"program", "--file"}

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем main
	main()

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что ошибка будет "insufficient arguments"
	expected := "Error: insufficient arguments\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

func TestExecuteQueueOperationsError(t *testing.T) {
	// Создаем временный файл, который не будет найден (не существует)
	nonExistentFile := "non_existent_file.txt"

	// Мокируем os.Args
	os.Args = []string{"program", "--file", nonExistentFile, "--query", "QPUSH test"}

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем main
	main()

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что ошибка будет "Cannot open file for reading"
	expected := fmt.Sprintf("Cannot open file for reading: %s\n", nonExistentFile)
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

func TestProcessCommandEmptyCommand(t *testing.T) {
	queue := &Queue{} // Создаем очередь

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем processCommand с пустой строкой
	processCommand("", queue)

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что будет напечатано "Error: Command is empty."
	expected := "Error: Command is empty.\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

// Тестируем команду QPUSH без аргумента
func TestProcessCommandQPUSHWithoutArgument(t *testing.T) {
	queue := &Queue{} // Создаем очередь

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем processCommand с командой QPUSH без аргумента
	processCommand("QPUSH", queue)

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что будет напечатано "Error: QPUSH command requires 1 argument."
	expected := "Error: QPUSH command requires 1 argument.\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}

// Тестируем команду с неизвестной командой
func TestProcessCommandUnknownCommand(t *testing.T) {
	queue := &Queue{} // Создаем очередь

	// Перехватываем стандартный вывод
	r, w, _ := os.Pipe()
	os.Stdout = w

	// Вызываем processCommand с неизвестной командой
	processCommand("QUNKNOWN", queue)

	// Закрываем запись в pipe
	_ = w.Close()

	// Читаем захваченный вывод
	var buf strings.Builder
	_, _ = io.Copy(&buf, r)

	// Ожидаем, что будет напечатано "Error: Unknown command: QUNKNOWN"
	expected := "Error: Unknown command: QUNKNOWN\n"
	if buf.String() != expected {
		t.Errorf("Expected output %q, got %q", expected, buf.String())
	}

	// Восстанавливаем стандартный вывод
	os.Stdout = os.NewFile(uintptr(syscall.Stdout), "/dev/stdout")
}
