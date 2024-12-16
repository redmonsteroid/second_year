#include "tablejson.hpp"
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

void deleteFromTable(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName, conditionColumn, conditionValue;

    // Разбираем команду
    iss >> word;       // Пропускаем "DELETE"
    iss >> word;       // Пропускаем "FROM"
    iss >> tableName;  // Читаем имя таблицы

    if (!tableJS.isTableExist(tableName)) {
        std::cerr << "Table does not exist.\n";
        return;
    }

    iss >> word;  // Ожидаем "WHERE"
    if (word != "WHERE") {
        std::cerr << "Expected 'WHERE' clause.\n";
        return;
    }

    // Читаем условие до знака '='
    {
        std::string conditionStr;
        std::getline(iss, conditionStr);
        // " conditionColumn = 'conditionValue'"
        // Удаляем начальные пробелы
        conditionStr.erase(0, conditionStr.find_first_not_of(" \t"));
        // Теперь ищем '='
        size_t eqPos = conditionStr.find('=');
        if (eqPos == std::string::npos) {
            std::cerr << "No '=' in condition.\n";
            return;
        }

        conditionColumn = conditionStr.substr(0, eqPos);
        conditionColumn.erase(conditionColumn.find_last_not_of(" \t")+1);
        conditionColumn.erase(0, conditionColumn.find_first_not_of(" \t"));

        // Убираем префикс таблицы, если он есть
        size_t dotPos = conditionColumn.find('.');
        if (dotPos != std::string::npos) {
            conditionColumn = conditionColumn.substr(dotPos + 1);  
        }

        // Из оставшейся части после '=' извлекаем значение
        std::string valPart = conditionStr.substr(eqPos+1);
        valPart.erase(0, valPart.find_first_not_of(" \t"));
        // Ожидаем что значение в апострофах
        if (valPart.size() < 2 || valPart.front() != '\'' || valPart.back() != '\'') {
            std::cerr << "Condition value must be in quotes.\n";
            return;
        }
        // Извлекаем значение без кавычек
        conditionValue = valPart.substr(1, valPart.size()-2);
        conditionValue.erase(conditionValue.find_last_not_of(" \t;")+1);
        conditionValue.erase(0, conditionValue.find_first_not_of(" \t"));
    }

    std::cout << "Deleting from table: " << tableName << std::endl;
    std::cout << "Condition column: " << conditionColumn << ", condition value: '" << conditionValue << "'" << std::endl;

    // Проверяем блокировку таблицы
    if (is_locked(tableName, tableJS.schemeName)) {
        std::cerr << "Table is locked.\n";
        return;
    }

    toggle_lock(tableName, tableJS.schemeName);  // Блокируем таблицу
    try {
        // Путь к CSV-файлу
        std::string csvFilePath = fs::current_path().string() + "/" + tableJS.schemeName + "/" + tableName + "/1.csv";

        // Сначала читаем файл целиком
        std::ifstream inFile(csvFilePath);
        if (!inFile.is_open()) {
            std::cerr << "Cannot open file: " << csvFilePath << "\n";
            toggle_lock(tableName, tableJS.schemeName);
            return;
        }

        // Читаем заголовок
        std::string headerLine;
        if (!std::getline(inFile, headerLine)) {
            std::cerr << "Empty file or cannot read header.\n";
            toggle_lock(tableName, tableJS.schemeName);
            return;
        }

        std::istringstream headerStream(headerLine);
        std::string header;
        std::vector<std::string> headers;
        while (std::getline(headerStream, header, ',')) {
            headers.push_back(header);
        }

        // Ищем индекс нужного столбца
        int columnIndex = -1;
        for (size_t i = 0; i < headers.size(); ++i) {
            if (headers[i] == conditionColumn) {
                columnIndex = (int)i;
                break;
            }
        }

        if (columnIndex == -1) {
            std::cerr << "Column '" << conditionColumn << "' does not exist.\n";
            inFile.close();
            toggle_lock(tableName, tableJS.schemeName);
            return;
        }

        // Читаем все строки и выбираем те, которые не нужно удалять
        std::vector<std::string> linesToKeep;
        linesToKeep.push_back(headerLine); // оставляем заголовок
        std::string line;
        while (std::getline(inFile, line)) {
            std::istringstream lineStream(line);
            std::string cell;
            std::vector<std::string> rowValues;
            while (std::getline(lineStream, cell, ',')) {
                rowValues.push_back(cell);
            }

            if ((int)rowValues.size() <= columnIndex) {
                // Строка короче, чем ожидалось, странный CSV, оставляем на всякий случай
                linesToKeep.push_back(line);
                continue;
            }

            // Проверяем условие
            if (rowValues[columnIndex] == conditionValue) {
                // Эту строку пропускаем, не добавляем
            } else {
                // Строка не подходит под условие удаления, оставляем
                linesToKeep.push_back(line);
            }
        }

        inFile.close();

        // Теперь перезаписываем тот же файл без удалённых строк
        std::ofstream outFile(csvFilePath, std::ios::trunc);
        if (!outFile.is_open()) {
            std::cerr << "Cannot open file for rewriting: " << csvFilePath << "\n";
            toggle_lock(tableName, tableJS.schemeName);
            return;
        }

        for (size_t i = 0; i < linesToKeep.size(); ++i) {
            outFile << linesToKeep[i] << "\n";
        }

        outFile.close();
        std::cout << "Rows successfully deleted if matched condition.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error processing table: " << e.what() << std::endl;
    }

    toggle_lock(tableName, tableJS.schemeName);  // Разблокируем таблицу
}
