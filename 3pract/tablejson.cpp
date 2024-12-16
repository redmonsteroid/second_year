#include "tablejson.hpp"
#include <filesystem>

void removeDirectory(const std::filesystem::path& directoryPath) {
    try {
        std::filesystem::remove_all(directoryPath);
        std::cout << "Directory removed: " << directoryPath << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error removing directory: " << e.what() << std::endl;
    }
}

void loadExistingSchema(const std::filesystem::path& schemePath, TableJson& tableJS) {
    // Загрузим таблицы из существующей базы данных
    for (const auto& entry : std::filesystem::directory_iterator(schemePath)) {
        if (entry.is_directory()) {
            std::string tableName = entry.path().filename().string();
            std::cout << "Loading table: " << tableName << std::endl;
            std::ifstream csvFile(entry.path() / "1.csv");
            if (csvFile.is_open()) {
                // Чтение структуры колонок таблицы
                std::string line;
                std::getline(csvFile, line); // Читаем строку с названиями колонок
                std::istringstream ss(line);
                std::string columnName;
                std::vector<std::string> columns;
                while (std::getline(ss, columnName, ',')) {
                    columns.push_back(columnName);
                }
                csvFile.close();
                // Добавляем таблицу в структуру TableJson
                tableJS.addTable(tableName, columns.data(), columns.size());
            }
        }
    }
}

void createDirectoriesAndFiles(const std::filesystem::path& schemePath, const nlohmann::json& structure, TableJson& tableJS) {
    // Проверяем, существует ли директория схемы
    if (std::filesystem::exists(schemePath)) {
        std::cout << "Database '" << schemePath.filename().string() << "' already exists.\n";
    } else {
        try {
            // Если директория не существует, создаем её и все таблицы
            std::filesystem::create_directory(schemePath);
            std::cout << "Database created: " << schemePath.filename().string() << "\n";
            
            // Проходим по каждой таблице в структуре
            for (const auto& [tableName, columns] : structure.items()) {
                std::string columnNames[10];
                int columnCount = 0;
                for (const auto& col : columns) {
                    if (columnCount < 10) {
                        columnNames[columnCount++] = col.get<std::string>();
                    }
                }

                // Добавляем таблицу и её колонки в структуру TableJson
                tableJS.addTable(tableName, columnNames, columnCount);

                std::filesystem::path tablePath = schemePath / tableName;
                std::filesystem::create_directory(tablePath);
                std::ofstream csvFile(tablePath / "1.csv");
                if (csvFile.is_open()) {
                    for (int i = 0; i < columnCount; ++i) {
                        csvFile << columnNames[i];
                        if (i < columnCount - 1) csvFile << ",";
                    }
                    csvFile << "\n";
                    csvFile.close();
                }

                // Создание файлов для первичного ключа и блокировки
                std::ofstream pkFile(tablePath / (tableName + "_pk_sequence.txt"));
                pkFile << 0;
                pkFile.close();

                std::ofstream lockFile(tablePath / (tableName + "_lock.txt"));
                lockFile << "unlocked";
                lockFile.close();
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating directories or files: " << e.what() << std::endl;
        }
    }
}

void parsing(TableJson& tableJS) {
    std::ifstream schemaFile("schema.json");
    if (!schemaFile.is_open()) {
        std::cerr << "Failed to open schema.json\n";
        return;
    }

    nlohmann::json schemaData;
    schemaFile >> schemaData;
    schemaFile.close();

    tableJS.schemeName = schemaData["name"];
    tableJS.tableSize = schemaData["tuples_limit"];

    // Строим путь для базы данных
    std::filesystem::path schemePath = tableJS.schemeName;

    // Если база данных уже существует, загружаем её
    if (std::filesystem::exists(schemePath)) {
        std::cout << "Database already exists. Loading schema...\n";
        loadExistingSchema(schemePath, tableJS);
    } else {
        std::cout << "Database does not exist. Creating new one...\n";
        createDirectoriesAndFiles(schemePath, schemaData["structure"], tableJS);
    }
}
bool is_locked(const std::string& tableName, const std::string& schemaName) {
    std::string lockFile = std::filesystem::current_path().string() + "/" + schemaName + "/" + tableName + "/" + tableName + "_lock.txt";
    std::ifstream file(lockFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open lock file: " << lockFile << std::endl;
        return false;
    }
    std::string status;
    file >> status;
    return (status == "locked");
}

void toggle_lock(const std::string& tableName, const std::string& schemaName) {
    std::string lockFile = std::filesystem::current_path().string() + "/" + schemaName + "/" + tableName + "/" + tableName + "_lock.txt";
    std::ifstream fileIn(lockFile);
    if (!fileIn.is_open()) {
        std::cerr << "Failed to open lock file for reading: " << lockFile << std::endl;
        return;
    }
    std::string currentStatus;
    fileIn >> currentStatus;
    fileIn.close();

    std::ofstream fileOut(lockFile);
    if (!fileOut.is_open()) {
        std::cerr << "Failed to open lock file for writing: " << lockFile << std::endl;
        return;
    }
    fileOut << (currentStatus == "locked" ? "unlocked" : "locked");
}
int amountOfCSV(const TableJson& tableJS, const std::string& tableName) {
    int count = 0;
    std::string directoryPath = tableJS.schemeName + "/" + tableName;

    // Перебираем все файлы в директории таблицы
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            count++;
        }
    }
    return count;
}