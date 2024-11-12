#include <iostream> //2
#include <cmath>

using namespace std;


struct Node {
    int data;
    Node* next;
    Node(int value) : data(value), next(nullptr) {}
};

struct LinkedList {
    Node* head;
    LinkedList() : head(nullptr) {}

    void add(int value) {
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
    }

    int sum() const {
        int total = 0;
        Node* current = head;
        while (current) {
            total += current->data;
            current = current->next;
        }
        return total;
    }

    void print() const {
        Node* current = head;
        while (current) {
            cout << current->data << " ";
            current = current->next;
        }
    }

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    int get(int index) const {
        Node* current = head;
        int i = 0;
        while (current) {
            if (i == index) return current->data;
            current = current->next;
            i++;
        }
        return 0; 
    }

    int size() const {
        int count = 0;
        Node* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
};

// Рекурсивная функция для нахождения подмножеств с минимальной разницей
void findMinDiff(LinkedList& list, int index, LinkedList& subset1, LinkedList& subset2, LinkedList& bestSubset1, LinkedList& bestSubset2, int& minDiff) {
    if (index == list.size()) {
        int sum1 = subset1.sum();
        int sum2 = subset2.sum();
        int currentDiff = abs(sum1 - sum2);

        if (currentDiff < minDiff) {
            minDiff = currentDiff;
            bestSubset1.clear();
            bestSubset2.clear();

            Node* node = subset1.head;
            while (node) {
                bestSubset1.add(node->data);
                node = node->next;
            }

            node = subset2.head;
            while (node) {
                bestSubset2.add(node->data);
                node = node->next;
            }
        }
        return;
    }

    int currentData = list.get(index);

    subset1.add(currentData);
    findMinDiff(list, index + 1, subset1, subset2, bestSubset1, bestSubset2, minDiff);
    subset1.head = subset1.head->next;

    subset2.add(currentData);
    findMinDiff(list, index + 1, subset1, subset2, bestSubset1, bestSubset2, minDiff);
    subset2.head = subset2.head->next;
}

void partitionSet(LinkedList& list) {
    LinkedList subset1, subset2, bestSubset1, bestSubset2;
    int minDiff = 123564698;

    findMinDiff(list, 0, subset1, subset2, bestSubset1, bestSubset2, minDiff);

    cout << "First subset: ";
    bestSubset1.print();
    cout << endl;
    cout << "Second subset: ";
    bestSubset2.print();
    cout << endl;
    cout << "Difference " << minDiff << "\n";
}

int main() {
    LinkedList list;
    list.add(5);
    list.add(8);
    list.add(1);
    list.add(14);
    list.add(7);

    partitionSet(list);

    return 0;
}
