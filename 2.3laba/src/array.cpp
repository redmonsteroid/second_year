#include "mainHeader.hpp"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

Array::Array(int capacity) {
    maxCapacity = capacity;
    size = 0;
    data = new string[maxCapacity]; 
}

Array::~Array() {
    delete[] data;
}

void Array::add(int index, string value) {
    if (index < 0 || index > size || size >= maxCapacity) {
        cout << "Index invalid or array is full" << endl;
        return;
    }
    // Сдвигаем элементы вправо начиная с указанного индекса
    for (int i = size; i > index; i--) {
        data[i] = data[i - 1];
    }
    data[index] = value; // Вставляем элемент
    size++;
}

void Array::addToTheEnd(string value) {
    if (size >= maxCapacity) {
        cout << "Array is full" << endl;
        return;
    }
    data[size] = value; // Вставляем элемент в конец
    size++;
}

void Array::remove(int index) {
    if (index < 0 || index >= size) {
        cout << "Index invalid" << endl;
        return;
    }
    // Сдвигаем элементы влево начиная с указанного индекса
    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}

void Array::replace(int index, string value) {
    if (index < 0 || index >= size) {
        cout << "Index invalid" << endl;
        return;
    }
    data[index] = value; // Замена элемента на заданном индексе
}

void Array::print() {
    for (int i = 0; i < size; i++) {
        cout << data[i] << " ";
    }
    cout << endl;
}

int Array::length() {
    return size;
}

void Array::saveToFile(const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Cannot open file for writing: " << filename << endl;
        return;
    }
    for (int i = 0; i < size; i++) {
        outFile << data[i] << endl;
    }
    outFile.close();
}

void Array::loadFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "Cannot open file for reading: " << filename << endl;
        return;
    }
    string line;
    size = 0;
    while (getline(inFile, line) && size < maxCapacity) {
        data[size] = line;
        size++;
    }
    inFile.close();
}

string Array::get(int index) {
    if (index < 0 || index >= size) {
        cout << "Index invalid" << endl;
        return "";
    }
    return data[index]; // Возвращаем элемент по индексу
}
