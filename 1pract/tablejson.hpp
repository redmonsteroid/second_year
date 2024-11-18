#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "json.hpp"

// Структура для представления колонки
class Column {
public:
    std::string column;
    Column* next;

    Column(const std::string& column, Column* next = nullptr) : column(column), next(next) {}
};

// Структура для представления таблицы
class Table {
public:
    std::string table;
    Column* column;
    Table* next;

    Table(const std::string& table, Column* column = nullptr, Table* next = nullptr) 
        : table(table), column(column), next(next) {}

    // Деструктор для удаления связанных объектов Column и Table
    ~Table() {
        while (column) {
            Column* tmp = column;
            column = column->next;
            delete tmp;
        }
    }
};

// Класс для управления схемой и таблицами
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

    // Метод для добавления таблицы и колонок
    void addTable(const std::string& tableName, const std::string columns[], int columnCount) {
        Column* columnHead = nullptr;
        for (int i = columnCount - 1; i >= 0; --i) {
            columnHead = new Column(columns[i], columnHead);
        }
        tablehead = new Table(tableName, columnHead, tablehead);
    }

    // Метод для проверки существования таблицы
    bool isTableExist(const std::string& name) const {
        Table* current = tablehead;
        while (current) {
            if (current->table == name) return true;
            current = current->next;
        }
        return false;
    }
};

// Прототипы функций
void removeDirectory(const std::filesystem::path& directoryPath);
void createDirectoriesAndFiles(const std::filesystem::path& schemePath, const nlohmann::json& structure, TableJson& tableJS);
void parsing(TableJson& tableJS);
