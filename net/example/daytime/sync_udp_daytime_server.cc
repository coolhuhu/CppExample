#include <array>
#include <asio.hpp>
#include <iostream>

using asio::ip::udp;

std::string make_daytime_string() {
  std::time_t now = std::time(0);
  return std::ctime(&now);
}

int main(int argc, char **argv) {
  try {
    asio::io_context io_context;

    udp::socket socket(io_context, udp::endpoint(udp::v4(), 13));
    auto local_endpoint = socket.local_endpoint();
    auto local_address = local_endpoint.address();
    std::cout << (local_address.is_v4() ? "ipv4" : "ipv6") << std::endl;
    std::cout << "local address: " << local_address.to_string() << std::endl;
    std::cout << "local port: " << local_endpoint.port() << std::endl;

    for (;;) {
      std::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      socket.receive_from(asio::buffer(recv_buf), remote_endpoint);
      std::cout << "new upd message received" << std::endl;

      std::string message = make_daytime_string();

      std::error_code ignore_error;
      socket.send_to(asio::buffer(message), remote_endpoint, 0, ignore_error);
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}