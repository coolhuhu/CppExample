#include <asio.hpp>
#include <functional>
#include <iostream>
#include <thread>

class Printer {
 public:
  Printer(asio::io_context &io)
      : strand_(asio::make_strand(io)),
        timer1_(io, asio::chrono::seconds(1)),
        timer2_(io, asio::chrono::seconds(1)),
        count_(0) {
    timer1_.async_wait(
        asio::bind_executor(strand_, std::bind(&Printer::Print1, this)));

    timer2_.async_wait(
        asio::bind_executor(strand_, std::bind(&Printer::Print2, this)));
  }

  ~Printer() { std::cout << "Final count is " << count_ << "\n"; }

  void Print1() {
    if (count_ < 10) {
      std::cout << "thread id: " << std::this_thread::get_id()
                << " Timer 1: " << count_ << "\n";
      ++count_;
      timer1_.expires_at(timer1_.expiry() + asio::chrono::seconds(1));

      timer1_.async_wait(
          asio::bind_executor(strand_, std::bind(&Printer::Print1, this)));
    }
  }

  void Print2() {
    if (count_ < 10) {
      std::cout << "thread id: " << std::this_thread::get_id()
                << " Timer 2: " << count_ << "\n";
      ++count_;
      timer2_.expires_at(timer2_.expiry() + asio::chrono::seconds(1));

      timer2_.async_wait(
          asio::bind_executor(strand_, std::bind(&Printer::Print2, this)));
    }
  }

 private:
  asio::strand<asio::io_context::executor_type> strand_;
  asio::steady_timer timer1_;
  asio::steady_timer timer2_;
  int count_;
};

int main() {
  asio::io_context io;
  Printer p(io);
  std::thread t([&] { io.run(); });
  io.run();
  t.join();

  return 0;
}
