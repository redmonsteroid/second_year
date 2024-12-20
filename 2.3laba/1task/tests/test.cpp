#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>




// Тест для метода push (добавление в стек)
TEST(StackTest, Push) {
    Stack stack;
    stack.push("item1");
    stack.push("item2");
    stack.push("item3");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    stack.print();  // Ожидаем вывод: item3 item2 item1

    std::string output = buffer.str();
    EXPECT_NE(output.find("item3"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item1"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода pop (удаление из стека)
TEST(StackTest, Pop) {
    Stack stack;
    stack.push("item1");
    stack.push("item2");
    stack.push("item3");

    stack.pop();  // Удаляем "item3"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    stack.print();  // Ожидаем вывод: item2 item1

    std::string output = buffer.str();
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item1"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода pop (попытка удалить из пустого стека)
TEST(StackTest, PopEmptyStack) {
    Stack stack;

    // Перенаправляем вывод в строковый поток
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    stack.pop();  // Стек пуст, должно быть выведено сообщение "Stack is empty."

    std::string output = buffer.str();
    EXPECT_NE(output.find("Stack is empty."), std::string::npos);  // Проверяем сообщение

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода saveToFile (сохранение в файл)
TEST(StackTest, SaveToFile) {
    Stack stack;
    stack.push("item1");
    stack.push("item2");
    stack.push("item3");

    std::string filename = "stack_test.txt";
    stack.saveToFile(filename);  // Сохраняем стек в файл

    // Открываем файл для проверки
    std::ifstream file(filename);
    std::string line;
    EXPECT_TRUE(std::getline(file, line) && line == "item3");
    EXPECT_TRUE(std::getline(file, line) && line == "item2");
    EXPECT_TRUE(std::getline(file, line) && line == "item1");

    file.close();
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода loadFromFile (загрузка из файла)
TEST(StackTest, LoadFromFile) {
    Stack stack;
    std::string filename = "stack_load_test.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "item3\nitem2\nitem1\n";  // Сохраняем данные для стека
    file.close();

    stack.loadFromFile(filename);  // Загружаем данные в стек

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    stack.print();  // Ожидаем вывод: item3 item2 item1

    std::string output = buffer.str();
    EXPECT_NE(output.find("item3"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item1"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода print (печать стека)
TEST(StackTest, Print) {
    Stack stack;
    stack.push("item1");
    stack.push("item2");
    stack.push("item3");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    stack.print();  // Ожидаем вывод: item3 item2 item1

    std::string output = buffer.str();
    EXPECT_NE(output.find("item3"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item1"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
