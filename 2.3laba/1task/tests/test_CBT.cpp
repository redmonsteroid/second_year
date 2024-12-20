#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>


TEST(CBTreeTest, IsCBT) {
    CBTree tree;
    
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Пустое дерево
    EXPECT_FALSE(tree.is_CBT());  // Должно вывести "Tree is empty."

    // Создаем сбалансированное дерево
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    tree.insert(5);

    // Теперь дерево полно
    EXPECT_TRUE(tree.is_CBT());

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода get_value (поиск по значению)
TEST(CBTreeTest, GetValue) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Проверяем, что дерево находит значение
    EXPECT_TRUE(tree.get_value(2)); // Должен найти
    EXPECT_FALSE(tree.get_value(4)); // Не существует, должно вывести "Tree is empty."

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}


// Тест для метода find_index (поиск по индексу)
TEST(CBTreeTest, FindIndex) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Проверяем, что значение найдено по индексу
    tree.find_index(1);  // Должен вернуть значение 2 (индекс 1)
    tree.find_index(10); // Индекс 10 не существует, должно вывести "Invalid index."

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода display (вывод дерева)
TEST(CBTreeTest, Display) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    tree.display();  // Проверяем, что дерево выводится в консоль

    std::string output = buffer.str();
    EXPECT_NE(output.find("1"), std::string::npos);  // Проверяем, что в выводе есть значения дерева
    EXPECT_NE(output.find("2"), std::string::npos);
    EXPECT_NE(output.find("3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}
// Тест для метода load_from_file (загрузка дерева из файла)
TEST(CBTreeTest, LoadFromFile) {
    CBTree tree;
    std::string filename = "test_tree.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "1\n2\n3\n4\n5\n";  // Сохраняем данные для дерева
    file.close();

    tree.load_from_file(filename);  // Загружаем данные в дерево

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout
    tree.display();  // Проверяем, что данные из файла загружены

    std::string output = buffer.str();
    EXPECT_NE(output.find("1"), std::string::npos);
    EXPECT_NE(output.find("2"), std::string::npos);
    EXPECT_NE(output.find("3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str()); // Удаляем файл после использования
}

// Тест для метода save_to_file (сохранение дерева в файл)
TEST(CBTreeTest, SaveToFile) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    std::string filename = "test_output.txt";

    tree.save_to_file(filename);  // Сохраняем дерево в файл

    // Открываем файл для проверки
    std::ifstream file(filename);
    std::string line;
    EXPECT_TRUE(std::getline(file, line) && line == "1");
    EXPECT_TRUE(std::getline(file, line) && line == "2");
    EXPECT_TRUE(std::getline(file, line) && line == "3");

    file.close();
    remove(filename.c_str());  // Удаляем файл после использования
}
TEST(CBTreeTest, LoadFromFileError) {
    CBTree tree;
    std::string filename = "non_existent_file.txt";

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    tree.load_from_file(filename);  // Попытка загрузки из несуществующего файла
    std::string output = buffer.str();
    EXPECT_NE(output.find("Failed to open file."), std::string::npos);  // Проверка на ошибку

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при попытке сохранить дерево в файл
TEST(CBTreeTest, SaveToFileError) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);

    std::string filename = "/invalid_path/test_output.txt";

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    tree.save_to_file(filename);  // Попытка сохранить в файл с ошибкой

    std::string output = buffer.str();
    EXPECT_NE(output.find("Failed to open file."), std::string::npos);  // Проверка на ошибку

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}


// Тест для метода clear (очистка дерева)
TEST(CBTreeTest, ClearTree) {
    CBTree tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    
    // Проверяем, что дерево не пусто перед очисткой
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout
    tree.display();
    std::string output = buffer.str();
    EXPECT_NE(output.find("1"), std::string::npos);  // Проверяем, что дерево не пусто
    EXPECT_NE(output.find("2"), std::string::npos);
    EXPECT_NE(output.find("3"), std::string::npos);
    EXPECT_NE(output.find("4"), std::string::npos);

    // Очищаем дерево
    tree.clear(tree.root);  // Явный вызов clear()

    // Проверяем, что дерево теперь пусто
    buffer.str("");  // Очищаем содержимое буфера
    tree.display();  // Дерево должно быть пустым
    output = buffer.str();
    EXPECT_NE(output.find("Tree is empty."), std::string::npos);  // Проверка, что дерево пусто

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}
