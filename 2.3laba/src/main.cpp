#include <iostream>
#include <sstream>
#include <vector>
#include "mainHeader.hpp"
#include <fstream>

using namespace std;

// Функция для обработки команд
void processQuery(const string& query, Array& array, Stack& stack, Queue& queue, SinglyLinkedList& singlyList, DoublyLinkedList& doublyList, HashTable& hashTable, CBTree& cbTree) {
    vector<string> tokens;
    stringstream ss(query);
    string token;

    while (getline(ss, token, ' ')) {
        tokens.push_back(token);
    }

    // Массив (Array)
    if (tokens[0] == "MPUSH") {
        if (tokens.size() == 3) {
            int index = stoi(tokens[1]);
            string value = tokens[2];
            array.add(index, value);
        } else {
            cout << "Error: MPUSH command requires 2 arguments." << endl;
        }
    } else if (tokens[0] == "MDEL") {
        if (tokens.size() == 2) {
            int index = stoi(tokens[1]);
            array.remove(index);
        } else {
            cout << "Error: MDEL command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "MGET") {
        if (tokens.size() == 2) {
            int index = stoi(tokens[1]);
            string value = array.get(index);
            cout << "Element at index " << index << ": " << value << endl;
        } else {
            cout << "Error: MGET command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "MADD") {
        if (tokens.size() == 2) {
            string value = tokens[1];
             array.addToTheEnd(value);
        } else{
             cout << "Error: MADD command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "MREPLACE") {
        if (tokens.size() == 3) {
            int index = stoi(tokens[1]);
            string value = tokens[2];
            array.replace(index, value);
        } else {
            cout << "Error: MREPLACE command requires 2 arguments." << endl;
        }
    }

    // Стек (Stack)
    else if (tokens[0] == "SPUSH") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            stack.push(value);
        } else {
            cout << "Error: SPUSH command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "SPOP") {
        stack.pop();
    }

    // Очередь (Queue)
    else if (tokens[0] == "QPUSH") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            queue.push(value);
        } else {
            cout << "Error: QPUSH command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "QPOP") {
        queue.pop();
    }

    // Односвязный список (Singly Linked List)
    else if (tokens[0] == "LSADDHEAD") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            singlyList.addToHead(value);
        } else {
            cout << "Error: LSADDHEAD requires 1 argument." << endl;
        }
    } else if (tokens[0] == "LSADDTAIL") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            singlyList.addToTail(value);
        } else {
            cout << "Error: LSADDTAIL requires 1 argument." << endl;
        }
    } else if (tokens[0] == "LSDELHEAD") {
        singlyList.removeHead();
    } else if (tokens[0] == "LSDELTAIL") {
        singlyList.removeTail();
    } else if (tokens[0] == "LSDELVALUE"){
        if  (tokens.size() == 2) {
            string value = tokens[1];
            singlyList.removeByValue(value);
        } else {
            cout << "Error: LSDELVALUE requires 1 argument." << endl;
        }
    }

    // Двусвязный список (Doubly Linked List)
   else if (tokens[0] == "LDADDHEAD") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            doublyList.addToHead(value);
        } else {
            cout << "Error: LDADDHEAD requires 1 argument." << endl;
        }
    } else if (tokens[0] == "LDADDTAIL") {
        if (tokens.size() == 2) {
            string value = tokens[1];
            doublyList.addToTail(value);
        } else {
            cout << "Error: LDADDTAIL requires 1 argument." << endl;
        }
    } else if (tokens[0] == "LDDELHEAD") {
        doublyList.removeFromHead();
    } else if (tokens[0] == "LDDELTAIL") {
        doublyList.removeFromTail();
    } else if (tokens[0] == "LDDELVALUE"){
        if  (tokens.size() == 2) {
            string value = tokens[1];
            doublyList.removeByValue(value);
        } else {
            cout << "Error: LDDELVALUE requires 1 argument." << endl;
        }
    }
    // Хэш-таблица (HashTable)
    else if (tokens[0] == "HSET") {
        if (tokens.size() == 3) {
            string key = tokens[1];
            string value = tokens[2];
            hashTable.hset(key, value);
        } else {
            cout << "Error: HSET command requires 2 arguments." << endl;
        }
    } else if (tokens[0] == "HGET") {
        if (tokens.size() == 2) {
            string key = tokens[1];
            hashTable.hget(key);
        }
    } else if (tokens[0] == "HDEL") {
        if (tokens.size() == 2) {
            string key = tokens[1];
            hashTable.hdel(key);
        } else {
            cout << "Error: HDEL command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "HPRINT") {
        hashTable.hprint();
    }

    // Полное бинарное дерево (CBTree)
    else if (tokens[0] == "TINSERT") {
        if (tokens.size() == 2) {
            int digit = stoi(tokens[1]);
            cbTree.insert(digit);
        } else {
            cout << "Error: TINSERT command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "TISCBT") {
        if (cbTree.is_CBT()) {
            cout << "The tree is a complete binary tree." << endl;
        } else {
            cout << "The tree is not a complete binary tree." << endl;
        }
    } else if (tokens[0] == "TFIND") {
        if (tokens.size() == 2) {
            int value = stoi(tokens[1]);
            if (cbTree.get_value(value)) {
                cout << "Value " << value << " found in the tree." << endl;
            } else {
                cout << "Value " << value << " not found in the tree." << endl;
            }
        } else {
            cout << "Error: CBFIND command requires 1 argument." << endl;
        }
    } else if (tokens[0] == "TDISPLAY") {
        cbTree.display();
    }

    // Общая операция PRINT для всех структур данных
    else if (tokens[0] == "PRINT") {
        array.print();
        stack.print();
        queue.print();
        singlyList.print();
        doublyList.print();
        hashTable.hprint();
    } else {
        cout << "Unknown command: " << tokens[0] << endl;
    }
}

int main(int argc, char* argv[]) {
    string query;
    string filename;
    Array array(10);   // Массив
    Stack stack;       // Стек
    Queue queue;       // Очередь
    SinglyLinkedList singlyList;   // Односвязный список
    DoublyLinkedList doublyList;   // Двусвязный список
    HashTable hashTable(10); // Хэш-таблица
    CBTree cbTree;           // Полное бинарное дерево

    // Чтение аргументов командной строки
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filename = argv[++i];
        }
        if (arg == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
    }

    // Логика загрузки данных для конкретной структуры данных
    if (!filename.empty() && !query.empty()) {
        stringstream ss(query);
        string command;
        ss >> command;

        // Проверка команд для массива
        if (command[0] == 'M') {
            array.loadFromFile(filename);
        }
        // Проверка команд для стека
        else if (command[0] == 'S') {
            stack.loadFromFile(filename);
        }
        // Проверка команд для очереди
        else if (command[0] == 'Q') {
            queue.loadFromFile(filename);
        }
        // Проверка команд для односвязного списка
        else if (command[0] == 'L' && command[1] == 'S') {
            singlyList.loadFromFile(filename);
        }
        // Проверка команд для двусвязного списка
        else if (command[0] == 'L' && command[1] == 'D') {
            doublyList.loadFromFile(filename);
        }
        // Проверка команд для хэш-таблицы
        else if (command[0] == 'H') {
            hashTable.loadFromFile(filename);
        }
        // Проверка команд для полного бинарного дерева
        else if (command[0] == 'T') {
            cbTree.load_from_file(filename);
        }
        else if (command == "PRINT") {
            std::ifstream file(filename);
            if (!file.is_open()) {
            std::cerr << "Error: could not open file " << filename << std::endl;
            return 1;}
            std::string line;
            while (std::getline(file, line)) {
            std::cout << line << std::endl;}
            file.close(); // Закрытие файла
        } else {
            cout << "Error: unrecognized command type." << endl;
            return 1;
        }
    }

    // Выполнение запроса
    if (!query.empty()) {
        processQuery(query, array, stack, queue, singlyList, doublyList, hashTable, cbTree);
    } else {
        cout << "Error: query not specified." << endl;
        return 1;
    }

    // Логика сохранения данных для конкретной структуры данных
    if (!filename.empty() && !query.empty()) {
        stringstream ss(query);
        string command;
        ss >> command;

        // Проверка команд для массива
        if (command[0] == 'M') {
            array.saveToFile(filename);
        }
        // Проверка команд для стека
        else if (command[0] == 'S') {
            stack.saveToFile(filename);
        }
        // Проверка команд для очереди
        else if (command[0] == 'Q') {
            queue.saveToFile(filename);
        }
        // Проверка команд для односвязного списка
        else if (command[0] == 'L' && command[1] == 'S') {
            singlyList.saveToFile(filename);
        }
        // Проверка команд для двусвязного списка
        else if (command[0] == 'L' && command[1] == 'D') {
            doublyList.saveToFile(filename);
        }
        // Проверка команд для хэш-таблицы
        else if (command[0] == 'H') {
            hashTable.saveToFile(filename);
        }
        // Проверка команд для полного бинарного дерева
        else if (command[0] == 'T') {
            cbTree.save_to_file(filename);
        }
    }

    return 0;
}
