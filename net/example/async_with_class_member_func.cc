#include <asio.hpp>
#include <functional>
#include <iostream>

class Printer {
 public:
  Printer(asio::io_context &io)
      : timer_(io, asio::chrono::seconds(1)), count_(0) {
    timer_.async_wait(std::bind(&Printer::Print, this));
  };

  ~Printer() { std::cout << "Final count is " << count_ << std::endl; }

  void Print() {
    if (count_ < 5) {
      std::cout << count_ << std::endl;
      ++count_;
      timer_.expires_at(timer_.expiry() + asio::chrono::seconds(1));
      timer_.async_wait(std::bind(&Printer::Print, this));
    }
  }

 private:
  asio::steady_timer timer_;
  int count_;
};

int main() {
  asio::io_context io;
  Printer printer(io);
  io.run();
}