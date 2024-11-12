#include <iostream> // 1
#include <string>

using namespace std;


struct HashNode {
    char key;
    char value;
    HashNode* next;

    HashNode(char k, char v) : key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
    static const int SIZE = 26;
    HashNode* table[SIZE];

    int hash(char key) {
        return key % SIZE;
    }

public:
    HashTable() {
        for (int i = 0; i < SIZE; ++i)
            table[i] = nullptr;
    }

    void insert(char key, char value) {
        int index = hash(key);
        HashNode* newNode = new HashNode(key, value);
        if (!table[index]) {
            table[index] = newNode;
        } else {
            HashNode* current = table[index];
            while (current->next)
                current = current->next;
            current->next = newNode;
        }
    }

    char get(char key) {
        int index = hash(key);
        HashNode* current = table[index];
        while (current) {
            if (current->key == key)
                return current->value;
            current = current->next;
        }
        return '\0';
    }

    bool contains(char key) {
        return get(key) != '\0';
    }
};

bool areIsomorphic(const string& str1, const string& str2) {
    if (str1.length() != str2.length())
        return false;

    HashTable map1, map2; // для коллизии 

    for (size_t i = 0; i < str1.length(); ++i) {
        char char1 = str1[i];
        char char2 = str2[i];

        //char1 -> char2
        if (map1.contains(char1)) {
            if (map1.get(char1) != char2)
                return false;
        } else {
            map1.insert(char1, char2);
        }

        //char2 -> char1
        if (map2.contains(char2)) {
            if (map2.get(char2) != char1)
                return false;
        } else {
            map2.insert(char2, char1);
        }
    }
    return true;
}

int main() {
    string str1 = "fall";
    string str2 = "redd";

    if (areIsomorphic(str1, str2)) {
        cout << "isomorphic" << endl;
    } else {
        cout << "not isomorphic" << endl;
    }

    return 0;
}
