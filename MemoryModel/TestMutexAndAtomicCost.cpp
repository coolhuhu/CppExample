/**
 * 比较 atomic 和 mutex 性能开销
*/

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>


void TestMutexCost()
{
    std::mutex mtx;

    int cnt = 0;
    auto f = [&]() {
        for (int i = 0; i < 10000000; ++i) {
            std::unique_lock<std::mutex> locker(mtx);
            ++cnt;
        }
    };

    std::vector<std::thread> pool;

    auto start_time = std::chrono::system_clock::now();

    for (int i = 0; i < 4; ++i) {
        pool.emplace_back(f);
    }
    for (auto& t : pool) {
        if (t.joinable()) {
            t.join();
        }
    }

    auto total_time = std::chrono::system_clock::now() - start_time;
    std::cout << "TestMutexCost: " << total_time.count() << std::endl;
}


void TestAtomicCost()
{
    std::atomic<int> cnt = 0;
    auto f = [&]() {
        for (int i = 0; i < 10000000; ++i) {
            ++cnt;
        }
    };

    std::vector<std::thread> pool;

    auto start_time = std::chrono::system_clock::now();

    for (int i = 0; i < 4; ++i) {
        pool.emplace_back(f);
    }
    for (auto& t : pool) {
        if (t.joinable()) {
            t.join();
        }
    }

    auto total_time = std::chrono::system_clock::now() - start_time;
    std::cout << "TestAtomicCost: " << total_time.count() << std::endl;
}


int main()
{
    TestAtomicCost();
    TestMutexCost();
}