#include <iostream>
#include <string>
#include <fstream>

using namespace std;

const int TABLE_SIZE = 11;

struct SetNode {
    int value;
    SetNode* next;
    SetNode(int val) : value(val), next(nullptr) {}
};

class CustomSet {
private:
    SetNode* table[TABLE_SIZE]; // Массив указателей для хранения цепочек

    int hashFunction(int value) const {
        return abs(value) % TABLE_SIZE; // Простая хеш-функция для вычисления индекса
    }

public:
    CustomSet() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            table[i] = nullptr; // Инициализация массива пустыми указателями
        }
    }

    ~CustomSet() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SetNode* node = table[i];
            while (node) {
                SetNode* temp = node;
                node = node->next;
                delete temp;
            }
        }
    }

    void add(int value) {
        int index = hashFunction(value);
        SetNode* node = table[index];
        while (node) {
            if (node->value == value) return; // Элемент уже есть в множестве
            node = node->next;
        }
        SetNode* newNode = new SetNode(value);
        newNode->next = table[index];
        table[index] = newNode;
    }

    void remove(int value) {
        int index = hashFunction(value);
        SetNode* node = table[index];
        SetNode* prev = nullptr;
        while (node) {
            if (node->value == value) {
                if (prev) prev->next = node->next;
                else table[index] = node->next;
                delete node;
                return;
            }
            prev = node;
            node = node->next;
        }
    }

    bool contains(int value) const {
        int index = hashFunction(value);
        SetNode* node = table[index];
        while (node) {
            if (node->value == value) return true;
            node = node->next;
        }
        return false;
    }

    void saveToFile(const string& filePath) const {
        ofstream file(filePath);
        if (!file) {
            cerr << "Error: Could not open file " << filePath << endl;
            return;
        }

        for (int i = 0; i < TABLE_SIZE; ++i) {
            SetNode* node = table[i];
            while (node) {
                file << node->value << endl; // Записываем каждое значение в файл
                node = node->next;
            }
        }

        file.close();
    }
};


void processQuery(CustomSet& mySet, const string& query, const string& filePath) {
    if (query.substr(0, 6) == "SETADD") {
        int value = stoi(query.substr(7));
        mySet.add(value);
        cout << "Added " << value << " to set." << endl;
        mySet.saveToFile(filePath); 
    } 
    else if (query.substr(0, 6) == "SETDEL") {
        int value = stoi(query.substr(7));
        mySet.remove(value);
        cout << "Deleted " << value << " from set." << endl;
        mySet.saveToFile(filePath); 
    } 
    else if (query.substr(0, 6) == "SET_AT") {
        int value = stoi(query.substr(7));
        if (mySet.contains(value)) {
            cout << value << " is in the set." << endl;
        } else {
            cout << value << " is not in the set." << endl;
        }
    } else {
        cout << "Invalid command." << endl;
    }
}

int main(int argc, char* argv[]) {
    string filePath = argv[2];
    string query = argv[4];

    CustomSet mySet;

    // Загрузка данных из файла
    ifstream file(filePath);
    if (!file) {
        cerr << "Error: Could not open file " << filePath << endl;
        return 1;
    }

    int value;
    while (file >> value) {
        mySet.add(value);
    }
    file.close();

    // Выполнение команды
    processQuery(mySet, query, filePath);

    return 0;
}
