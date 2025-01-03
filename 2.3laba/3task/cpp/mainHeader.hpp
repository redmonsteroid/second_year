#pragma once
#include <iostream>
#include "node.hpp"
using namespace std;

class Array {
public:
    string* data;    // Массив строк
    int size;        // Текущий размер массива
    int maxCapacity; // Максимальная вместимость

    // Конструкторы
    Array(int max);
    ~Array();

    void add(int index, string value);
    void addToTheEnd(string value);
    string get(int index);
    void remove(int index);
    void replace(int index, string value);
    int length();
    void print();
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);

    // Методы для бинарной сериализации и десериализации
    void saveToBinaryFile(const string& filename) const;
    void loadFromBinaryFile(const string& filename);
};

class SinglyLinkedList {
    public:
    Node *head;
    int size;

    SinglyLinkedList() : head(nullptr), size(0) {}

    void addToHead(string value);
    void addToTail(string value);
    void removeHead();
    void removeTail();
    void removeByValue(string value);
    Node* search(string value);
    void print();
    void clear();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void saveToBinaryFile(const std::string& filename) const;  // Сериализация
    void loadFromBinaryFile(const std::string& filename);       // Десериализация
};


class DoublyLinkedList {
    public:
    DoublyNode* head;
    DoublyNode* tail;
    int size;

    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    void addToHead(string value);
    void addToTail(string value);
    void removeFromHead();
    void removeFromTail();
    void removeByValue(string value);
    DoublyNode* search(string value);
    void print();
    void clear();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void saveToBinaryFile(const std::string& filename) const;
    void loadFromBinaryFile(const std::string& filename);
};



class Stack {
    public:
    Node *top;
    int size;

    Stack() : top(nullptr), size(0) {}

    void push(string value);
    void pop();
    void print();
    void clear();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void saveToBinaryFile(const std::string& filename) const;
    void loadFromBinaryFile(const std::string& filename);
};


class Queue {
    public:
    Node *front;
    Node *end;
    int size;

    Queue() : front(nullptr), end(nullptr), size(0) {}

    void push(string value);
    void pop();
    void print();
    void clear();  // Объявление метода clear
    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);
    void saveToBinaryFile(const string& filename) const;
    void loadFromBinaryFile(const string& filename);
};


class HashNode {
public:
    std::string key;
    std::string value;
    HashNode* next;

    HashNode(const std::string& k, const std::string& v, HashNode* nextNode = nullptr)
        : key(k), value(v), next(nextNode) {}
};

// Класс HashTable для реализации хеш-таблицы
class HashTable {
public:
    HashTable(int size = 10);
    ~HashTable();

    void hset(const std::string& key, const std::string& value);
    void hget(const std::string& key) const;
    void hdel(const std::string& key);
    void clear();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);
    void hprint() const;
    void saveToBinaryFile(const std::string& filename) const;
    void loadFromBinaryFile(const std::string& filename);

private:
    int hashFunction(const std::string& key) const;
    HashNode** table;
    int capacity;
};


class CBTree {
    public:
    tree_node* root;
    CBTree() : root(nullptr) {}

    void insert(int digit);
    bool is_CBT();
    bool get_value(int value);
    void find_index(int index);
    bool find_value(tree_node* current, int value);
    void display();
    void load_from_file(const std::string& file);
    void save_to_file(const std::string& file);
    void clear(tree_node*& node);
    void print_CBT(tree_node* current, int level);
    void saveToBinaryFile(const std::string& filename) const;
    void loadFromBinaryFile(const std::string& filename);
};
