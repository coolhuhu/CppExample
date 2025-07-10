#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::atomic_flag lock = ATOMIC_FLAG_INIT;

/*
    示例来源：https://en.cppreference.com/w/cpp/atomic/atomic_flag
*/
void f(int n) {
  for (int cnt = 0; cnt < 40; ++cnt) {
    while (lock.test_and_set(std::memory_order_acquire))  // acquire lock
    {
      // Since C++20, it is possible to update atomic_flag's
      // value only when there is a chance to acquire the lock.
      // See also: https://stackoverflow.com/questions/62318642
#if defined(__cpp_lib_atomic_flag_test)
      while (lock.test(std::memory_order_relaxed))  // test lock
#endif
        ;  // spin
    }
    static int out{};
    std::cout << n << ((++out % 40) == 0 ? '\n' : ' ');
    lock.clear(std::memory_order_release);  // release lock
  }
}

void Test_f() {
  std::vector<std::thread> v;
  for (int n = 0; n < 10; ++n) v.emplace_back(f, n);
  for (auto &t : v) t.join();
}

/**
 * SpinLock should be noncopyable.
 */
class SpinLock {
 public:
  SpinLock() : flag_(ATOMIC_FLAG_INIT) {};

  void lock() {
    while (flag_.test_and_set(std::memory_order_acquire)) {
    }
  }

  void unlock() { flag_.clear(std::memory_order_release); }

 private:
  std::atomic_flag flag_;
};

void TestSpinLock() {
  SpinLock spinlock;
  std::thread t1([&spinlock]() {
    {
      std::lock_guard<SpinLock> locker(spinlock);
      for (int i = 0; i < 3; i++) {
        std::cout << "*";
      }
      std::cout << std::endl;
    }
  });

  std::thread t2([&spinlock]() {
    {
      std::lock_guard<SpinLock> locker(spinlock);
      for (int i = 0; i < 3; i++) {
        std::cout << "?";
      }
      std::cout << std::endl;
    }
  });

  t1.join();
  t2.join();
}

int main() {
  // Test_f();
  TestSpinLock();
}