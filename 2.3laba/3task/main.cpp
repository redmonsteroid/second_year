#include "mainHeader.hpp"
#include <iostream>
#include <string>

void testArraySerialization() {
    std::cout << "Testing Array Serialization" << std::endl;
    Array arr(5);
    arr.addToTheEnd("Hello");
    arr.addToTheEnd("World");
    arr.addToTheEnd("!");

    std::cout << "Array before saving to binary file: ";
    arr.print();

    arr.saveToBinaryFile("array_data.bin");

    Array arr2(5);
    arr2.loadFromBinaryFile("array_data.bin");

    std::cout << "Array after loading from binary file: ";
    arr2.print();
}

void testQueueSerialization() {
    std::cout << "Testing Queue Serialization" << std::endl;
    Queue queue;
    queue.push("Apple");
    queue.push("Banana");
    queue.push("Cherry");

    std::cout << "Queue before saving to binary file: ";
    queue.print();

    queue.saveToBinaryFile("queue_data.bin");

    Queue queue2;
    queue2.loadFromBinaryFile("queue_data.bin");

    std::cout << "Queue after loading from binary file: ";
    queue2.print();
}

void testCBTreeSerialization() {
    std::cout << "Testing CBTree Serialization" << std::endl;
    CBTree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);

    std::cout << "CBTree before saving to binary file:" << std::endl;
    tree.display();

    tree.saveToBinaryFile("tree_data.bin");

    CBTree tree2;
    tree2.loadFromBinaryFile("tree_data.bin");

    std::cout << "CBTree after loading from binary file:" << std::endl;
    tree2.display();
}

void testHashTableSerialization() {
    std::cout << "Testing HashTable Serialization" << std::endl;
    HashTable hashTable(5);
    hashTable.hset("key1", "value1");
    hashTable.hset("key2", "value2");
    hashTable.hset("key3", "value3");

    std::cout << "HashTable before saving to binary file:" << std::endl;
    hashTable.hprint();

    hashTable.saveToBinaryFile("hash_data.bin");

    HashTable hashTable2(5);
    hashTable2.loadFromBinaryFile("hash_data.bin");

    std::cout << "HashTable after loading from binary file:" << std::endl;
    hashTable2.hprint();
}

void testSinglyLinkedListSerialization() {
    std::cout << "Testing SinglyLinkedList Serialization" << std::endl;
    SinglyLinkedList sll;
    sll.addToHead("first");
    sll.addToTail("second");
    sll.addToTail("third");

    std::cout << "SinglyLinkedList before saving to binary file: ";
    sll.print();

    sll.saveToBinaryFile("sll_data.bin");

    SinglyLinkedList sll2;
    sll2.loadFromBinaryFile("sll_data.bin");

    std::cout << "SinglyLinkedList after loading from binary file: ";
    sll2.print();
}

void testDoublyLinkedListSerialization() {
    std::cout << "Testing DoublyLinkedList Serialization" << std::endl;
    DoublyLinkedList dll;
    dll.addToHead("first");
    dll.addToTail("second");
    dll.addToTail("third");

    std::cout << "DoublyLinkedList before saving to binary file: ";
    dll.print();

    dll.saveToBinaryFile("dll_data.bin");

    DoublyLinkedList dll2;
    dll2.loadFromBinaryFile("dll_data.bin");

    std::cout << "DoublyLinkedList after loading from binary file: ";
    dll2.print();
}

void testStackSerialization() {
    std::cout << "Testing Stack Serialization" << std::endl;
    Stack stack;
    stack.push("top");
    stack.push("middle");
    stack.push("bottom");

    std::cout << "Stack before saving to binary file: ";
    stack.print();

    stack.saveToBinaryFile("stack_data.bin");

    Stack stack2;
    stack2.loadFromBinaryFile("stack_data.bin");

    std::cout << "Stack after loading from binary file: ";
    stack2.print();
}

int main() {
    testArraySerialization();
    testQueueSerialization();
    testCBTreeSerialization();
    testHashTableSerialization();
    testSinglyLinkedListSerialization();
    testDoublyLinkedListSerialization();
    testStackSerialization();

    return 0;
}
