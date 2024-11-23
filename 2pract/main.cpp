#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>  // Для функции inet_ntoa
#include <unistd.h>
#include "tablejson.hpp"
#include "insert.hpp"
#include "delete.hpp"
#include "select.hpp"

std::mutex db_mutex; // Для синхронизации доступа к базе данных

// Функция для обработки клиентского подключения
void handleClient(int client_socket, TableJson &tableJS) {
    char buffer[1024];
    std::string command;

    // Получаем информацию о клиенте
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    if (getpeername(client_socket, (sockaddr*)&client_addr, &client_len) == 0) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "Client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
    }

    while (true) {
        ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) break;  // Если чтение не удалось или клиент закрыл соединение
        
        buffer[bytes_read] = '\0';  // Завершаем строку
        command = std::string(buffer);

        std::string response;

        // Защищаем доступ к базе данных мьютексом
        std::lock_guard<std::mutex> lock(db_mutex);

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
            selectFromTables(command, tableJS);  // Выполняем SELECT, но не сохраняем результат в переменной
            response = "SELECT executed successfully.\n";  // Можно отправить этот ответ или обработать по-другому
        } else {
            response = "Incorrect command.\n";
        }

        // Отправка ответа клиенту
        write(client_socket, response.c_str(), response.size());
    }

    // Вывод информации об отключении клиента
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "Client disconnected: " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

    // Закрытие соединения с клиентом
    close(client_socket);
}

int main() {
    TableJson tableJS;
    parsing(tableJS);  // Загружаем данные таблицы

    // Создаем серверный сокет
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Настроим серверный адрес (порт 7432)
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Принимаем соединения с любого IP
    server_addr.sin_port = htons(7432);  // Порт 7432

    // Привязываем сокет к порту
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket\n";
        return 1;
    }

    // Начинаем прослушивание соединений
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error listening on socket\n";
        return 1;
    }
    std::cout << "Server is listening on port 7432...\n";

    while (true) {
        // Принимаем соединение от клиента
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            std::cerr << "Error accepting client\n";
            continue;
        }

        // Создаем новый поток для обработки клиента
        std::thread client_thread(handleClient, client_socket, std::ref(tableJS));
        client_thread.detach();  // Отсоединяем поток для асинхронной обработки
    }

    // Закрытие серверного сокета
    close(server_socket);
    return 0;
}
