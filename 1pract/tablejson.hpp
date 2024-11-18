#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "json.hpp"

class Column {
public:
    std::string column;
    Column* next;

    Column(const std::string& column, Column* next = nullptr) : column(column), next(next) {}
};

class Table {
public:
    std::string table;
    Column* column;
    Table* next;

    Table(const std::string& table, Column* column = nullptr, Table* next = nullptr) 
        : table(table), column(column), next(next) {}

    // Удаляем все связанные объекты Column и Table
    ~Table() {
        while (column) {
            Column* tmp = column;
            column = column->next;
            delete tmp;
        }
    }
};

class TableJson {
public:
    int tableSize;
    std::string schemeName;
    Table* tablehead;

    TableJson() : tableSize(0), schemeName(""), tablehead(nullptr) {}

    ~TableJson() {
        while (tablehead) {
            Table* tmp = tablehead;
            tablehead = tablehead->next;
            delete tmp;
        }
    }

    // Добавление таблицы и колонок в список
    void addTable(const std::string& tableName, const std::string columns[], int columnCount) {
        Column* columnHead = nullptr;
        for (int i = columnCount - 1; i >= 0; --i) {
            columnHead = new Column(columns[i], columnHead);
        }
        tablehead = new Table(tableName, columnHead, tablehead);
    }

    // Проверка существования таблицы
    bool isTableExist(const std::string& name) const {
        Table* current = tablehead;
        while (current) {
            if (current->table == name) return true;
            current = current->next;
        }
        return false;
    }
};

// Функции для работы с таблицами
void removeDirectory(const std::filesystem::path& directoryPath);
void createDirectoriesAndFiles(const std::filesystem::path& schemePath, const nlohmann::json& structure, TableJson& tableJS);
void parsing(TableJson& tableJS);
