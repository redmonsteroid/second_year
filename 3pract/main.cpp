#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>  // Для inet_ntop
#include <unistd.h>
#include "tablejson.hpp"
#include "insert.hpp"
#include "delete.hpp"
#include "select.hpp"

std::mutex db_mutex; // Для синхронизации доступа к базе данных

// Функция для обработки клиентского подключения
void handleClient(int client_socket, TableJson &tableJS) {
    char buffer[4096];
    std::string command;

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    // Получаем информацию о подключённом клиенте
    if (getpeername(client_socket, (sockaddr*)&client_addr, &client_len) == 0) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "Client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
    }

    while (true) {
        // Читаем данные от клиента
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        buffer[bytes_read] = '\0';
        command = std::string(buffer);
        std::string response;

        std::lock_guard<std::mutex> lock(db_mutex); // Блокируем доступ к БД

        std::cout << "[DEBUG] Received command: " << command << std::endl;

        try {
            if (command == "EXIT") {
                response = "Disconnecting...\n";
                write(client_socket, response.c_str(), response.size());
                break;
            } else if (command.find("INSERT") == 0) {
                insert(command, tableJS);
                response = "Insert successful.\n";
            } else if (command.find("DELETE") == 0) {
                deleteFromTable(command, tableJS);
                response = "Delete successful.\n";
            } else if (command.find("SELECT") == 0) {
                selectFromTables(command, tableJS);

                // Читаем результат из select_result.csv
                std::ifstream resultFile("select_result.csv");
                if (resultFile.is_open()) {
                    std::stringstream resultBuffer;
                    resultBuffer << resultFile.rdbuf();
                    response = resultBuffer.str();
                    resultFile.close();
                } else {
                    std::cerr << "[ERROR] Could not open select_result.csv." << std::endl;
                    response = "Error: Could not open select_result.csv.\n";
                }
            } else {
                std::cerr << "[ERROR] Unknown command: " << command << std::endl;
                response = "Error: Unknown command.\n";
            }
        } catch (const std::exception &e) {
            std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
            response = std::string("Error: ") + e.what() + "\n";
        }

std::cout << "[DEBUG] Preparing to send response to client." << std::endl;

ssize_t bytes_sent = write(client_socket, response.c_str(), response.size());
if (bytes_sent < 0) {
    std::cerr << "[ERROR] Failed to send response to client." << std::endl;
} else {
    std::cout << "[DEBUG] Sent response: " << response << " (" << bytes_sent << " bytes)" << std::endl;
}

if (shutdown(client_socket, SHUT_WR) == -1) {
    std::cerr << "[ERROR] Failed to shutdown socket for writing." << std::endl;
} else {
    std::cout << "[DEBUG] Socket shutdown for writing." << std::endl;
}
    }
}

int main() {
    TableJson tableJS;
    parsing(tableJS);  // Загружаем данные таблицы

    // Создаём серверный сокет
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Настраиваем серверный адрес
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Принимаем соединения с любого IP
    server_addr.sin_port = htons(7432);       // Порт 7432

    // Привязываем сокет к адресу
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    // Запускаем прослушивание
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }
    std::cout << "Server is listening on port 7432..." << std::endl;

    while (true) {
        // Принимаем входящее соединение
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Error accepting client" << std::endl;
            continue;
        }

        // Создаём поток для обработки клиента
        std::thread client_thread(handleClient, client_socket, std::ref(tableJS));
        client_thread.detach();  // Отсоединяем поток для асинхронной обработки
    }

    close(server_socket);
    return 0;
}
