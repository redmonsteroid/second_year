#include <iostream>
#include <string>
#include "tablejson.hpp"
#include "insert.hpp"

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
        } else {
            std::cerr << "Incorrect command.\n";
        }
    }
    return 0;
}
