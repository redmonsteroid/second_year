#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

class Fork {
public:
    mutex mtx; // Мьютекс для вилки
};

class Philosopher {
public:
    Philosopher(int id, Fork& leftFork, Fork& rightFork, mutex& printMutex) 
        : id(id), leftFork(leftFork), rightFork(rightFork), printMutex(printMutex) {}

    void dine() {
        while (true) {
            think();
            eat();
        }
    }

private:
    int id;
    Fork& leftFork;
    Fork& rightFork;
    mutex& printMutex;

    void think() {
        {
            lock_guard<mutex> lock(printMutex);
            cout << "Philosopher " << id << " is thinking." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    void eat() {
        // Философ пытается взять вилки
        lock(leftFork.mtx, rightFork.mtx); // Блокируем обе вилки
        lock_guard<mutex> lock1(leftFork.mtx, adopt_lock); // Завершаем захват левой вилки
        lock_guard<mutex> lock2(rightFork.mtx, adopt_lock); // Завершаем захват правой вилки

        // Начинаем есть
        {
            lock_guard<mutex> lock(printMutex);
            cout << "Philosopher " << id << " is eating." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
};

int main() {
    const int numPhilosophers = 5;
    vector<Fork> forks(numPhilosophers); // Вилки
    vector<thread> philosophers;
    mutex printMutex; // Мьютекс для синхронизации вывода в консоль

    for (int i = 0; i < numPhilosophers; ++i) {
        philosophers.push_back(thread(&Philosopher::dine, Philosopher(i, forks[i], forks[(i + 1) % numPhilosophers], printMutex)));
    }

    for (auto& t : philosophers) {
        t.join();
    }

    return 0;
}
