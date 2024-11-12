#include "mainHeader.hpp"
#include <fstream>
#include <sstream>
// Конструктор хеш-таблицы
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
            std::cout << "[" << i << "]: ";
            while (current != nullptr) {
                std::cout << current->key << " => " << current->value << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
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
