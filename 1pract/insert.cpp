#include "insert.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;


void insert(const std::string& command, TableJson& tableJS) {
    std::string baseDir = fs::current_path().string() + "/" + tableJS.schemeName + "/";
    std::istringstream iss(command);
    std::string word, tableName;

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

    std::string values;
    std::getline(iss, values);
    size_t start = values.find('(');
    size_t end = values.rfind(')');
    if (start == std::string::npos || end == std::string::npos || start >= end) {
        std::cerr << "Incorrect command format.\n";
        return;
    }
    values = values.substr(start + 1, end - start - 1);

    // Разделяем значения на группы для множественной вставки
    std::string valueSet;
    std::istringstream valuesStream(values);
    size_t startValue = 0;
    bool insideQuotes = false;

    // Цикл обработки множественной вставки
    while (std::getline(valuesStream, valueSet, ')')) {
        size_t start = valueSet.find('(');
        if (start != std::string::npos) {
            valueSet = valueSet.substr(start + 1);
        }

        if (valueSet.empty()) continue;

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

        std::ofstream csvOut(csvFile, std::ios::app);
        if (!csvOut.is_open()) {
            std::cerr << "Failed to open CSV file: " << csvFile << std::endl;
            toggle_lock(tableName, tableJS.schemeName); // Разблокируем в случае ошибки
            return;
        }
        csvOut << currentPk << ",";

        // Обработка значений для вставки
        size_t endPos = 0;
        std::string token, currentValue;
        for (size_t i = 0; i < valueSet.length(); i++) {
            if (valueSet[i] == ',' && !insideQuotes) {
                currentValue = valueSet.substr(endPos, i - endPos);
                endPos = i + 1;
                currentValue.erase(std::remove(currentValue.begin(), currentValue.end(), '\''), currentValue.end());
                csvOut << currentValue << ",";
            } else if (valueSet[i] == '\'' && (i == 0 || valueSet[i - 1] != '\\')) {
                insideQuotes = !insideQuotes;
            }
        }
        // Окончание вставки значения
        currentValue = valueSet.substr(endPos);
        currentValue.erase(std::remove(currentValue.begin(), currentValue.end(), '\''), currentValue.end());
        csvOut << currentValue << std::endl;

        toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу
    }
}
