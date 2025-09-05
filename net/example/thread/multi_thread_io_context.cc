#include <asio.hpp>
#include <iostream>
#include <thread>

void worker(asio::io_context &io_ctx) { io_ctx.run(); }

void working_func(int i) {
  std::cout << "working_func " << i << std::endl;
  std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
}

int main() {
  asio::io_context io_ctx;

  std::vector<std::thread> threads;

  for (int i = 0; i < 10; ++i) {
    asio::post(io_ctx, [i]() { working_func(i); });
  }

  for (int i = 0; i < 4; ++i) {
    threads.emplace_back(worker, std::ref(io_ctx));
  }

  for (auto &t : threads) {
    t.join();
  }
}