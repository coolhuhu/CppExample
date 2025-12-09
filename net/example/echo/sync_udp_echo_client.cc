#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

using asio::ip::udp;

std::string make_daytime_string() {
  std::time_t now = std::time(0);
  return std::ctime(&now);
}

int main(int argc, char **argv) {
  std::string remote_address = "127.0.0.1";
  std::string port = "0";

  try {
    if (argc != 3) {
      std::cerr << "Usage: blocking_udp_echo_client <host> <port>\n";
      return 1;
    }
    remote_address = argv[1];
    port = argv[2];

    asio::io_context io_ctx;
    asio::error_code err_code;
    std::cout << "start sync_udp_echo_client..." << std::endl;

    udp::socket socket(io_ctx, udp::endpoint(udp::v4(), 0));
    udp::resolver resolver(io_ctx);
    auto remote_endpoint =
        *resolver.resolve(udp::v4(), remote_address, port).begin();

    for (int i = 0; i < 100; ++i) {
      std::string send_msg = make_daytime_string();
      send_msg += ": " + std::to_string(i);

      socket.send_to(asio::buffer(send_msg), remote_endpoint);

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 1;
}