package main

import (
	"fmt"
	"bytes"
	"encoding/gob"
	"os"
)

// Структура Node для элементов очереди
type Node struct {
	Data string
	Next *Node
}

// Структура Queue для управления очередью
type Queue struct {
	Front *Node
	End   *Node
	Size  int
}

// Метод Push добавляет элемент в очередь
func (q *Queue) Push(value string) {
	newNode := &Node{Data: value}
	if q.End == nil {
		q.Front = newNode
		q.End = newNode
	} else {
		q.End.Next = newNode
		q.End = newNode
	}
	q.Size++
}

func (q *Queue) Print() {
	temp := q.Front
	for temp != nil {
		fmt.Print(temp.Data, " ")
		temp = temp.Next
	}
	fmt.Println()
}
// Метод SaveToTextFile сохраняет очередь в текстовый файл
func (q *Queue) SaveToTextFile(filename string) {
	file, err := os.Create(filename)
	if err != nil {
		return
	}
	defer file.Close()

	temp := q.Front
	for temp != nil {
		_, err := file.WriteString(temp.Data + "\n")
		if err != nil {
			return
		}
		temp = temp.Next
	}
}

// Метод LoadFromTextFile загружает элементы в очередь из текстового файла
func (q *Queue) LoadFromTextFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		return
	}
	defer file.Close()

	q.Clear() // Очистить текущую очередь перед загрузкой
	var value string
	for {
		_, err := fmt.Fscanf(file, "%s\n", &value)
		if err != nil {
			break
		}
		q.Push(value)
	}
}

// Метод SaveToBinaryFile сохраняет очередь в бинарный файл
func (q *Queue) SaveToBinaryFile(filename string) {
	file, err := os.Create(filename)
	if err != nil {
		return
	}
	defer file.Close()

	var buf bytes.Buffer
	enc := gob.NewEncoder(&buf)
	err = enc.Encode(q)
	if err != nil {
		return
	}

	_, err = file.Write(buf.Bytes())
	if err != nil {
		return
	}
}

// Метод LoadFromBinaryFile загружает элементы в очередь из бинарного файла
func (q *Queue) LoadFromBinaryFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		return
	}
	defer file.Close()

	var buf bytes.Buffer
	_, err = buf.ReadFrom(file)
	if err != nil {
		return
	}

	dec := gob.NewDecoder(&buf)
	err = dec.Decode(q)
	if err != nil {
		return
	}
}

// Метод Clear очищает очередь
func (q *Queue) Clear() {
	q.Front = nil
	q.End = nil
	q.Size = 0
}

func main() {
	queue := &Queue{}
	queue.Push("first")
	queue.Push("second")
	queue.Push("third")

	// Сохранение и загрузка в текстовом формате
	fmt.Println("Queue before saving to text file:")
	queue.Print()
	queue.SaveToTextFile("queue_text.txt")
	queue.Clear()
	queue.LoadFromTextFile("queue_text.txt")
	fmt.Println("Queue after loading from text file:")
	queue.Print()

	// Сохранение и загрузка в бинарном формате
	fmt.Println("Queue before saving to binary file:")
	queue.Print()
	queue.SaveToBinaryFile("queue_binary.bin")
	queue.Clear()
	queue.LoadFromBinaryFile("queue_binary.bin")
	fmt.Println("Queue after loading from binary file:")
	queue.Print()
}
