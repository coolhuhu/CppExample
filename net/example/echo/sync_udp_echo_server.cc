#include <array>
#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::udp;

std::string make_daytime_string() {
  std::time_t now = std::time(0);
  return std::ctime(&now);
}

int main(int argc, char *argv[]) {
  try {
    asio::io_context io_ctx;

    udp::socket socket(io_ctx, udp::endpoint(udp::v4(), 6868));
    auto local_endpoint = socket.local_endpoint();
    auto local_address = local_endpoint.address();
    std::cout << "local address: " << local_address.to_string() << std::endl;
    std::cout << "local port: " << local_endpoint.port() << std::endl;

    for (;;) {
      std::array<char, 256> recv_buf;
      udp::endpoint remote_endpoint;
      size_t len = socket.receive_from(asio::buffer(recv_buf), remote_endpoint);
      std::string remote_address = remote_endpoint.address().to_string();
      int remote_port = remote_endpoint.port();
      std::cout << "Reveived from " << remote_address << " " << remote_port
                << ", received msg: " << std::string(recv_buf.data(), len)
                << std::endl;

      std::string send_msg("I receive a message");
      socket.send_to(asio::buffer(send_msg), remote_endpoint);
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 1;
}