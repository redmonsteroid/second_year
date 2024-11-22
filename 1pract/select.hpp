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
    std::string* data;
    int capacity;
    int size;

public:
    // Конструктор
    DynamicArray(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        data = new std::string[capacity];
        std::cout << "[DEBUG] DynamicArray initialized with capacity: " << capacity << "\n";
    }

    // Деструктор
    ~DynamicArray() {
        delete[] data;
        std::cout << "[DEBUG] DynamicArray destroyed\n";
    }

    // Конструктор копирования
    DynamicArray(const DynamicArray& other) : capacity(other.capacity), size(other.size) {
        data = new std::string[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        std::cout << "[DEBUG] DynamicArray copied\n";
    }

    // Оператор присваивания
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;

            capacity = other.capacity;
            size = other.size;
            data = new std::string[capacity];
            for (int i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
        }
        std::cout << "[DEBUG] DynamicArray assigned\n";
        return *this;
    }

    // Добавление значения
    void add(const std::string& value) {
        if (size == capacity) {
            capacity *= 2;
            std::string* newData = new std::string[capacity];
            for (int i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }
        data[size++] = value;
        std::cout << "[DEBUG] Added value: " << value << "\n";
    }

    // Получение значения
    const std::string& get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    void remove(int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
    }

    // Размер массива
    int getSize() const {
        return size;
    }
};

#include <iostream>
#include <stdexcept>
#include <string>

class DynamicPairArray {
private:
    std::pair<std::string, std::string>* data;
    int capacity;
    int size;

public:
    // Конструктор
    DynamicPairArray(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        data = new std::pair<std::string, std::string>[capacity];
        std::cout << "[DEBUG] DynamicPairArray initialized with capacity: " << capacity << "\n";
    }

    // Деструктор
    ~DynamicPairArray() {
        delete[] data;
        std::cout << "[DEBUG] DynamicPairArray destroyed\n";
    }

    // Добавление пары значений
    void add(const std::string& first, const std::string& second) {
        if (size == capacity) {
            capacity *= 2;
            auto* newData = new std::pair<std::string, std::string>[capacity];
            for (int i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }
        data[size++] = std::make_pair(first, second);
        std::cout << "[DEBUG] Added pair to array: (" << first << ", " << second << ")\n";
    }

    // Удаление пары по индексу
    void remove(int index) {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        for (int i = index; i < size - 1; ++i) {
            data[i] = data[i + 1];
        }
        --size;
        std::cout << "[DEBUG] Removed pair at index " << index << "\n";
    }

    // Получение пары по индексу
    const std::pair<std::string, std::string>& get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    // Получение текущего размера массива
    int getSize() const {
        return size;
    }
};


void splitDot(const std::string& word, std::string& table, std::string& column, TableJson& tableJS);
int countCsv(TableJson& tableJS, const std::string& table);
void crossJoin(TableJson& tableJS, const std::string& table1, const std::string& table2, const std::string& column1, const std::string& column2);
void selectFromTables(const std::string& command, TableJson& tableJS);
