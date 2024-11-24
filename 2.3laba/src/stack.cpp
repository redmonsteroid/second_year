#include "mainHeader.hpp"
#include "node.hpp"
#include <iostream>
#include <fstream>

using namespace std;
// last in first out
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