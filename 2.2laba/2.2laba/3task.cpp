#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

const int TABLE_SIZE = 11;

struct SetNode {
    int value;
    SetNode* next;
    SetNode(int val) : value(val), next(nullptr) {}
};

class CustomSet {
private:
    SetNode* table[TABLE_SIZE];

    int hashFunction(int value) const {
        return abs(value) % TABLE_SIZE;
    }

public:
    CustomSet() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            table[i] = nullptr;
        }
    }

    ~CustomSet() {
        clear();
    }

    void add(int value) {
        if (contains(value)) return;
        int index = hashFunction(value);
        SetNode* newNode = new SetNode(value);
        newNode->next = table[index];
        table[index] = newNode;
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

    int sum() const {
        int total = 0;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SetNode* node = table[i];
            while (node) {
                total += node->value;
                node = node->next;
            }
        }
        return total;
    }

    void clear() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SetNode* node = table[i];
            while (node) {
                SetNode* temp = node;
                node = node->next;
                delete temp;
            }
            table[i] = nullptr;
        }
    }

    void print() const {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SetNode* node = table[i];
            while (node) {
                cout << node->value << " ";
                node = node->next;
            }
        }
    }
};

// Recursive function for finding partitions with minimal difference
void findMinDiff(CustomSet& set, int index, CustomSet& subset1, CustomSet& subset2, CustomSet& bestSubset1, CustomSet& bestSubset2, int& minDiff, int size, int* elements) {
    if (index == size) {
        int sum1 = subset1.sum();
        int sum2 = subset2.sum();
        int currentDiff = abs(sum1 - sum2);

        if (currentDiff < minDiff) {
            minDiff = currentDiff;
            bestSubset1.clear();
            bestSubset2.clear();
            for (int i = 0; i < size; ++i) {
                if (subset1.contains(elements[i])) bestSubset1.add(elements[i]);
                if (subset2.contains(elements[i])) bestSubset2.add(elements[i]);
            }
        }
        return;
    }

    subset1.add(elements[index]);
    findMinDiff(set, index + 1, subset1, subset2, bestSubset1, bestSubset2, minDiff, size, elements);
    subset1.remove(elements[index]);

    subset2.add(elements[index]);
    findMinDiff(set, index + 1, subset1, subset2, bestSubset1, bestSubset2, minDiff, size, elements);
    subset2.remove(elements[index]);
}

void partitionSet(CustomSet& set, int size, int* elements) {
    CustomSet subset1, subset2, bestSubset1, bestSubset2;
    int minDiff = 1234567890;

    findMinDiff(set, 0, subset1, subset2, bestSubset1, bestSubset2, minDiff, size, elements);

    cout << "First subset: ";
    bestSubset1.print();
    cout << endl;
    cout << "Second subset: ";
    bestSubset2.print();
    cout << endl;
    cout << "Difference: " << minDiff << endl;
}

int main() {
    CustomSet mySet;
    int elements[] = {5, 8, 1, 14, 7};
    int size = sizeof(elements) / sizeof(elements[0]);

    for (int i = 0; i < size; i++) {
        mySet.add(elements[i]);
    }

    partitionSet(mySet, size, elements);

    return 0;
}
