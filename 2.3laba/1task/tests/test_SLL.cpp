#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>



// Тест для метода addToHead (добавление в начало списка)
TEST(SinglyLinkedListTest, AddToHead) {
    SinglyLinkedList list;
    list.addToHead("first");
    list.addToHead("second");
    list.addToHead("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: third second first

    std::string output = buffer.str();
    EXPECT_NE(output.find("third"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("first"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода addToTail (добавление в конец списка)
TEST(SinglyLinkedListTest, AddToTail) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: first second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeHead (удаление с начала списка)
TEST(SinglyLinkedListTest, RemoveHead) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeHead();  // Удаляем "first"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeTail (удаление с конца списка)
TEST(SinglyLinkedListTest, RemoveTail) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeTail();  // Удаляем "third"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: first second

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeByValue (удаление по значению)
TEST(SinglyLinkedListTest, RemoveByValue) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeByValue("second");  // Удаляем "second"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: first third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода search (поиск по значению)
TEST(SinglyLinkedListTest, Search) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    Node* node = list.search("second");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, "second");

    node = list.search("fourth");
    EXPECT_EQ(node, nullptr);  // Не найдено
}

// Тест для метода print (печать списка)
TEST(SinglyLinkedListTest, Print) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: first second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода saveToFile (сохранение в файл)
TEST(SinglyLinkedListTest, SaveToFile) {
    SinglyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::string filename = "list_test.txt";
    list.saveToFile(filename);  // Сохраняем список в файл

    // Открываем файл для проверки
    std::ifstream file(filename);
    std::string line;
    EXPECT_TRUE(std::getline(file, line) && line == "first");
    EXPECT_TRUE(std::getline(file, line) && line == "second");
    EXPECT_TRUE(std::getline(file, line) && line == "third");

    file.close();
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода loadFromFile (загрузка из файла)
TEST(SinglyLinkedListTest, LoadFromFile) {
    SinglyLinkedList list;
    std::string filename = "list_load_test.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "first\nsecond\nthird\n";  // Сохраняем данные для списка
    file.close();

    list.loadFromFile(filename);  // Загружаем данные в список

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем вывод: first second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str());  // Удаляем файл после использования
}
