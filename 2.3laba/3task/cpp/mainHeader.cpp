#include "mainHeader.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include "node.hpp"
#include <sstream>
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

void CBTree::insert(int digit) {
    tree_node* new_node = new tree_node(digit);
    if (root == nullptr) {
        root = new_node;
        return;
    }
    queue_tree Q;
    Q.enqueue(root);
    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();
        if (current->left == nullptr) {
            current->left = new_node;
            return;
        } else {
            Q.enqueue(current->left);
        }
        if (current->right == nullptr) {
            current->right = new_node;
            return;
        } else {
            Q.enqueue(current->right);
        }
    }
}

// Проверка на полное дерево
bool CBTree::is_CBT() {
    if (root == nullptr) {
        std::cout << "Tree is empty.\n";
        return false;
    }

    queue_tree Q;
    Q.enqueue(root);
    bool non_full_node = false;
    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();

        if (current->left) {
            if (non_full_node) {
                return false;
            }
            Q.enqueue(current->left);
        } else {
            non_full_node = true;
        }

        if (current->right) {
            if (non_full_node) {
                return false;
            }
            Q.enqueue(current->right);
        } else {
            non_full_node = true;
        }
    }
    return true;
}

// Поиск элемента по значению
bool CBTree::find_value(tree_node* current, int value) {
    if (current == nullptr) {
        return false;
    }
    if (current->digit == value) {
        return true;
    }
    return find_value(current->left, value) || find_value(current->right, value);
}

bool CBTree::get_value(int value) {
    if (root == nullptr) {
        std::cout << "Tree is empty.\n";
        return false;
    }
    return find_value(root, value);
}

// Поиск по индексу
void CBTree::find_index(int index) {
    if (index < 0) {
        std::cout << "Invalid index.\n";
        return;
    }

    if (root == nullptr) {
        std::cout << "Tree is empty.\n";
        return;
    }

    queue_tree Q;
    Q.enqueue(root);
    int currentIndex = 0;

    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();
        if (currentIndex == index) {
            std::cout << "Value: " << current->digit << std::endl;
            return;
        }
        currentIndex++;
        if (current->left) {
            Q.enqueue(current->left);
        }
        if (current->right) {
            Q.enqueue(current->right);
        }
    }
    std::cout << "Value not found.\n";
}

// Вывод обхода
void CBTree::print_CBT(tree_node* current, int level) {
    if (current) {
        print_CBT(current->right, level + 1);
        for (int i = 0; i < level; i++) {
            std::cout << "   ";
        }
        std::cout << current->digit << std::endl;
        print_CBT(current->left, level + 1);
    }
}

// Вывод в консоль
void CBTree::display() {
    if (root == nullptr) {
        std::cout << "Tree is empty.\n";
        return;
    }
    print_CBT(root, 0);
}

// Очистка дерева
void CBTree::clear(tree_node*& node) {
    if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
        node = nullptr;
    }
}

void CBTree::saveToBinaryFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cout << "Cannot open file for binary writing: " << filename << std::endl;
        return;
    }

    queue_tree Q;
    Q.enqueue(root);
    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();
        bool isNode = (current != nullptr);
        outFile.write(reinterpret_cast<const char*>(&isNode), sizeof(isNode));

        if (isNode) {
            outFile.write(reinterpret_cast<const char*>(&current->digit), sizeof(current->digit));
            Q.enqueue(current->left);
            Q.enqueue(current->right);
        }
    }
    outFile.close();
}


void CBTree::loadFromBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cout << "Cannot open file for binary reading: " << filename << std::endl;
        return;
    }

    clear(root);  // Очищаем текущее дерево
    bool isNode;
    inFile.read(reinterpret_cast<char*>(&isNode), sizeof(isNode));

    if (!isNode) {
        root = nullptr;
        return;
    }

    int rootValue;
    inFile.read(reinterpret_cast<char*>(&rootValue), sizeof(rootValue));
    root = new tree_node(rootValue);

    queue_tree Q;
    Q.enqueue(root);

    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();

        // Левый узел
        inFile.read(reinterpret_cast<char*>(&isNode), sizeof(isNode));
        if (isNode) {
            int leftValue;
            inFile.read(reinterpret_cast<char*>(&leftValue), sizeof(leftValue));
            current->left = new tree_node(leftValue);
            Q.enqueue(current->left);
        } else {
            current->left = nullptr;
        }

        // Правый узел
        inFile.read(reinterpret_cast<char*>(&isNode), sizeof(isNode));
        if (isNode) {
            int rightValue;
            inFile.read(reinterpret_cast<char*>(&rightValue), sizeof(rightValue));
            current->right = new tree_node(rightValue);
            Q.enqueue(current->right);
        } else {
            current->right = nullptr;
        }
    }
    inFile.close();
}


// Чтение из файла
void CBTree::load_from_file(const std::string& file) {
    clear(root);
    std::ifstream load(file);
    if (!load) {
        std::cout << "Failed to open file.\n";
        return;
    }

    int value;
    while (load >> value) {
        insert(value);
    }
    load.close();
}

// Запись в файл
void CBTree::save_to_file(const std::string& file) {
    std::ofstream save(file);
    if (!save) {
        std::cout << "Failed to open file.\n";
        return;
    }

    queue_tree Q;
    Q.enqueue(root);
    while (!Q.is_empty()) {
        tree_node* current = Q.dequeue();
        save << current->digit << std::endl;
        if (current->left) {
            Q.enqueue(current->left);
        }
        if (current->right) {
            Q.enqueue(current->right);
        }
    }
    save.close();
}

void DoublyLinkedList::addToHead(std::string value) {
    DoublyNode* newNode = new DoublyNode(value);
    if (head == nullptr) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
}

void DoublyLinkedList::addToTail(std::string value) {
    DoublyNode* newNode = new DoublyNode(value);
    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        newNode->prev = tail;
        tail->next = newNode;
        tail = newNode;
    }
}

void DoublyLinkedList::removeFromHead() {
    if (head == nullptr) {
        return;
    }
    DoublyNode* temp = head;
    head = head->next;
    if (head != nullptr) {
        head->prev = nullptr;
    } else {
        tail = nullptr;
    }
    delete temp;
}

void DoublyLinkedList::removeFromTail() {
    if (tail == nullptr) {
        return;
    }
    DoublyNode* temp = tail;
    tail = tail->prev;
    if (tail != nullptr) {
        tail->next = nullptr;
    } else {
        head = nullptr;
    }
    delete temp;
}

void DoublyLinkedList::removeByValue(std::string value) {
    DoublyNode* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            if (current == head) {
                removeFromHead();
            } else if (current == tail) {
                removeFromTail();
            } else {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                delete current;
            }
            return;
        }
        current = current->next;
    }
}

DoublyNode* DoublyLinkedList::search(string value) {
    DoublyNode* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void DoublyLinkedList::print() {
    DoublyNode* current = head;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}

void DoublyLinkedList::clear() {
    while (head) {
        removeFromHead();
    }
}

void DoublyLinkedList::saveToBinaryFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cout << "Cannot open file for binary writing: " << filename << std::endl;
        return;
    }
    DoublyNode* current = head;
    while (current) {
        size_t len = current->data.size();
        outFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
        outFile.write(current->data.c_str(), len);
        current = current->next;
    }
    outFile.close();
}

void DoublyLinkedList::loadFromBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cout << "Cannot open file for binary reading: " << filename << std::endl;
        return;
    }
    clear(); // Очищаем текущий список перед загрузкой
    while (true) {
        size_t len;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (inFile.eof()) break; // Проверка конца файла
        std::string value(len, '\0');
        inFile.read(&value[0], len);
        addToTail(value);
    }
    inFile.close();
}



void DoublyLinkedList::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    DoublyNode* current = head;
    while (current != nullptr) {
        file << current->data << " ";
        current = current->next;
    }
    file.close();
}

void DoublyLinkedList::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string value;
        while (file >> value) {
            addToTail(value);
        }
        file.close();
    } else {
        cout << "Unable to open file." << endl;
    }
}
HashTable::HashTable(int size) {
    capacity = size;
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
    }
}

// Деструктор хеш-таблицы
HashTable::~HashTable() {
    clear();  // Очищаем всю таблицу при удалении
    delete[] table;
}

// Функция для вставки значения по ключу
void HashTable::hset(const std::string& key, const std::string& value) {
    int index = hashFunction(key); // Получаем индекс для ключа
    HashNode* current = table[index]; // Получаем узел по индексу

    // Проходим по списку в ячейке хэш-таблицы
    while (current != nullptr) {
        // Если ключ найден, обновляем значение
        if (current->key == key) {
            current->value = value;
            return; // Выходим из функции, так как значение обновлено
        }
        current = current->next; // Переходим к следующему узлу
    }

    // Если ключ не найден, добавляем новый узел в начало списка
    HashNode* newNode = new HashNode(key, value);
    newNode->next = table[index]; // Устанавливаем следующий узел
    table[index] = newNode; // Устанавливаем новый узел как первый в списке
}


// Функция для получения значения по ключу
void HashTable::hget(const std::string& key) const {
    int index = hashFunction(key);
    HashNode* current = table[index];
    while (current != nullptr) {
        if (current->key == key) {
            std::cout << "Value for key [" << key << "]: " << current->value << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "Key [" << key << "] not found." << std::endl;
}

// Функция для удаления элемента по ключу
void HashTable::hdel(const std::string& key) {
    int index = hashFunction(key);
    HashNode* current = table[index];
    HashNode* prev = nullptr;

    while (current != nullptr && current->key != key) {
        prev = current;
        current = current->next;
    }

    if (current == nullptr) {
        std::cout << "Key [" << key << "] not found for deletion." << std::endl;
        return;
    }

    if (prev == nullptr) {
        table[index] = current->next;
    } else {
        prev->next = current->next;
    }

    delete current;
}

// Функция для очистки хеш-таблицы
void HashTable::clear() {
    for (int i = 0; i < capacity; ++i) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
}

// Хэш-функция для ключа
int HashTable::hashFunction(const std::string& key) const {
    int hash = 0;
    for (char ch : key) {
        hash = (hash * 31 + ch) % capacity;  // Простая хэш-функция
    }
    return hash;
}

// Функция для печати содержимого хеш-таблицы
void HashTable::hprint() const {
    for (int i = 0; i < capacity; ++i) {
        HashNode* current = table[i];
        if (current != nullptr) {
            
            while (current != nullptr) {
                std::cout << current->key << " => " << current->value << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
}

void HashTable::saveToBinaryFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cout << "Cannot open file for binary writing: " << filename << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
    for (int i = 0; i < capacity; ++i) {
        int chainLength = 0;
        HashNode* node = table[i];
        while (node) {
            ++chainLength;
            node = node->next;
        }
        outFile.write(reinterpret_cast<const char*>(&chainLength), sizeof(chainLength));
        node = table[i];
        while (node) {
            size_t keySize = node->key.size();
            size_t valueSize = node->value.size();
            outFile.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
            outFile.write(node->key.c_str(), keySize);
            outFile.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
            outFile.write(node->value.c_str(), valueSize);
            node = node->next;
        }
    }
    outFile.close();
}


void HashTable::loadFromBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cout << "Cannot open file for binary reading: " << filename << std::endl;
        return;
    }
    clear();
    inFile.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
    table = new HashNode*[capacity];
    for (int i = 0; i < capacity; ++i) {
        table[i] = nullptr;
        int chainLength;
        inFile.read(reinterpret_cast<char*>(&chainLength), sizeof(chainLength));
        HashNode* prev = nullptr;
        for (int j = 0; j < chainLength; ++j) {
            size_t keySize, valueSize;
            inFile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
            std::string key(keySize, '\0');
            inFile.read(&key[0], keySize);

            inFile.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
            std::string value(valueSize, '\0');
            inFile.read(&value[0], valueSize);

            HashNode* newNode = new HashNode(key, value);
            if (!table[i]) {
                table[i] = newNode;
            } else {
                prev->next = newNode;
            }
            prev = newNode;
        }
    }
    inFile.close();
}


// Функция для загрузки данных из файла
void HashTable::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        clear();  // Очищаем текущую хеш-таблицу перед загрузкой

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string key, value;
            ss >> key >> value;
            hset(key, value);
        }

        file.close();
    } else {
        std::cerr << "Error: could not open file " << filename << std::endl;
    }
}

// Функция для сохранения данных в файл
void HashTable::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < capacity; ++i) {
            HashNode* current = table[i];
            while (current != nullptr) {
                file << current->key << " " << current->value << std::endl;
                current = current->next;
            }
        }
        file.close();
    } else {
        std::cerr << "Error: could not open file " << filename << std::endl;
    }
}

void Queue::push(string value) {
    Node* newNode = new Node(value);
    if (end == nullptr) {
        front = end = newNode;
    } else {
        end->next = newNode;
        end = newNode;
    }
    size++;
}

void Queue::pop() {
    if (front == nullptr) {
        cout << "Queue is empty." << endl;
        return;
    }
    Node* temp = front;
    front = front->next;
    if (front == nullptr) {
        end = nullptr;
    }
    delete temp;
    size--;
}

void Queue::print() {
    Node* temp = front;
    while (temp != nullptr) {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}
void Queue::saveToFile(const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Cannot open file for writing: " << filename << endl;
        return;
    }
    Node* temp = front;
    while (temp) {
        outFile << temp->data << endl;
        temp = temp->next;
    }
    outFile.close();
}

void Queue::loadFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "Cannot open file for reading: " << filename << endl;
        return;
    }
    string line;
    while (getline(inFile, line)) {
        push(line);
    }
    inFile.close();
}

void Queue::clear() {
    while (front != nullptr) {
        Node* temp = front;
        front = front->next;
        delete temp;
    }
    end = nullptr;
    size = 0;
}


void SinglyLinkedList::addToHead(string value){
    Node* newNode = new Node(value);
    newNode->next = head; // н узел указывает на голову
    head = newNode;
    size++;
}

void SinglyLinkedList::addToTail(std::string value) {
    Node* newNode = new Node(value);
    if (head == nullptr) {
        head = newNode;
    } else {
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    size++;
}

void SinglyLinkedList::removeHead() {
    if (head == nullptr) {
        return;
    }
    Node* temp = head;
    head = head->next;
    delete temp;
    size--;
}

void SinglyLinkedList::removeTail() {
    if (head == nullptr) {
        return;
    }
    if (head->next == nullptr) {
        delete head;
        head = nullptr;
    } else {
        Node* current = head;
        while (current->next->next != nullptr) {
            current = current->next;
        }
        delete current->next;
        current->next = nullptr;
    }
    size--;
}

void SinglyLinkedList::removeByValue(std::string value) {
    if (head == nullptr) {
        return;
    }
    if (head->data == value) {
        removeHead();
        return;
    }
    Node* current = head;
    while (current->next != nullptr) {
        if (current->next->data == value) {
            Node* temp = current->next;
            current->next = temp->next;
            delete temp;
            size--;
            return;
        }
        current = current->next;
    }
}

Node* SinglyLinkedList::search(std::string value) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data == value) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void SinglyLinkedList::print() {
    Node* current = head;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->next;
    }
    std::cout << std::endl;
}


void SinglyLinkedList::clear() {
    while (head) {
        removeHead();
    }
}

void SinglyLinkedList::saveToBinaryFile(const string& filename) const {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        cout << "Cannot open file for binary writing: " << filename << endl;
        return;
    }
    Node* current = head;
    while (current) {
        size_t len = current->data.size();
        outFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
        outFile.write(current->data.c_str(), len);
        current = current->next;
    }
    outFile.close();
}

void SinglyLinkedList::loadFromBinaryFile(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "Cannot open file for binary reading: " << filename << endl;
        return;
    }
    clear(); // Очистить текущий список перед загрузкой
    while (!inFile.eof()) {
        size_t len;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (inFile.eof()) break; // Проверка конца файла
        string value(len, '\0');
        inFile.read(&value[0], len);
        addToTail(value);
    }
    inFile.close();
}



void SinglyLinkedList::saveToFile(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        Node* current = head;
        while (current != nullptr) {
            file << current->data << endl;
            current = current->next;
        }
        file.close();
    } else {
        cout << " Unable to open file " << filename << endl;
    }
}



void SinglyLinkedList::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string value;
        while (getline(file, value)) {
            addToTail(value); // Добавляем каждый элемент в хвост
        }
        file.close();
    } else {
        cout << " Unable to open file " << filename << endl;
    }
}

void Stack::push(string value) {
    Node* newNode = new Node(value);
    newNode->next = top;
    top = newNode;
    size++;
}

void Stack::pop() {
    if (top == nullptr) {
        cout << "Stack is empty." << endl;
        return;
    }
    Node* temp = top;
    top = top->next;
    delete temp;
    size--;
}

void Stack::print() {
    Node* temp = top;
    while (temp != nullptr) {
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}

void Stack::clear() {
    while (top) {
        pop();
    }
}

void Stack::saveToBinaryFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cout << "Cannot open file for binary writing: " << filename << std::endl;
        return;
    }
    Node* current = top;
    while (current) {
        size_t len = current->data.size();
        outFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
        outFile.write(current->data.c_str(), len);
        current = current->next;
    }
    outFile.close();
}

// Stack Binary Deserialization
void Stack::loadFromBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cout << "Cannot open file for binary reading: " << filename << std::endl;
        return;
    }
    clear(); // Очистить текущий стек перед загрузкой
    while (!inFile.eof()) {
        size_t len;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (inFile.eof()) break; // Проверка конца файла
        std::string value(len, '\0');
        inFile.read(&value[0], len);
        push(value); // Вставляем элемент сверху стека
    }
    inFile.close();
}


void Stack::saveToFile(const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Cannot open file for writing: " << filename << endl;
        return;
    }
    Node* temp = top;
    while (temp) {
        outFile << temp->data << endl;
        temp = temp->next;
    }
    outFile.close();
}

void Stack::loadFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "Cannot open file for reading: " << filename << endl;
        return;
    }
    string line;
    while (getline(inFile, line)) {
        push(line);
    }
    inFile.close();
}


//сериализация/десериализация 
void Array::saveToBinaryFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cout << "Cannot open file for binary writing: " << filename << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    outFile.write(reinterpret_cast<const char*>(&maxCapacity), sizeof(maxCapacity));
    for (int i = 0; i < size; ++i) {
        size_t len = data[i].size();
        outFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
        outFile.write(data[i].c_str(), len);
    }
    outFile.close();
}


void Array::loadFromBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cout << "Cannot open file for binary reading: " << filename << std::endl;
        return;
    }
    inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
    inFile.read(reinterpret_cast<char*>(&maxCapacity), sizeof(maxCapacity));
    delete[] data;
    data = new std::string[maxCapacity];
    for (int i = 0; i < size; ++i) {
        size_t len;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        data[i].resize(len);
        inFile.read(&data[i][0], len);
    }
    inFile.close();
}


void Queue::saveToBinaryFile(const string& filename) const {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        cout << "Cannot open file for binary writing: " << filename << endl;
        return;
    }
    Node* temp = front;
    while (temp) {
        size_t len = temp->data.size();
        outFile.write(reinterpret_cast<const char*>(&len), sizeof(len));
        outFile.write(temp->data.c_str(), len);
        temp = temp->next;
    }
    outFile.close();
}

void Queue::loadFromBinaryFile(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cout << "Cannot open file for binary reading: " << filename << endl;
        return;
    }
    clear();  // Очистить текущую очередь перед загрузкой
    while (!inFile.eof()) {
        size_t len;
        inFile.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (inFile.eof()) break; // Проверка конца файла
        string value(len, '\0');
        inFile.read(&value[0], len);
        push(value);
    }
    inFile.close();
}
