package main

import (
	"fmt"
	"os"
	"strings"
)

// Структура Node для элементов очереди
type Node struct {
	data string
	next *Node
}

// Структура Queue для управления очередью
type Queue struct {
	front *Node
	end   *Node
	size  int
}

// Метод Push добавляет элемент в очередь
func (q *Queue) Push(value string) {
	newNode := &Node{data: value}
	if q.end == nil {
		q.front = newNode
		q.end = newNode
	} else {
		q.end.next = newNode
		q.end = newNode
	}
	q.size++
}

// Метод Pop удаляет элемент из очереди
func (q *Queue) Pop() {
	if q.front == nil {
		fmt.Println("Queue is empty.")
		return
	}
	temp := q.front
	q.front = q.front.next
	if q.front == nil {
		q.end = nil
	}
	q.size--
	fmt.Println("Popped:", temp.data)
}

// Метод Print выводит все элементы очереди
func (q *Queue) Print() {
	temp := q.front
	for temp != nil {
		fmt.Print(temp.data, " ")
		temp = temp.next
	}
	fmt.Println()
}

// Метод SaveToFile сохраняет очередь в файл
func (q *Queue) SaveToFile(filename string) {
	file, err := os.Create(filename)
	if err != nil {
		fmt.Println("Cannot open file for writing:", filename)
		return
	}
	defer file.Close()

	temp := q.front
	for temp != nil {
		_, err := fmt.Fprintln(file, temp.data)
		if err != nil {
			fmt.Println("Error writing to file:", err)
			return
		}
		temp = temp.next
	}
}

// Метод LoadFromFile загружает элементы в очередь из файла
func (q *Queue) LoadFromFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		fmt.Println("Cannot open file for reading:", filename)
		return
	}
	defer file.Close()

	var value string
	for {
		_, err := fmt.Fscanf(file, "%s\n", &value)
		if err != nil {
			break
		}
		q.Push(value)
	}
}

// Функция для обработки аргументов командной строки
func processArguments(args []string) (string, string, error) {
	if len(args) < 3 {
		return "", "", fmt.Errorf("insufficient arguments")
	}

	var filename string
	var query string

	for i := 1; i < len(args); i++ {
		arg := args[i]
		if arg == "--file" && i+1 < len(args) {
			filename = args[i+1]
			i++
		} else if arg == "--query" && i+1 < len(args) {
			query = args[i+1]
			i++
		}
	}

	if filename == "" || query == "" {
		return "", "", fmt.Errorf("missing required arguments")
	}

	return filename, query, nil
}

// Функция для выполнения операций с очередью
func executeQueueOperations(filename string, query string) error {
	queue := &Queue{}
	queue.LoadFromFile(filename)
	processCommand(query, queue)
	queue.SaveToFile(filename)
	return nil
}

// Функция для обработки команд
func processCommand(command string, queue *Queue) {
	tokens := strings.Fields(command)

	if len(tokens) == 0 {
		fmt.Println("Error: Command is empty.")
		return
	}

	switch tokens[0] {
	case "QPUSH":
		if len(tokens) == 2 {
			queue.Push(tokens[1])
		} else {
			fmt.Println("Error: QPUSH command requires 1 argument.")
		}
	case "QPOP":
		queue.Pop()
	case "QPRINT":
		queue.Print()
	default:
		fmt.Println("Error: Unknown command:", tokens[0])
	}
}

func main() {
	// Обрабатываем аргументы
	filename, query, err := processArguments(os.Args)
	if err != nil {
		fmt.Println("Error:", err)
		return // завершение без ошибки выхода
	}

	// Выполняем операции с очередью
	err = executeQueueOperations(filename, query)
	if err != nil {
		fmt.Println("Error:", err)
		return // завершение без ошибки выхода
	}
}

