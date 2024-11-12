#pragma once
#include <string>

using namespace std;

struct Node {
    std::string data;
    Node* next;

    Node(const std::string& value, Node* nextNode = nullptr)
        : data(value), next(nextNode) {
    }
};

struct DoublyNode {
    std::string data;
    DoublyNode* next;
    DoublyNode* prev;

    DoublyNode(const std::string& value, DoublyNode* nextNode = nullptr, DoublyNode* prevNode = nullptr)
        : data(value), next(nextNode), prev(prevNode) {
    }
};

struct tree_node {
    int digit;
    tree_node* left;
    tree_node* right;

    tree_node(int dig) : digit(dig), left(nullptr), right(nullptr) {}
};

struct Queue_node {
    tree_node* tree;
    Queue_node* next;
};

struct queue_tree {
    Queue_node* front;
    Queue_node* rear;
    int count;

    queue_tree() : front(nullptr), rear(nullptr), count(0) {}

    bool is_empty() {
        return count == 0;
    }

    void enqueue(tree_node* node) {
        Queue_node* new_node = new Queue_node();
        new_node->tree = node;
        new_node->next = nullptr;

        if (rear == nullptr) {
            front = rear = new_node;
        } else {
            rear->next = new_node;
            rear = new_node;
        }

        count++;
    }

    tree_node* dequeue() {
        if (is_empty()) {
            return nullptr;
        }

        Queue_node* new_node = front;
        tree_node* res = front->tree;
        front = front->next;

        if (front == nullptr) {
            rear = nullptr;
        }

        delete new_node;
        count--;
        return res;
    }
};