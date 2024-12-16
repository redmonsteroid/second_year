#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    // Адрес и порт сервера
    const std::string server_ip = "127.0.0.1"; // Локальный IP (для подключения к серверу на том же компьютере)
    const int server_port = 7432; // Порт сервера

    // Создаем сокет
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Настроим адрес сервера
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // Подключаемся к серверу
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection to server failed\n";
        close(client_socket);
        return 1;
    }
    std::cout << "Connected to server " << server_ip << ":" << server_port << "\n";

    std::string command;
    while (true) {
        // Чтение команды от пользователя
        std::cout << "Enter command (INSERT, DELETE, SELECT, EXIT): ";
        std::getline(std::cin, command);

        if (command == "EXIT") {
            // Если команда "EXIT", выходим из программы
            break;
        }

        // Отправка команды на сервер
        ssize_t bytes_sent = send(client_socket, command.c_str(), command.size(), 0);
        if (bytes_sent < 0) {
            std::cerr << "Error sending command\n";
            break;
        }

        // Получение ответа от сервера
        char buffer[1024];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0) {
            std::cerr << "Error receiving response\n";
            break;
        }

        buffer[bytes_received] = '\0';  // Завершаем строку
        std::cout << "Server response: " << buffer << "\n";
    }

    // Закрытие соединения
    close(client_socket);
    std::cout << "Disconnected from server\n";
    return 0;
}
