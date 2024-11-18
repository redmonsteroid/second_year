#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <random>

using namespace std;

struct Purchase {
    int receipt_number;
    string product;
    double price;
    int quantity;
};

mutex mtx;

// Функция для однопоточной обработки
void processSingleThread(const vector<Purchase>& purchases,
                         unordered_map<string, int>& totalQuantity,
                         unordered_map<string, vector<int>>& receiptNumbers) {
    for (const auto& purchase : purchases) {
        totalQuantity[purchase.product] += purchase.quantity;
        receiptNumbers[purchase.product].push_back(purchase.receipt_number);
    }
}

// Функция для многопоточной обработки
void processMultiThread(const vector<Purchase>& purchases, 
                        int start, int end,
                        unordered_map<string, int>& totalQuantity,
                        unordered_map<string, vector<int>>& receiptNumbers) {
    unordered_map<string, int> localTotalQuantity;
    unordered_map<string, vector<int>> localReceiptNumbers;

    for (int i = start; i < end; ++i) {
        const auto& purchase = purchases[i];
        localTotalQuantity[purchase.product] += purchase.quantity;
        localReceiptNumbers[purchase.product].push_back(purchase.receipt_number);
    }

    lock_guard<mutex> lock(mtx);
    for (const auto& [product, quantity] : localTotalQuantity) {
        totalQuantity[product] += quantity;
    }
    for (const auto& [product, receipts] : localReceiptNumbers) {
        receiptNumbers[product].insert(receiptNumbers[product].end(), receipts.begin(), receipts.end());
    }
}

// Генерация случайных данных
void generateData(vector<Purchase>& purchases, int dataSize) {
    vector<string> products = {"Apple", "Banana", "Orange", "Grapes", "Peach"};
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> receiptDist(1, 1000);
    uniform_int_distribution<> productDist(0, products.size() - 1);
    uniform_real_distribution<> priceDist(0.5, 10.0);
    uniform_int_distribution<> quantityDist(1, 100);

    for (int i = 0; i < dataSize; ++i) {
        purchases.push_back({receiptDist(gen), products[productDist(gen)], priceDist(gen), quantityDist(gen)});
    }
}

int main() {
    // Увеличенный объем данных
    int dataSize = 100000;  // Количество записей
    int numThreads = 4;     // Количество потоков

    // Инициализация данных покупок
    vector<Purchase> purchases;
    generateData(purchases, dataSize);

    // Однопоточная обработка
    unordered_map<string, int> singleThreadQuantity;
    unordered_map<string, vector<int>> singleThreadReceipts;

    auto start = chrono::high_resolution_clock::now();
    processSingleThread(purchases, singleThreadQuantity, singleThreadReceipts);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> singleThreadTime = end - start;
    cout << "Single-threaded processing time: " << singleThreadTime.count() << " seconds\n";

    // Многопоточная обработка
    unordered_map<string, int> multiThreadQuantity;
    unordered_map<string, vector<int>> multiThreadReceipts;
    vector<thread> threads;

    start = chrono::high_resolution_clock::now();
    int chunkSize = dataSize / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        int startIdx = i * chunkSize;
        int endIdx = (i == numThreads - 1) ? dataSize : startIdx + chunkSize;
        threads.emplace_back(processMultiThread, ref(purchases), startIdx, endIdx,
                             ref(multiThreadQuantity), ref(multiThreadReceipts));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> multiThreadTime = end - start;
    cout << "Multi-threaded processing time: " << multiThreadTime.count() << " seconds\n";

    // Вывод результатов
    // cout << "\nResults (Single-threaded):\n";
    // for (const auto& [product, quantity] : singleThreadQuantity) {
    //     cout << "Product: " << product << ", Total Sold: " << quantity << "\n";
    //     cout << "Receipts: ";
    //     for (int receipt : singleThreadReceipts[product]) {
    //         cout << receipt << " ";
    //     }
    //     cout << "\n";
    // }

    // cout << "\nResults (Multi-threaded):\n";
    // for (const auto& [product, quantity] : multiThreadQuantity) {
    //     cout << "Product: " << product << ", Total Sold: " << quantity << "\n";
    //     cout << "Receipts: ";
    //     for (int receipt : multiThreadReceipts[product]) {
    //         cout << receipt << " ";
    //     }
    //     cout << "\n";
    // }

    return 0;
}
