#include "SyncPrimitives.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <semaphore>
#include <atomic>

SyncPrimitives::SyncPrimitives(int threadCount)
    : barrierGRCBarrier(threadCount) // Инициализация barrier с количеством потоков
{
    srand(static_cast<unsigned int>(time(0)));
}


std::string SyncPrimitives::generateRandomChars(int quantityChar) {
    std::string local_chars;
    for (int i = 0; i < quantityChar; ++i) {
        char random_char = static_cast<char>(rand() % (126 - 32 + 1) + 32);
        local_chars += random_char;
    }
    return local_chars;
}

// Race Condition
void SyncPrimitives::GRCRaceCondition(int thread_id, std::string& buffer, int& quantityChar) {
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
}

// Mutex
void SyncPrimitives::GRCMutex(int thread_id, std::string& buffer, int& quantityChar) {
    std::lock_guard<std::mutex> lock(GRCmutex);
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
}

// Semaphore
void SyncPrimitives::GRCSemaphore(int thread_id, std::string& buffer, int& quantityChar) {
    smphSignalCreateThreadsToGRCSemaphore.acquire();
    
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;

    smphSignalGRCSemaphoreToCreateThreads.release();
}

// SemaphoreSlim
void SyncPrimitives::GRCSemaphoreSlim(int thread_id, std::string& buffer, int& quantityChar) {
    static std::counting_semaphore<1> smphSlim(1);
    smphSlim.acquire();
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
    smphSlim.release();
}

// Barrier
void SyncPrimitives::GRCBarrier(int thread_id, std::string& buffer, int& quantityChar) {
    barrierGRCBarrier.arrive_and_wait();
    std::string local_chars = generateRandomChars(quantityChar);

    std::lock_guard<std::mutex> lock(GRCmutex);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
}

// SpinLock
void SyncPrimitives::GRCSpinLock(int thread_id, std::string& buffer, int& quantityChar) {
    while (spinlock.test_and_set(std::memory_order_acquire));
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
    spinlock.clear(std::memory_order_release);
}

// Реализация методов класса Spinwait

void Spinwait::wait() {
    while (!ready.load(std::memory_order_acquire)) {
        // Цикл ожидания
        std::this_thread::yield(); // Позволяет другим потокам продолжить выполнение
    }
}

void Spinwait::notify() {
    ready.store(true, std::memory_order_release); // Устанавливаем флаг готовности
}

// SpinWait
void SyncPrimitives::GRCSpinWait(int thread_id, std::string& buffer, int& quantityChar) {
    spinwait.wait();  // Wait for notification to continue

    std::string local_chars = generateRandomChars(quantityChar);

    {   
        std::lock_guard<std::mutex> lock(GRCmutex); // Защищаем доступ к буферу
        buffer += local_chars;
    }

    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;

    spinwait.notify();
}

// Monitor
void SyncPrimitives::GRCMonitor(int thread_id, std::string& buffer, int& quantityChar) {
    monitor_.enter();
    std::string local_chars = generateRandomChars(quantityChar);
    buffer += local_chars;
    std::cout << "Thread " << thread_id << " generated: " << local_chars << std::endl;
    monitor_.leave();
}

void SyncPrimitives::startWork() { // только для spinwait
    spinwait.notify();  // Уведомляем все потоки, чтобы они начали работать
}

void SyncPrimitives::CreateThreads(int quantityThreads, int quantityChar, int syncType) {
    std::string buffer;
    std::vector<std::thread> threads;

    for (int i = 0; i < quantityThreads; ++i) {
        switch (syncType) {
            case 0:
                threads.emplace_back(&SyncPrimitives::GRCRaceCondition, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 1:
                threads.emplace_back(&SyncPrimitives::GRCMutex, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 2:
                smphSignalCreateThreadsToGRCSemaphore.release();
                threads.emplace_back(&SyncPrimitives::GRCSemaphore, this, i, std::ref(buffer), std::ref(quantityChar));
                smphSignalGRCSemaphoreToCreateThreads.acquire();
                break;
            case 3:
                threads.emplace_back(&SyncPrimitives::GRCSemaphoreSlim, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 4:
                threads.emplace_back(&SyncPrimitives::GRCBarrier, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 5:
                threads.emplace_back(&SyncPrimitives::GRCSpinLock, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 6:
                threads.emplace_back(&SyncPrimitives::GRCSpinWait, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
            case 7:
                threads.emplace_back(&SyncPrimitives::GRCMonitor, this, i, std::ref(buffer), std::ref(quantityChar));
                break;
        }
    }
    if (syncType == 6) {
        startWork();
    }

    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << "Buffer: " << buffer << std::endl;
}
