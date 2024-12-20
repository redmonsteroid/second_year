#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>

// Тест для метода push (добавление в очередь)
TEST(QueueTest, Push) {
    Queue queue;
    queue.push("item1");
    queue.push("item2");
    queue.push("item3");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    queue.print();  // Ожидаем вывод: item1 item2 item3

    std::string output = buffer.str();
    EXPECT_NE(output.find("item1"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода pop (удаление из очереди)
TEST(QueueTest, Pop) {
    Queue queue;
    queue.push("item1");
    queue.push("item2");
    queue.push("item3");

    queue.pop();  // Удаляем "item1"

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    queue.print();  // Ожидаем вывод: item2 item3

    std::string output = buffer.str();
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода pop (попытка удалить из пустой очереди)
TEST(QueueTest, PopEmptyQueue) {
    Queue queue;

    // Перенаправляем вывод в строковый поток
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    queue.pop();  // Очередь пуста, должно быть выведено сообщение "Queue is empty."

    std::string output = buffer.str();
    EXPECT_NE(output.find("Queue is empty."), std::string::npos);  // Проверяем сообщение

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для метода saveToFile (сохранение в файл)
TEST(QueueTest, SaveToFile) {
    Queue queue;
    queue.push("item1");
    queue.push("item2");
    queue.push("item3");

    std::string filename = "queue_test.txt";
    queue.saveToFile(filename);  // Сохраняем очередь в файл

    // Открываем файл для проверки
    std::ifstream file(filename);
    std::string line;
    EXPECT_TRUE(std::getline(file, line) && line == "item1");
    EXPECT_TRUE(std::getline(file, line) && line == "item2");
    EXPECT_TRUE(std::getline(file, line) && line == "item3");

    file.close();
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода loadFromFile (загрузка из файла)
TEST(QueueTest, LoadFromFile) {
    Queue queue;
    std::string filename = "queue_load_test.txt";

    // Создаем тестовый файл с данными
    std::ofstream file(filename);
    file << "item1\nitem2\nitem3\n";  // Сохраняем данные для очереди
    file.close();

    queue.loadFromFile(filename);  // Загружаем данные в очередь

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    queue.print();  // Ожидаем вывод: item1 item2 item3

    std::string output = buffer.str();
    EXPECT_NE(output.find("item1"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
    remove(filename.c_str());  // Удаляем файл после использования
}

// Тест для метода print (печать очереди)
TEST(QueueTest, Print) {
    Queue queue;
    queue.push("item1");
    queue.push("item2");
    queue.push("item3");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    queue.print();  // Ожидаем вывод: item1 item2 item3

    std::string output = buffer.str();
    EXPECT_NE(output.find("item1"), std::string::npos);
    EXPECT_NE(output.find("item2"), std::string::npos);
    EXPECT_NE(output.find("item3"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}
