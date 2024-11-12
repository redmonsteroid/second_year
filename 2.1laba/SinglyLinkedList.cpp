#include "node.hpp"
#include "mainHeader.hpp"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

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