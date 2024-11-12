#include <iostream> //6

using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;

    Node(int val) : value(val), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    Node* root;

    
    Node* addNode(Node* node, int value) { // добавление по правилам bst
        if (node == nullptr) {
            return new Node(value);
        }
        if (value < node->value) { // меньше налево 
            node->left = addNode(node->left, value);
        } else if (value > node->value) {
            node->right = addNode(node->right, value);
        }
        return node;
    }

    
    void collectLeaves(Node* node) const {
        if (node == nullptr) return;

        
        collectLeaves(node->left); // рекурсивно доходим до конца каждой половины 

        
        if (node->left == nullptr && node->right == nullptr) {
            cout << node->value << " ";
        }

        collectLeaves(node->right);
    }

public:
    BinarySearchTree() : root(nullptr) {}

    
    void add(int value) {
        root = addNode(root, value);
    }

    
    void printLeavesInOrder() {
        cout << "Leaves: ";
        collectLeaves(root);
        cout << endl;
    }
};

int main() {
    BinarySearchTree bst;
    int value;

    cout << "Enter numbers to add to the tree (end with letter) ";
    while (cin >> value) {
        bst.add(value);
    }

    bst.printLeavesInOrder();

    return 0;
}
