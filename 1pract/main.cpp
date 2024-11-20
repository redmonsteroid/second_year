#include <iostream>
#include <string>
#include "tablejson.hpp"
#include "insert.hpp"
#include "delete.hpp"
#include "select.hpp"

int main() {
    TableJson tableJS;
    parsing(tableJS);

    std::string command;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        if (command == "EXIT") {
            break;
        } else if (command.find("INSERT") == 0) {
            insert(command, tableJS);
        } else if (command.find("DELETE") == 0) {
            deleteFromTable(command, tableJS);
        } else if (command.find("SELECT") == 0) {
            selectFromTables(command, tableJS);
        }else {
            std::cerr << "Incorrect command.\n";
        }
    }
    return 0;
}
