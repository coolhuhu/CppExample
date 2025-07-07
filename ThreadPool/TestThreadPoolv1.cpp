#include <cmath>
#include <iostream>

#include "threadpool_v1.h"

int task1(int a, int b) { return a + b; }

void test_threadpool() {
  ThreadPool threadpool(2);
  auto task1_ret = threadpool.commit_task(task1, 2, 9);
  auto task2_ret = threadpool.commit_task(
      [](int a, int b) -> int { return std::pow(a, b); }, 2, 9);

  auto lambda_func = [](int a) -> int { return std::sqrt(a); };
  auto task3_ret = threadpool.commit_task(lambda_func, 9);

  std::cout << task1_ret.get() << std::endl;
  std::cout << task2_ret.get() << std::endl;
  std::cout << task3_ret.get() << std::endl;
}

int main() { test_threadpool(); }