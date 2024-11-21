#include "rapidcsv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "select.hpp"

// Функция для удаления префикса таблицы и очистки от лишних пробелов
// Функция для удаления префикса таблицы и очистки от лишних пробелов
std::string removeTablePrefix(const std::string& columnName) {
    size_t dotPos = columnName.find('.');
    std::string cleanCol = (dotPos != std::string::npos) ? columnName.substr(dotPos + 1) : columnName;

    // Убираем кавычки и пробелы вокруг имени колонки
    cleanCol.erase(0, cleanCol.find_first_not_of(" '\""));
    cleanCol.erase(cleanCol.find_last_not_of(" '\"") + 1);
    
    std::cout << "[DEBUG] Cleaned column name: " << cleanCol << "\n";  // Отладка
    return cleanCol;
}


// Функция для проверки условий WHERE
bool checkCondition(const std::string& cellValue, const std::string& condition) {
    // Очищаем condition от кавычек и пробелов
    std::string cleanedCondition = condition;
    if (cleanedCondition.front() == '\'' && cleanedCondition.back() == '\'') {
        cleanedCondition = cleanedCondition.substr(1, cleanedCondition.size() - 2);
    }
    cleanedCondition.erase(0, cleanedCondition.find_first_not_of(" '\""));
    cleanedCondition.erase(cleanedCondition.find_last_not_of(" '\"") + 1);

    // Очищаем cellValue от пробелов и кавычек
    std::string cleanedCellValue = cellValue;
    cleanedCellValue.erase(0, cleanedCellValue.find_first_not_of(" '\""));
    cleanedCellValue.erase(cleanedCellValue.find_last_not_of(" '\"") + 1);

    std::cout << "[DEBUG] Comparing cellValue: '" << cleanedCellValue 
              << "' with cleanedCondition: '" << cleanedCondition << "'\n";  // Отладка

    return cleanedCellValue == cleanedCondition;  // Сравниваем после очистки
}




// Функция для удаления префикса таблицы и очистки от лишних пробелов


void crossJoin(rapidcsv::Document& table1, rapidcsv::Document& table2, 
               const DynamicArray& selectedColumns, 
               const std::string& whereCol1, const std::string& whereCond1,
               const std::string& whereCol2, const std::string& whereCond2, 
               const std::string& logicalOp) {
    std::ofstream outFile("cross_join_result.csv");
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open output file: cross_join_result.csv");
    }

    outFile << selectedColumns.get(0) << "," << selectedColumns.get(1) << "\n";

    int table1RowCount = table1.GetRowCount();
    int table2RowCount = table2.GetRowCount();

    std::string table1Col = removeTablePrefix(selectedColumns.get(0));
    std::string table2Col = removeTablePrefix(selectedColumns.get(1));
    std::string whereCol1Clean = removeTablePrefix(whereCol1);
    std::string whereCol2Clean = removeTablePrefix(whereCol2);

    // Если условия WHERE нет, просто добавляем все строки из обеих таблиц
    if (whereCol1.empty() && whereCol2.empty()) {
        for (int i = 0; i < table1RowCount; ++i) {
            std::string cell1 = table1.GetCell<std::string>(table1Col, i);
            for (int j = 0; j < table2RowCount; ++j) {
                std::string cell2 = table2.GetCell<std::string>(table2Col, j);
                outFile << cell1 << "," << cell2 << "\n";
                std::cout << "[DEBUG] Cross Join Row: " << cell1 << "," << cell2 << "\n";
            }
        }
    } else {
        // Если есть условия, фильтруем их
        for (int i = 0; i < table1RowCount; ++i) {
            std::string cell1 = table1.GetCell<std::string>(table1Col, i);
            std::string whereCell1 = whereCol1.empty() ? "" : table1.GetCell<std::string>(whereCol1Clean, i);

            for (int j = 0; j < table2RowCount; ++j) {
                std::string cell2 = table2.GetCell<std::string>(table2Col, j);
                std::string whereCell2 = whereCol2.empty() ? "" : table2.GetCell<std::string>(whereCol2Clean, j);

                std::cout << "[DEBUG] Checking conditions for whereCell1: " << whereCell1 
                          << ", whereCond1: " << whereCond1 
                          << ", whereCell2: " << whereCell2 
                          << ", whereCond2: " << whereCond2 << "\n";

                bool condition1 = whereCol1.empty() || checkCondition(whereCell1, whereCond1);
                bool condition2 = whereCol2.empty() || checkCondition(whereCell2, whereCond2);

                bool finalCondition = false;
                if (logicalOp == "AND") {
                    finalCondition = condition1 && condition2;
                } else if (logicalOp == "OR") {
                    finalCondition = condition1 || condition2;
                } else {
                    finalCondition = condition1;
                }

                if (finalCondition) {
                    outFile << cell1 << "," << cell2 << "\n";
                    std::cout << "[DEBUG] Cross Join Row: " << cell1 << "," << cell2 << "\n";
                }
            }
        }
    }

    outFile.close();
    std::cout << "[DEBUG] Cross join result has been written to cross_join_result.csv\n";
}


void selectFromTables(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName;
    DynamicArray selectedColumns;
    DynamicArray tableNames;

    iss >> word;
    while (iss >> word && word != "FROM") {
        if (word.back() == ',') word.pop_back();
        selectedColumns.add(word);
        std::cout << "[DEBUG] Selected column: " << word << "\n";
    }

    while (iss >> tableName) {
        if (tableName == "WHERE") break;
        if (tableName.back() == ',') tableName.pop_back();
        tableNames.add(tableName);
        std::cout << "[DEBUG] Table name: " << tableName << "\n";
    }

    // Загружаем данные для таблиц
    rapidcsv::Document table1(tableJS.schemeName + "/" + tableNames.get(0) + "/1.csv");
    rapidcsv::Document table2(tableJS.schemeName + "/" + tableNames.get(1) + "/1.csv");

    // Ищем условие WHERE
    size_t wherePos = command.find("WHERE");
    std::string whereCol1, whereCond1, whereCol2, whereCond2, logicalOp;

    if (wherePos != std::string::npos) {
        std::string condition = command.substr(wherePos + 5); // Извлекаем строку после WHERE
        size_t andPos = condition.find(" AND ");
        size_t orPos = condition.find(" OR ");

        if (andPos != std::string::npos) {
            logicalOp = "AND";
            whereCol1 = condition.substr(0, andPos);
            whereCol2 = condition.substr(andPos + 5);
        } else if (orPos != std::string::npos) {
            logicalOp = "OR";
            whereCol1 = condition.substr(0, orPos);
            whereCol2 = condition.substr(orPos + 4);
        } else {
            whereCol1 = condition;
        }

        whereCond1 = whereCol1.substr(whereCol1.find('=') + 1);
        whereCol1 = whereCol1.substr(0, whereCol1.find('='));
        whereCond1.erase(0, whereCond1.find_first_not_of(' '));
        whereCond1.erase(whereCond1.find_last_not_of(' ') + 1);

        if (!whereCol2.empty()) {
            whereCond2 = whereCol2.substr(whereCol2.find('=') + 1);
            whereCol2 = whereCol2.substr(0, whereCol2.find('='));
            whereCond2.erase(0, whereCond2.find_first_not_of(' '));
            whereCond2.erase(whereCond2.find_last_not_of(' ') + 1);
        }
    }

    std::cout << "[DEBUG] whereCol1: " << whereCol1 << "\n";
    std::cout << "[DEBUG] whereCond1: " << whereCond1 << "\n";
    std::cout << "[DEBUG] whereCol2: " << whereCol2 << "\n";
    std::cout << "[DEBUG] whereCond2: " << whereCond2 << "\n";
    std::cout << "[DEBUG] logicalOp: " << logicalOp << "\n";

    std::cout << "[DEBUG] Table 1 Data:\n";


    // Если условие WHERE существует, выполняем crossJoin с фильтрацией
    crossJoin(table1, table2, selectedColumns, whereCol1, whereCond1, whereCol2, whereCond2, logicalOp);
}

