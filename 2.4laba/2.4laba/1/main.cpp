#include <iostream>
#include <chrono>
#include "SyncPrimitives.hpp"

int main(int argc, char* argv[]) {
    // Проверка на количество переданных аргументов
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <quantityThreads> <quantityChar> <syncPrimitiveType>" << std::endl;
        return 1;
    }

    // Чтение параметров из командной строки
    int quantityThreads = std::stoi(argv[1]);
    int quantityChar = std::stoi(argv[2]);
    int syncPrimitiveType = std::stoi(argv[3]);

    // Создание объекта SyncPrimitives с количеством потоков
    SyncPrimitives syncPrimitives(quantityThreads);

    // Начало замера времени
    auto start = std::chrono::high_resolution_clock::now();

    // Запуск выбранного синхронизационного примитива
    syncPrimitives.CreateThreads(quantityThreads, quantityChar, syncPrimitiveType);

    // Конец замера времени
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Вывод времени выполнения
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    return 0;
}
