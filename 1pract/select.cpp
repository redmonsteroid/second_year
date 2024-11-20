#include "tablejson.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "rapidcsv.h"

void selectFromTables(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName;
    std::string selectedColumns[10]; // Массив для хранения имен колонок
    std::string tableNames[5];       // Массив для хранения имен таблиц
    int columnCount = 0;
    int tableCount = 0;

    // Парсинг команды: игнорируем "SELECT"
    iss >> word;

    // Чтение имен колонок до "FROM"
    while (iss >> word && word != "FROM") {
        if (word.back() == ',') word.pop_back();  // Удаляем запятую
        selectedColumns[columnCount++] = word;
    }

    // Чтение имен таблиц после "FROM"
    while (iss >> tableName) {
        if (tableName.back() == ',') tableName.pop_back();  // Удаляем запятую
        tableNames[tableCount++] = tableName;
    }

    // Проверка существования таблиц
    for (int i = 0; i < tableCount; ++i) {
        if (!tableJS.isTableExist(tableNames[i])) {
            std::cerr << "Table " << tableNames[i] << " does not exist.\n";
            return;
        }
    }

    // Выводим заголовок результата
    for (int i = 0; i < columnCount; ++i) {
        std::cout << selectedColumns[i];
        if (i < columnCount - 1) std::cout << ", ";
    }
    std::cout << "\n";

    // Чтение данных из таблиц
    for (int t = 0; t < tableCount; ++t) {
        std::string baseDir = tableJS.schemeName + "/" + tableNames[t] + "/";
        int csvCount = amountOfCSV(tableJS, tableNames[t]);

        for (int csvNum = 1; csvNum <= csvCount; ++csvNum) {
            std::string csvFilePath = baseDir + std::to_string(csvNum) + ".csv";
            std::ifstream csvFile(csvFilePath);

            if (!csvFile.is_open()) {
                std::cerr << "Failed to open CSV file: " << csvFilePath << std::endl;
                return;
            }

            // Чтение заголовка и определение индексов колонок для выборки
            std::string header;
            std::getline(csvFile, header);
            int columnIndices[10];
            int foundColumns = 0;
            std::istringstream headerStream(header);
            std::string columnName;
            int colIdx = 0;

            while (std::getline(headerStream, columnName, ',')) {
                columnName.erase(0, columnName.find_first_not_of(" \t"));  // Убираем пробелы
                columnName.erase(columnName.find_last_not_of(" \t") + 1);

                for (int i = 0; i < columnCount; ++i) {
                    std::string requestedColumn = selectedColumns[i];
                    size_t dotPos = requestedColumn.find('.');

                    // Убираем префикс таблицы из имени колонки, если он есть
                    std::string tablePrefix;
                    if (dotPos != std::string::npos) {
                        tablePrefix = requestedColumn.substr(0, dotPos);
                        requestedColumn = requestedColumn.substr(dotPos + 1);

                        // Пропускаем, если колонка не принадлежит текущей таблице
                        if (tablePrefix != tableNames[t]) {
                            continue;
                        }
                    }

                    if (columnName == requestedColumn) {
                        columnIndices[foundColumns++] = colIdx;
                        break;
                    }
                }
                colIdx++;
            }

            // // Проверка, все ли указанные колонки были найдены
            // if (foundColumns != columnCount) {
            //     std::cerr << "Some columns do not exist in the specified table(s).\n";
            //     return;
            // }

            // Чтение строк и вывод значений выбранных колонок
            std::string line;
            while (std::getline(csvFile, line)) {
                std::istringstream lineStream(line);
                std::string value;
                int colIdx = 0;
                int foundIdx = 0;

                while (std::getline(lineStream, value, ',')) {
                    if (colIdx == columnIndices[foundIdx]) {
                        std::cout << value;
                        if (++foundIdx < columnCount) std::cout << ", ";
                    }
                    colIdx++;
                }
                std::cout << "\n";
            }
            csvFile.close();
        }
    }
}
