#include <asio.hpp>
#include <iostream>
#include <memory>

using asio::ip::tcp;

class EchoClient {
 public:
  EchoClient(asio::io_context &io, const std::string &host,
             const std::string &port)
      : io_(io), socket_(io), resolver_(io) {
    Connect(host, port);
  }

  void Connect(const std::string &host, const std::string &port) {
    auto endpoints = resolver_.resolve(host, port);
    asio::connect(socket_, endpoints);
    asio::async_connect(
        socket_, endpoints, [this](std::error_code ec, tcp::endpoint endpoint) {
          std::cout << endpoint << " connect success!" << std::endl;
          Start();
        });
  }

  void Start() {
    // Start reading user input and sending messages
    Write();
  }

 private:
  void Write() {
    std::cout << "Enter message: ";
    std::getline(std::cin, send_buffer_);

    asio::async_write(
        socket_, asio::buffer(send_buffer_),
        [this](std::error_code ec, std::size_t /*bytes_transferred*/) {
          if (!ec) {
            Read();
          } else {
            std::cerr << "Write error: " << ec.message() << std::endl;
            socket_.close();
          }
        });
  }

  void Read() {
    recv_buffer_.resize(1024);
    socket_.async_read_some(
        asio::buffer(recv_buffer_),
        [this](std::error_code ec, std::size_t length) {
          if (!ec) {
            std::cout << "Received: "
                      << std::string_view(recv_buffer_.data(), length)
                      << std::endl;
            Write();
          } else {
            std::cerr << "Read error: " << ec.message() << std::endl;
            socket_.close();
          }
        });
  }

  asio::io_context &io_;
  tcp::socket socket_;
  tcp::resolver resolver_;
  std::string send_buffer_;
  std::vector<char> recv_buffer_;
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
    return 1;
  }

  try {
    asio::io_context io;
    EchoClient client(io, argv[1], argv[2]);
    io.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}