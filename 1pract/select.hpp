#pragma once
#include <string>
#include "tablejson.hpp"

void splitDot(const std::string& word, std::string& table, std::string& column, TableJson& tableJS);
int countCsv(TableJson& tableJS, const std::string& table);
void crossJoin(TableJson& tableJS, const std::string& table1, const std::string& table2, const std::string& column1, const std::string& column2);
void selectFromTables(const std::string& command, TableJson& tableJS);
