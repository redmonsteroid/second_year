#include "insert.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

bool is_locked(const std::string& tableName, const std::string& schemaName) {
    // Исправление: корректный путь к файлу блокировки
    std::string lockFile = fs::current_path().string() + "/" + schemaName + "/" + tableName + "/" + tableName + "_lock.txt";
    std::ifstream file(lockFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open lock file: " << lockFile << std::endl;
        return false;
    }
    std::string status;
    file >> status;
    return (status == "locked");
}

void toggle_lock(const std::string& tableName, const std::string& schemaName) {
    std::string lockFile = fs::current_path().string() + "/" + schemaName + "/" + tableName + "/" + tableName + "_lock.txt";
    std::ifstream fileIn(lockFile);
    if (!fileIn.is_open()) {
        std::cerr << "Failed to open lock file for reading: " << lockFile << std::endl;
        return;
    }
    std::string currentStatus;
    fileIn >> currentStatus;
    fileIn.close();

    std::ofstream fileOut(lockFile);
    if (!fileOut.is_open()) {
        std::cerr << "Failed to open lock file for writing: " << lockFile << std::endl;
        return;
    }
    fileOut << (currentStatus == "locked" ? "unlocked" : "locked");
}

void insert(const std::string& command, TableJson& tableJS) {
    std::string baseDir = fs::current_path().string() + "/" + tableJS.schemeName + "/";
    std::istringstream iss(command);
    std::string word, tableName;

    // Парсинг команды INSERT INTO <table_name> VALUES (...)
    iss >> word;       // Пропускаем "INSERT"
    iss >> word;       // Пропускаем "INTO"
    iss >> tableName;  // Читаем имя таблицы

    if (!tableJS.isTableExist(tableName)) {
        std::cerr << "Table does not exist.\n";
        return;
    }

    iss >> word;  // Ожидаем "VALUES"
    if (word != "VALUES") {
        std::cerr << "Expected 'VALUES' keyword.\n";
        return;
    }

    // Получаем значения внутри скобок
    std::string values;
    std::getline(iss, values);
    size_t start = values.find('(');
    size_t end = values.rfind(')');
    if (start == std::string::npos || end == std::string::npos || start >= end) {
        std::cerr << "Incorrect command format.\n";
        return;
    }
    values = values.substr(start + 1, end - start - 1); // Удаляем скобки вокруг значений

    if (is_locked(tableName, tableJS.schemeName)) {
        std::cerr << "Table is locked.\n";
        return;
    }
    toggle_lock(tableName, tableJS.schemeName);  // Блокируем таблицу

    int currentPk;
    std::string pkFile = baseDir + tableName + "/" + tableName + "_pk_sequence.txt";
    std::ifstream pkIn(pkFile);
    if (!pkIn.is_open()) {
        std::cerr << "Failed to open primary key file: " << pkFile << std::endl;
        toggle_lock(tableName, tableJS.schemeName); // Разблокируем в случае ошибки
        return;
    }
    pkIn >> currentPk;
    pkIn.close();

    std::ofstream pkOut(pkFile);
    if (!pkOut.is_open()) {
        std::cerr << "Failed to open primary key file for writing: " << pkFile << std::endl;
        toggle_lock(tableName, tableJS.schemeName); // Разблокируем в случае ошибки
        return;
    }
    pkOut << ++currentPk;

    int csvNum = 1;
    std::string csvFile;
    while (true) {
        csvFile = baseDir + tableName + "/" + std::to_string(csvNum) + ".csv";
        std::ifstream file(csvFile);
        if (file.peek() == std::ifstream::traits_type::eof() || std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n') < tableJS.tableSize) break;
        csvNum++;
    }

    // Запись данных в CSV-файл
    std::ofstream csvOut(csvFile, std::ios::app);
    if (!csvOut.is_open()) {
        std::cerr << "Failed to open CSV file: " << csvFile << std::endl;
        toggle_lock(tableName, tableJS.schemeName); // Разблокируем в случае ошибки
        return;
    }
    csvOut << currentPk << ",";

    // Разделяем значения, обрабатывая каждый блок, заключенный в одинарные кавычки
    std::istringstream valueStream(values);
    std::string value;
    bool first = true;
    while (std::getline(valueStream, value, '\'')) {
        if (value.empty() || value == ",") continue;
        if (!first) csvOut << ","; // Добавляем запятую перед каждым значением, кроме первого
        csvOut << value;
        std::getline(valueStream, value, ','); // Пропускаем запятую
        first = false;
    }
    csvOut << std::endl;

    toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу
}
