#include "mainHeader.hpp"
#include "node.hpp"
#include <iostream>
#include <fstream>
#include <string>
// Добавление узла
#include "mainHeader.hpp"

// Добавление узла
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
