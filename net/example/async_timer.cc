#include <asio.hpp>
#include <iostream>
#include <thread>

void print(const std::error_code & /* e */) {
  auto current_thread_id = std::this_thread::get_id();
  std::cout << "function print called from thread " << current_thread_id
            << std::endl;
  std::cout << "Hello, world!" << std::endl;
}

int main() {
  std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
  asio::io_context io;
  asio::steady_timer t(io, asio::chrono::seconds(5));

  t.async_wait(&print);

  io.run();

  return 0;
}