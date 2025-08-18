#include <asio.hpp>
#include <iostream>

int main() {
  asio::io_context io;

  std::cout << "Create a timer and wait 5 seconds." << std::endl;
  asio::steady_timer timer(io, asio::chrono::seconds(5));
  timer.wait();

  std::cout << "Hello, world!" << std::endl;
  return 0;
}