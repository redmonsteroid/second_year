#include <gtest/gtest.h>
#include "../src/mainHeader.hpp"
#include <sstream>
#include <fstream>

// Простой тест для проверки добавления в конец
TEST(ArrayTest, AddToEnd_SingleElement) {
    Array arr(10);
    arr.addToTheEnd("testValue");
    EXPECT_EQ(arr.get(0), "testValue");
}

// Тест для конструктора и деструктора
TEST(ArrayTest, ConstructorAndDestructor) {
    Array arr(10);
    EXPECT_EQ(arr.size, 0);
    EXPECT_EQ(arr.maxCapacity, 10);
}

// Тест для добавления элемента по индексу
TEST(ArrayTest, AddByIndex) {
    Array arr(10);
    arr.add(0, "test1");
    EXPECT_EQ(arr.get(0), "test1");
}

// Тест для добавления элемента в конец (проверка увеличения размера)
TEST(ArrayTest, AddToEnd_MultipleElements) {
    Array arr(10);
    arr.addToTheEnd("test2");
    EXPECT_EQ(arr.get(0), "test2");
    EXPECT_EQ(arr.size, 1);
}

// Тест для получения элемента по индексу
TEST(ArrayTest, GetElement) {
    Array arr(10);
    arr.add(0, "test3");
    EXPECT_EQ(arr.get(0), "test3");
}

// Тест для удаления элемента
TEST(ArrayTest, RemoveElement) {
    Array arr(10);
    arr.addToTheEnd("test4");
    arr.remove(0);
    EXPECT_EQ(arr.size, 0);
}

// Тест для замены элемента
TEST(ArrayTest, ReplaceElement) {
    Array arr(10);
    arr.addToTheEnd("test5");
    arr.replace(0, "test6");
    EXPECT_EQ(arr.get(0), "test6");
}

// Тест для функции length
TEST(ArrayTest, LengthCheck) {
    Array arr(10);
    arr.addToTheEnd("test7");
    arr.addToTheEnd("test8");
    EXPECT_EQ(arr.length(), 2);
}

// Тест для записи и загрузки данных в/из файла
TEST(ArrayTest, SaveAndLoadFromFile) {
    Array arr(10);
    arr.addToTheEnd("test1");
    arr.addToTheEnd("test2");

    std::string filename = "testfile.txt";
    arr.saveToFile(filename);

    // Загружаем данные в новый массив и проверяем их
    Array loadedArr(10);
    loadedArr.loadFromFile(filename);
    EXPECT_EQ(loadedArr.get(0), "test1");
    EXPECT_EQ(loadedArr.get(1), "test2");

    // Удаляем временный файл
    remove(filename.c_str());
}

// Тест для проверки ошибок при доступе по неверному индексу
TEST(ArrayTest, InvalidIndex) {
    Array arr(5);
    arr.addToTheEnd("test");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Пытаемся получить элемент по неверному индексу
    arr.get(10);

    // Проверяем, что в выводе содержится сообщение "Index invalid"
    std::string output = buffer.str();
    EXPECT_NE(output.find("Index invalid"), std::string::npos)
        << "Expected message 'Index invalid' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при попытке добавить элемент в полный массив
TEST(ArrayTest, AddToFullArray) {
    Array arr(2);  // Массив с максимальной вместимостью 2
    arr.addToTheEnd("item1");
    arr.addToTheEnd("item2");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Попытка добавить третий элемент в уже полный массив
    arr.addToTheEnd("item3");

    // Проверяем, что в выводе содержится сообщение "Array is full"
    std::string output = buffer.str();
    EXPECT_NE(output.find("Array is full"), std::string::npos)
        << "Expected message 'Array is full' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при попытке добавить элемент с неверным индексом или переполнении массива
TEST(ArrayTest, AddInvalidIndexOrFullArray) {
    Array arr(2); // Массив с максимальной вместимостью 2
    arr.add(0, "item1");
    arr.add(1, "item2");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Попытка добавить третий элемент (переполнение массива)
    arr.add(2, "item3");
    std::string output = buffer.str();
    EXPECT_NE(output.find("Index invalid or array is full"), std::string::npos)
        << "Expected message 'Index invalid or array is full' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при попытке удаления элемента по неверному индексу
TEST(ArrayTest, RemoveInvalidIndex) {
    Array arr(5);
    arr.addToTheEnd("item1");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Пытаемся удалить элемент по неверному индексу
    arr.remove(10);

    // Проверяем, что в выводе содержится сообщение "Index invalid"
    std::string output = buffer.str();
    EXPECT_NE(output.find("Index invalid"), std::string::npos)
        << "Expected message 'Index invalid' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для удаления элемента сдвигом элементов
TEST(ArrayTest, RemoveAndShiftElements) {
    Array arr(5);
    arr.addToTheEnd("item1");
    arr.addToTheEnd("item2");
    arr.addToTheEnd("item3");

    // Удаляем элемент по индексу 1
    arr.remove(1);

    EXPECT_EQ(arr.get(1), "item3");  // Проверяем, что элемент сдвинулся
}

// Тест для метода print
TEST(ArrayTest, Print) {
    Array arr(10);
    arr.addToTheEnd("test1");
    arr.addToTheEnd("test2");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    arr.print();
    std::string output = buffer.str();
    EXPECT_NE(output.find("test1"), std::string::npos);
    EXPECT_NE(output.find("test2"), std::string::npos);

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при сохранении в файл
TEST(ArrayTest, SaveToFileError) {
    Array arr(2);
    arr.addToTheEnd("item1");

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Пытаемся сохранить в файл с ошибкой
    arr.saveToFile("/invalid/path/test.txt");

    std::string output = buffer.str();
    EXPECT_NE(output.find("Cannot open file for writing"), std::string::npos)
        << "Expected message 'Cannot open file for writing' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}

// Тест для ошибки при загрузке из файла
TEST(ArrayTest, LoadFromFileError) {
    Array arr(2);

    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем std::cout

    // Пытаемся загрузить данные из файла с ошибкой
    arr.loadFromFile("/invalid/path/test.txt");

    std::string output = buffer.str();
    EXPECT_NE(output.find("Cannot open file for reading"), std::string::npos)
        << "Expected message 'Cannot open file for reading' not found in output.";

    std::cout.rdbuf(old); // Восстанавливаем std::cout
}
