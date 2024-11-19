#include "tablejson.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void deleteFromTable(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName, conditionColumn, conditionValue;
    iss >> word;       // Пропускаем "DELETE"
    iss >> word;       // Пропускаем "FROM"
    iss >> tableName;  // Читаем имя таблицы

    if (!tableJS.isTableExist(tableName)) {
        std::cerr << "Table does not exist.\n";
        return;
    }

    iss >> word;  // Читаем "WHERE"
    if (word != "WHERE") {
        std::cerr << "Expected 'WHERE' clause.\n";
        return;
    }

    // Читаем условие WHERE
    std::getline(iss, conditionColumn, '=');
    conditionColumn.erase(conditionColumn.find_last_not_of(" \t") + 1);  // Удаляем лишние пробелы справа

    // Проверяем, содержит ли название таблицы
    size_t dotPos = conditionColumn.find('.');
    if (dotPos != std::string::npos) {
        conditionColumn = conditionColumn.substr(dotPos + 1);  // Удаляем префикс tableName.
    }

    // Извлекаем значение условия WHERE
    std::getline(iss, conditionValue, '\'');
    std::getline(iss, conditionValue, '\'');  // Извлекаем значение между апострофами

    if (is_locked(tableName, tableJS.schemeName)) {
        std::cerr << "Table is locked.\n";
        return;
    }

    toggle_lock(tableName, tableJS.schemeName);  // Блокируем таблицу

    std::string baseDir = fs::current_path().string() + "/" + tableJS.schemeName + "/" + tableName + "/";
    int csvCount = amountOfCSV(tableJS, tableName);
    bool rowDeleted = false;

    for (int csvNum = 1; csvNum <= csvCount; ++csvNum) {
        std::string csvFilePath = baseDir + std::to_string(csvNum) + ".csv";
        std::string tempFilePath = baseDir + "temp_" + std::to_string(csvNum) + ".csv";

        std::ifstream csvFile(csvFilePath);
        std::ofstream tempFile(tempFilePath);

        if (!csvFile.is_open() || !tempFile.is_open()) {
            std::cerr << "Failed to open CSV file: " << csvFilePath << std::endl;
            toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу при ошибке
            return;
        }

        std::string header;
        std::getline(csvFile, header);
        tempFile << header << "\n";  // Копируем заголовок

        int columnIndex = -1;
        std::istringstream headerStream(header);
        std::string column;
        int index = 0;
        while (std::getline(headerStream, column, ',')) {
            column.erase(0, column.find_first_not_of(" \t"));  // Убираем пробелы в начале и конце
            column.erase(column.find_last_not_of(" \t") + 1);
            if (column == conditionColumn) {
                columnIndex = index;
                break;
            }
            ++index;
        }
        if (columnIndex == -1) {
            std::cerr << "Column does not exist.\n";
            toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу при ошибке
            return;
        }

        std::string line;
        while (std::getline(csvFile, line)) {
            std::istringstream lineStream(line);
            std::string value;
            int colIdx = 0;
            bool shouldDelete = false;

            while (std::getline(lineStream, value, ',')) {
                value.erase(value.find_last_not_of(" \t") + 1);  // Убираем лишние пробелы
                if (colIdx == columnIndex && value == conditionValue) {
                    shouldDelete = true;
                    rowDeleted = true;
                    break;
                }
                ++colIdx;
            }
            if (!shouldDelete) {
                tempFile << line << "\n";  // Копируем строку, если она не должна быть удалена
            }
        }

        csvFile.close();
        tempFile.close();

        fs::remove(csvFilePath);
        fs::rename(tempFilePath, csvFilePath);
    }

    toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу

    if (!rowDeleted) {
        std::cout << "No matching rows found.\n";
    } else {
        std::cout << "Rows successfully deleted.\n";
    }
}

