#ifndef SYNC_PRIMITIVES_HPP
#define SYNC_PRIMITIVES_HPP

#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <semaphore>
#include <barrier>
#include <atomic>
#include <condition_variable>

class Monitor {
public:
    Monitor() = default;
    
    void enter() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (locked_) {
            cv_.wait(lock);
        }
        locked_ = true;
    }

    void leave() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            locked_ = false;
        }
        cv_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    bool locked_ = false;
};


class Spinwait {
public:
    void wait();
    void notify();

private:
    std::atomic<bool> ready{false};
};

class SyncPrimitives {
public:
    SyncPrimitives(int threadCount);
    
    void GRCRaceCondition(int thread_id, std::string& buffer, int& quantityChar);
    void GRCMutex(int thread_id, std::string& buffer, int& quantityChar);
    void GRCSemaphore(int thread_id, std::string& buffer, int& quantityChar);
    void GRCSemaphoreSlim(int thread_id, std::string& buffer, int& quantityChar);
    void GRCBarrier(int thread_id, std::string& buffer, int& quantityChar);
    void GRCSpinLock(int thread_id, std::string& buffer, int& quantityChar);
    void GRCSpinWait(int thread_id, std::string& buffer, int& quantityChar);
    void GRCMonitor(int thread_id, std::string& buffer, int& quantityChar);
    void startWork();
    void CreateThreads(int quantityThreads, int quantityChar, int syncType);

    // Generates a string of random characters
    std::string generateRandomChars(int quantityChar);

private:
    std::mutex GRCmutex; 
    std::binary_semaphore smphSignalCreateThreadsToGRCSemaphore{0};
    std::binary_semaphore smphSignalGRCSemaphoreToCreateThreads{0};
    std::barrier<> barrierGRCBarrier;
    std::atomic<bool> ready{false};
    std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
    Monitor monitor_;  
    Spinwait spinwait;
};

#endif // SYNC_PRIMITIVES_HPP
