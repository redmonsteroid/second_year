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
        if (line.empty()) {
            std::cerr << "[WARNING] Skipping empty line in file: " << filePath << "\n";
            continue;
        }

        rows.add(line);
        std::cout << "[DEBUG] Added row: " << line << " to data\n";
    }

    if (rows.getSize() == 0) {
        std::cerr << "[WARNING] No data found in file: " << filePath << "\n";
    }

    return rows;
}



void crossJoin(DynamicArray& table1Data, DynamicArray& table2Data, DynamicArray& table1Header, DynamicArray& table2Header, const DynamicArray& selectedColumns) {
    try {
        std::ofstream outFile("cross_join_result.csv");
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open output file: cross_join_result.csv");
        }

        // Определяем индексы выбранных колонок
        int table1ColIndex = -1;
        int table2ColIndex = -1;

        for (int i = 0; i < selectedColumns.getSize(); ++i) {
            std::string col = selectedColumns.get(i);
            size_t dotPos = col.find('.');
            if (dotPos != std::string::npos) {
                std::string tableName = col.substr(0, dotPos);
                std::string columnName = col.substr(dotPos + 1);

                if (tableName == "таблица1") {
                    for (int j = 0; j < table1Header.getSize(); ++j) {
                        if (table1Header.get(j) == columnName) {
                            table1ColIndex = j;
                            break;
                        }
                    }
                } else if (tableName == "таблица2") {
                    for (int j = 0; j < table2Header.getSize(); ++j) {
                        if (table2Header.get(j) == columnName) {
                            table2ColIndex = j;
                            break;
                        }
                    }
                }
            }
        }

        if (table1ColIndex == -1 || table2ColIndex == -1) {
            throw std::runtime_error("Selected columns not found in tables.");
        }

        // Записываем заголовок
        outFile << selectedColumns.get(0) << "," << selectedColumns.get(1) << "\n";

        // Cross Join и вывод
        for (int i = 0; i < table1Data.getSize(); ++i) {
            std::string row1 = table1Data.get(i);
            std::istringstream rowStream1(row1);
            std::string cell1;
            for (int col = 0; col <= table1ColIndex; ++col) {
                std::getline(rowStream1, cell1, ',');
            }

            for (int j = 0; j < table2Data.getSize(); ++j) {
                std::string row2 = table2Data.get(j);
                std::istringstream rowStream2(row2);
                std::string cell2;
                for (int col = 0; col <= table2ColIndex; ++col) {
                    std::getline(rowStream2, cell2, ',');
                }

                // Записываем в файл и консоль
                outFile << cell1 << "," << cell2 << "\n";
                std::cout << "[DEBUG] Cross Join Row: " << cell1 << "," << cell2 << "\n";
            }
        }

        outFile.close();
        std::cout << "[DEBUG] Cross join result has been written to cross_join_result.csv\n";
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Cross join failed: " << e.what() << "\n";
    }
}





void selectFromTables(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName;
    DynamicArray selectedColumns;  // Список колонок
    DynamicArray tableNames;       // Список таблиц

    // Парсинг команды: игнорируем "SELECT"
    iss >> word;

    // Чтение колонок до "FROM"
    while (iss >> word && word != "FROM") {
        if (word.back() == ',') word.pop_back();
        selectedColumns.add(word);
        std::cout << "[DEBUG] Selected column: " << word << "\n";
    }

    // Чтение таблиц после "FROM"
    while (iss >> tableName) {
        if (tableName.back() == ',') tableName.pop_back();
        tableNames.add(tableName);
        std::cout << "[DEBUG] Table name: " << tableName << "\n";
    }

    // Проверка существования таблиц
    for (int i = 0; i < tableNames.getSize(); ++i) {
        if (!tableJS.isTableExist(tableNames.get(i))) {
            std::cerr << "Table " << tableNames.get(i) << " does not exist.\n";
            return;
        }
    }

    // Считываем данные таблиц
    DynamicArray tableHeaders[2];
    DynamicArray tableData[2];

    try {
        for (int i = 0; i < tableNames.getSize(); ++i) {
            std::string csvFilePath = tableJS.schemeName + "/" + tableNames.get(i) + "/1.csv";
            std::cout << "[DEBUG] Reading table: " << tableNames.get(i) << " from " << csvFilePath << "\n";

            tableHeaders[i] = readCSVHeader(csvFilePath);  // Читаем заголовки
            tableData[i] = readCSVData(csvFilePath);      // Читаем данные
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading tables: " << e.what() << "\n";
        return;
    }

    // Выполняем Cross Join
    crossJoin(tableData[0], tableData[1], tableHeaders[0], tableHeaders[1], selectedColumns);

}
