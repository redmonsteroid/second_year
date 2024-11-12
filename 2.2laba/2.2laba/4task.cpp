#include <iostream> //2 
#include <sstream>

using namespace std;

struct Node {
    int value;

    Node(int val = 0) : value(val) {}
};

class Array {
private:
    Node* data;     
    int capacity;   
    int size;      

    void resize() {
        int newCapacity = capacity * 2;
        Node* newData = new Node[newCapacity];
        
       
        for (int i = 0; i < size; ++i) {
            newData[i] = data[i];
        }

      
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    
    Array(int initialCapacity = 10) : capacity(initialCapacity), size(0) {
        data = new Node[capacity];
    }

    
    ~Array() {
        delete[] data;
    }

    
    void add(int value) {
        if (size >= capacity) {
            resize();
        }
        data[size++] = Node(value);
    }

   
    int get(int index) const {
        if (index < 0 || index >= size) {
            cerr << "Index out of bounds" << endl;
            return -1;
        }
        return data[index].value;
    }

   
    int getSize() const {
        return size;
    }

 
    void findSubarraysWithSum(int targetSum) const {
        for (int start = 0; start < size; ++start) {
            int currentSum = 0;
            for (int end = start; end < size; ++end) {
                currentSum += get(end);
                if (currentSum == targetSum) {
                    printSubarray(start, end);
                }
            }
        }
    }

    
    void printSubarray(int start, int end) const {
        cout << "[";
        for (int i = start; i <= end; ++i) {
            cout << get(i);
            if (i < end) cout << ", ";
        }
        cout << "]" << endl;
    }
};

int main() {
    Array Array;
    int targetSum;

    cout << "Enter elements separated by spaces: ";
    string input;
    getline(cin, input);
    istringstream iss(input);
    int value;
    while (iss >> value) {
        Array.add(value);
    }

  
    cout << "Enter the target sum: ";
    cin >> targetSum;

    cout << "Subarrays with sum " << targetSum << ":" << endl;
    Array.findSubarraysWithSum(targetSum);

    return 0;
}
