#pragma once
#include <string>
#include "tablejson.hpp"

struct Node {
    std::string data;
    Node* next;

    Node(const std::string& value) : data(value), next(nullptr) {}
};

class DynamicArray {
private:
    std::string* data;  // Указатель на массив данных
    int capacity;       // Текущая емкость массива
    int size;           // Количество элементов в массиве

    void resize() {
        // Увеличиваем размер массива в 2 раза
        capacity *= 2;
        std::string* newData = new std::string[capacity];
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    DynamicArray(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        data = new std::string[capacity];
    }

    ~DynamicArray() {
        delete[] data;
    }

   void add(const std::string& value) {
    if (size == capacity) {
        resize();
    }
    data[size++] = value;
    std::cout << "[DEBUG] Added value: " << value << "\n";  // Отладочный вывод
}


    std::string& get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    int getSize() const {
        return size;
    }
};


void splitDot(const std::string& word, std::string& table, std::string& column, TableJson& tableJS);
int countCsv(TableJson& tableJS, const std::string& table);
void crossJoin(TableJson& tableJS, const std::string& table1, const std::string& table2, const std::string& column1, const std::string& column2);
void selectFromTables(const std::string& command, TableJson& tableJS);
