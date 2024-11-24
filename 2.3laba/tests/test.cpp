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
