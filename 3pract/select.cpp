#include "select.hpp"
#include "rapidcsv.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

struct Condition {
    std::string column;
    std::string op;
    std::string value;
};

// Удаление префикса таблицы и очистка от лишних пробелов
std::string removeTablePrefix(const std::string& columnName) {
    size_t dotPos = columnName.find('.');
    std::string cleanCol = (dotPos != std::string::npos) ? columnName.substr(dotPos + 1) : columnName;
    cleanCol.erase(0, cleanCol.find_first_not_of(" '\""));
    cleanCol.erase(cleanCol.find_last_not_of(" '\"") + 1);
    return cleanCol;
}

// Функция для очистки значения от кавычек и пробелов
std::string cleanValue(std::string val) {
    val.erase(0, val.find_first_not_of(" \t\n\r'\""));
    val.erase(val.find_last_not_of(" \t\n\r'\"") + 1);
    return val;
}

// Проверка условий WHERE
bool checkCondition(const std::string& cellValue, const std::string& condition, const std::string& op) {
    std::string cleanedCondition = cleanValue(condition);
    std::string cleanedCellValue = cleanValue(cellValue);

    std::cout << "[DEBUG] Comparing values: cleanedCellValue = '" << cleanedCellValue
              << "', cleanedCondition = '" << cleanedCondition << "'\n";

    // Проверяем, не осталась ли точка с запятой на конце condition
    if (!cleanedCondition.empty() && cleanedCondition.back() == ';') {
        cleanedCondition.pop_back();
        cleanedCondition = cleanValue(cleanedCondition);
    }

    try {
        double cellNumericValue = std::stod(cleanedCellValue);
        double conditionNumericValue = std::stod(cleanedCondition);

        if (op == "=") return cellNumericValue == conditionNumericValue;
        else if (op == "<") return cellNumericValue < conditionNumericValue;
        else if (op == ">") return cellNumericValue > conditionNumericValue;
        else if (op == "<=") return cellNumericValue <= conditionNumericValue;
        else if (op == ">=") return cellNumericValue >= conditionNumericValue;
    } catch (...) {
        // Строковое сравнение, если не число
        std::cout << "[DEBUG] Performing string comparison\n";
        if (op == "=") return cleanedCellValue == cleanedCondition;
        else if (op == "<") return cleanedCellValue < cleanedCondition;
        else if (op == ">") return cleanedCellValue > cleanedCondition;
        else if (op == "<=") return cleanedCellValue <= cleanedCondition;
        else if (op == ">=") return cleanedCellValue >= cleanedCondition;
    }

    return false;
}

void parseSingleCondition(const std::string& combined, std::vector<Condition>& conditions) {
    // Сначала удалим лишние пробелы и точки с запятой
    std::string str = combined;
    str = cleanValue(str);
    if (!str.empty() && str.back() == ';') {
        str.pop_back();
        str = cleanValue(str);
    }

    // Проверяем двухсимвольные операторы сначала
    size_t opPos = std::string::npos;
    std::string opCandidates[] = {"<=", ">=", "<", ">", "="};
    std::string foundOp;
    for (auto &opc : opCandidates) {
        size_t pos = str.find(opc);
        if (pos != std::string::npos) {
            opPos = pos;
            foundOp = opc;
            break;
        }
    }

    if (opPos == std::string::npos) {
        // Не нашли оператор - ошибка
        throw std::runtime_error("No valid operator found in condition: " + combined);
    }

    Condition c;
    c.column = str.substr(0, opPos);
    c.op = foundOp;
    c.value = str.substr(opPos + foundOp.size());

    c.column = cleanValue(c.column);
    c.value = cleanValue(c.value);

    conditions.push_back(c);
}

void parseConditions(const std::string& conditionStr,
                     std::vector<Condition>& conditions,
                     std::vector<std::string>& logicOps) 
{
    std::istringstream iss(conditionStr);
    std::string token;
    std::vector<std::string> tokens;
    while (iss >> token) {
        tokens.push_back(token);
    }

    std::vector<std::string> conditionParts;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "AND" || tokens[i] == "OR") {
            if (!conditionParts.empty()) {
                std::string combined;
                for (size_t j = 0; j < conditionParts.size(); ++j) {
                    if (j > 0) combined += " ";
                    combined += conditionParts[j];
                }
                conditionParts.clear();
                parseSingleCondition(combined, conditions);
            }
            logicOps.push_back(tokens[i]);
        } else {
            conditionParts.push_back(tokens[i]);
        }
    }

    if (!conditionParts.empty()) {
        std::string combined;
        for (size_t j = 0; j < conditionParts.size(); ++j) {
            if (j > 0) combined += " ";
            combined += conditionParts[j];
        }
        parseSingleCondition(combined, conditions);
    }
}


// Функция проверки всех условий по логическим связкам
bool checkAllConditions(const std::vector<Condition>& conditions,
                        const std::vector<std::string>& headers,
                        const std::vector<std::string>& rowValues,
                        const std::vector<std::string>& logicOps) 
{
    if (conditions.empty()) {
        // Нет условий
        return true;
    }

    // Находим индексы колонок для каждого условия
    std::vector<int> conditionIndices;
    for (const auto &cond : conditions) {
        auto it = std::find(headers.begin(), headers.end(), cond.column);
        if (it == headers.end()) {
            throw std::runtime_error("Condition column not found: " + cond.column);
        }
        conditionIndices.push_back((int)std::distance(headers.begin(), it));
    }

    // Проверяем каждое условие
    std::vector<bool> results;
    for (size_t i = 0; i < conditions.size(); ++i) {
        bool res = checkCondition(rowValues[conditionIndices[i]], conditions[i].value, conditions[i].op);
        results.push_back(res);
    }

    // Объединяем результаты с учётом логических операторов
    bool finalResult = results[0];
    for (size_t i = 0; i < logicOps.size(); ++i) {
        if (logicOps[i] == "AND") {
            finalResult = finalResult && results[i+1];
        } else if (logicOps[i] == "OR") {
            finalResult = finalResult || results[i+1];
        } else {
            // Если оператор не AND/OR - ошибка
            throw std::runtime_error("Unknown logical operator in WHERE clause.");
        }
    }

    return finalResult;
}

// Функция парсинга условий из строки после WHERE

void selectFromTables(const std::string& command, TableJson& tableJS) {
    std::istringstream iss(command);
    std::string word, tableName;
    std::vector<std::string> selectedColumns;

    // Парсим команду SELECT
    iss >> word;  // "SELECT"
    while (iss >> word && word != "FROM") {
        if (!word.empty() && word.back() == ',') word.pop_back();
        selectedColumns.push_back(word);
    }
    iss >> tableName;

    // Парсим WHERE-условия, если есть
    std::vector<Condition> conditions;
    std::vector<std::string> logicOps;
    size_t wherePos = command.find("WHERE");
    if (wherePos != std::string::npos) {
        std::string conditionStr = command.substr(wherePos + 5); // после слова WHERE
        // Удалим ведущие и конечные пробелы
        conditionStr.erase(0, conditionStr.find_first_not_of(" \t\n\r"));
        conditionStr.erase(conditionStr.find_last_not_of(" \t\n\r")+1);
        parseConditions(conditionStr, conditions, logicOps);
    }

    // Загружаем таблицу
    std::string filePath = tableJS.schemeName + "/" + tableName + "/1.csv";
    std::ifstream tableFile(filePath);
    if (!tableFile.is_open()) {
        throw std::runtime_error("Table file does not exist: " + filePath);
    }

    // Читаем заголовки
    std::string headerLine;
    std::getline(tableFile, headerLine);
    std::vector<std::string> headers;
    {
        std::istringstream headerStream(headerLine);
        std::string header;
        while (std::getline(headerStream, header, ',')) {
            header = cleanValue(header);
            headers.push_back(header);
        }
    }

    // Если звездочка, выбираем все колонки
    if (selectedColumns.size() == 1 && selectedColumns[0] == "*") {
        selectedColumns = headers;
    }

    // Ищем индексы выбранных колонок
    std::vector<int> columnIndices;
    for (const auto& col : selectedColumns) {
        auto it = std::find(headers.begin(), headers.end(), col);
        if (it != headers.end()) {
            columnIndices.push_back((int)std::distance(headers.begin(), it));
        } else {
            throw std::runtime_error("Column not found: " + col);
        }
    }

    // Фильтруем строки и записываем результат
    std::ofstream resultFile("select_result.csv");
    // Записываем заголовки выбранных колонок
    for (size_t i = 0; i < columnIndices.size(); ++i) {
        if (i > 0) resultFile << ",";
        resultFile << headers[columnIndices[i]];
    }
    resultFile << "\n";

    std::string row;
    while (std::getline(tableFile, row)) {
        std::istringstream rowStream(row);
        std::vector<std::string> values;
        std::string value;
        while (std::getline(rowStream, value, ',')) {
            // Очищаем каждое прочитанное значение
            value = cleanValue(value);
            values.push_back(value);
        }

        // Проверяем условия
        bool meetsCondition = checkAllConditions(conditions, headers, values, logicOps);

        if (meetsCondition) {
            for (size_t i = 0; i < columnIndices.size(); ++i) {
                if (i > 0) resultFile << ",";
                resultFile << values[columnIndices[i]];
            }
            resultFile << "\n";
        }
    }
    resultFile.close();
}
