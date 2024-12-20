#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>



TEST(DoublyLinkedListTest, AddToHead) {
    DoublyLinkedList list;
    list.addToHead("first");
    list.addToHead("second");
    list.addToHead("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: third second first

    std::string output = buffer.str();
    EXPECT_NE(output.find("third"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("first"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода addToTail (добавление в конец)
TEST(DoublyLinkedListTest, AddToTail) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: first second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeFromHead (удаление с начала)
TEST(DoublyLinkedListTest, RemoveFromHead) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeFromHead();  // Удаляем "first"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeFromTail (удаление с конца)
TEST(DoublyLinkedListTest, RemoveFromTail) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeFromTail();  // Удаляем "third"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: first second

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода removeByValue (удаление по значению)
TEST(DoublyLinkedListTest, RemoveByValue) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    list.removeByValue("second");  // Удаляем "second"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: first third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода search (поиск по значению)
TEST(DoublyLinkedListTest, Search) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    DoublyNode* node = list.search("second");
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, "second");

    node = list.search("fourth");
    EXPECT_EQ(node, nullptr);  // Не найдено
}

// Тест для метода print (печать списка)
TEST(DoublyLinkedListTest, Print) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    list.print();  // Ожидаем: first second third

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

TEST(DoublyLinkedListTest, SaveToFile) {
    DoublyLinkedList list;
    list.addToTail("first");
    list.addToTail("second");
    list.addToTail("third");

    std::string filename = "test_output.txt";
    list.saveToFile(filename);  // Сохраняем список в файл

    // Проверяем, что файл существует и содержит ожидаемые данные
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open()) << "Не удалось открыть файл для чтения.";

    std::string content;
    std::getline(file, content);
    file.close();
    remove(filename.c_str());  // Удаляем файл после использования

    // Проверяем, что содержимое файла совпадает с ожидаемым результатом
    EXPECT_EQ(content, "first second third ");
}

// Тест для метода loadFromFile (загрузка из файла)
TEST(DoublyLinkedListTest, LoadFromFile) {
    DoublyLinkedList list;
    std::string filename = "test_tree.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "first\nsecond\nthird\n";  // Сохраняем данные для списка
    file.close();

    list.loadFromFile(filename);  // Загружаем данные в список

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout
    list.print();  // Проверяем, что данные из файла загружены

    std::string output = buffer.str();
    EXPECT_NE(output.find("first"), std::string::npos);
    EXPECT_NE(output.find("second"), std::string::npos);
    EXPECT_NE(output.find("third"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str()); // Удаляем файл после использования
}

