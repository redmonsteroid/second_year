#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>


// Тест для метода hset (вставка в хеш-таблицу)
TEST(HashTableTest, HSet) {
    HashTable table(10);

    table.hset("key1", "value1");
    table.hset("key2", "value2");
    table.hset("key3", "value3");

    // Проверяем, что элементы добавлены
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hprint();  // Ожидаем вывод хеш-таблицы с ключами и значениями

    std::string output = buffer.str();
    EXPECT_NE(output.find("key1 => value1"), std::string::npos);
    EXPECT_NE(output.find("key2 => value2"), std::string::npos);
    EXPECT_NE(output.find("key3 => value3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}
TEST(HashTableTest, HSet_UpdateValue) {
    HashTable table(10);

    // Добавляем элемент с ключом "key1" и значением "value1"
    table.hset("key1", "value1");

    // Перенаправляем вывод в строковый поток
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Проверяем значение для "key1"
    table.hget("key1");  // Ожидаем: "Value for key [key1]: value1"
    
    // Обновляем значение для "key1"
    table.hset("key1", "new_value1");

    // Проверяем, что значение для "key1" обновилось
    table.hget("key1");  // Ожидаем: "Value for key [key1]: new_value1"

    std::string output = buffer.str();
    EXPECT_NE(output.find("Value for key [key1]: value1"), std::string::npos);  // Старое значение
    EXPECT_NE(output.find("Value for key [key1]: new_value1"), std::string::npos);  // Новое значение

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода hget (получение значения по ключу)
TEST(HashTableTest, HGet) {
    HashTable table(10);
    table.hset("key1", "value1");
    table.hset("key2", "value2");

    // Перенаправляем вывод в строковый поток
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hget("key1");  // Ожидаем: "Value for key [key1]: value1"
    table.hget("key3");  // Ожидаем: "Key [key3] not found."

    std::string output = buffer.str();
    EXPECT_NE(output.find("Value for key [key1]: value1"), std::string::npos);
    EXPECT_NE(output.find("Key [key3] not found."), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода hdel (удаление по ключу)
TEST(HashTableTest, HDel) {
    HashTable table(10);
    table.hset("key1", "value1");
    table.hset("key2", "value2");

    // Удаляем элемент
    table.hdel("key1");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hget("key1");  // Ожидаем: "Key [key1] not found."
    table.hget("key2");  // Ожидаем: "Value for key [key2]: value2"

    std::string output = buffer.str();
    EXPECT_NE(output.find("Key [key1] not found."), std::string::npos);
    EXPECT_NE(output.find("Value for key [key2]: value2"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода clear (очистка хеш-таблицы)
TEST(HashTableTest, Clear) {
    HashTable table(10);
    table.hset("key1", "value1");
    table.hset("key2", "value2");

    // Очистка таблицы
    table.clear();

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hget("key1");  // Ожидаем: "Key [key1] not found."
    table.hget("key2");  // Ожидаем: "Key [key2] not found."

    std::string output = buffer.str();
    EXPECT_NE(output.find("Key [key1] not found."), std::string::npos);
    EXPECT_NE(output.find("Key [key2] not found."), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода saveToFile (сохранение в файл)
TEST(HashTableTest, SaveToFile) {
    HashTable table(10);
    table.hset("key1", "value1");
    table.hset("key2", "value2");

    std::string filename = "hash_table_test.txt";
    table.saveToFile(filename);  // Сохраняем данные в файл

    // Открываем файл для проверки
    std::ifstream file(filename);
    std::string line;
    EXPECT_TRUE(std::getline(file, line) && line == "key1 value1");
    EXPECT_TRUE(std::getline(file, line) && line == "key2 value2");

    file.close();
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода loadFromFile (загрузка из файла)
TEST(HashTableTest, LoadFromFile) {
    HashTable table(10);
    std::string filename = "hash_table_load_test.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "key1 value1\n";
    file << "key2 value2\n";
    file.close();

    table.loadFromFile(filename);  // Загружаем данные в хеш-таблицу

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hget("key1");  // Ожидаем: "Value for key [key1]: value1"
    table.hget("key2");  // Ожидаем: "Value for key [key2]: value2"

    std::string output = buffer.str();
    EXPECT_NE(output.find("Value for key [key1]: value1"), std::string::npos);
    EXPECT_NE(output.find("Value for key [key2]: value2"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для хэш-функции (проверка коллизий)
TEST(HashTableTest, HashFunctionCollision) {
    HashTable table(10);

    // Пробуем вставить два ключа, которые, возможно, попадут в одну ячейку
    table.hset("key1", "value1");
    table.hset("key2", "value2");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    table.hprint();  // Проверяем, что оба элемента вставлены и хранятся корректно в таблице

    std::string output = buffer.str();
    EXPECT_NE(output.find("key1 => value1"), std::string::npos);
    EXPECT_NE(output.find("key2 => value2"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

