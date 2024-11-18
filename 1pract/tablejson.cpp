#include "tablejson.hpp"

void removeDirectory(const std::filesystem::path& directoryPath) {
    try {
        std::filesystem::remove_all(directoryPath);
        std::cout << "Directory removed: " << directoryPath << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error removing directory: " << e.what() << std::endl;
    }
}

void createDirectoriesAndFiles(const std::filesystem::path& schemePath, const nlohmann::json& structure, TableJson& tableJS) {
    try {
        std::filesystem::create_directory(schemePath);
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
    createDirectoriesAndFiles(tableJS.schemeName, schemaData["structure"], tableJS);
}
