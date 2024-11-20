#include "tablejson.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "select.hpp"

DynamicArray readCSVHeader(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::string headerLine;
    if (!std::getline(file, headerLine)) {
        throw std::runtime_error("File is empty or header is missing: " + filePath);
    }

    DynamicArray headers;
    std::istringstream headerStream(headerLine);
    std::string column;

    while (std::getline(headerStream, column, ',')) {
        headers.add(column);
    }

    return headers;
}

DynamicArray readCSVData(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::string line;
    std::getline(file, line);  // Пропускаем заголовок

    DynamicArray rows;
    while (std::getline(file, line)) {
        rows.add(line);
    }

    return rows;
}

void crossJoin(DynamicArray& table1Data, DynamicArray& table2Data, DynamicArray& table1Header, DynamicArray& table2Header) {
    std::cout << "[DEBUG] Table 1 size: " << table1Data.getSize() << "\n";  // Отладка
    std::cout << "[DEBUG] Table 2 size: " << table2Data.getSize() << "\n";  // Отладка

    if (table1Data.getSize() == 0 || table2Data.getSize() == 0) {
        std::cerr << "[DEBUG] One or both tables are empty. Cross join aborted.\n";
        return;
    }

    std::ofstream outFile("cross_join_result.csv");
    outFile << table1Header.get(0) << "," << table2Header.get(0) << "\n";
    std::cout << "[DEBUG] Headers: " << table1Header.get(0) << "," << table2Header.get(0) << "\n";  // Отладка

    for (int i = 0; i < table1Data.getSize(); ++i) {
        for (int j = 0; j < table2Data.getSize(); ++j) {
            outFile << table1Data.get(i) << "," << table2Data.get(j) << "\n";
            std::cout << "[DEBUG] Cross Join Row: " << table1Data.get(i) << "," << table2Data.get(j) << "\n";  // Отладка
        }
    }

    outFile.close();
    std::cout << "[DEBUG] Cross join result has been written to cross_join_result.csv\n";
}


void selectFromTables(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName;
    DynamicArray selectedColumns;  // Массив для хранения имен колонок
    DynamicArray tableNames;       // Массив для хранения имен таблиц

    // Парсинг команды: игнорируем "SELECT"
    iss >> word;

    // Чтение имен колонок до "FROM"
    while (iss >> word && word != "FROM") {
        if (word.back() == ',') word.pop_back();  // Удаляем запятую
        selectedColumns.add(word);
        std::cout << "[DEBUG] Selected column: " << word << "\n"; // Отладочный вывод
    }

    // Чтение имен таблиц после "FROM"
    while (iss >> tableName) {
        if (tableName.back() == ',') tableName.pop_back();  // Удаляем запятую
        tableNames.add(tableName);
        std::cout << "[DEBUG] Table name: " << tableName << "\n"; // Отладочный вывод
    }

    // Проверка существования таблиц
    for (int i = 0; i < tableNames.getSize(); ++i) {
        if (!tableJS.isTableExist(tableNames.get(i))) {
            std::cerr << "[DEBUG] Table does not exist: " << tableNames.get(i) << "\n";
            return;
        }
    }

    // Считываем данные таблиц
    DynamicArray tableHeaders[2];
    DynamicArray tableData[2];

    try {
        for (int i = 0; i < tableNames.getSize(); ++i) {
            std::string csvFilePath = tableJS.schemeName + "/" + tableNames.get(i) + "/1.csv";
            std::cout << "[DEBUG] Reading table: " << tableNames.get(i) << " from " << csvFilePath << "\n"; // Отладка

            tableHeaders[i] = readCSVHeader(csvFilePath);
            tableData[i] = readCSVData(csvFilePath);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading tables: " << e.what() << "\n";
        return;
    }

    // Выполняем Cross Join
    crossJoin(tableData[0], tableData[1], tableHeaders[0], tableHeaders[1]);
}
