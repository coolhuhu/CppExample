#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

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

    std::cout << "input msg: " << std::endl;
    // 3. 从控制台接收输入的数据
    std::string msg;
    while (std::getline(std::cin, msg)) {
      // 4. 发送数据
      std::string current_time = make_daytime_string();
      std::string send_msg = current_time + ": " + msg;
      socket.send_to(asio::buffer(send_msg), remote_endpoint);

      // 5. 接收数据
      std::array<char, 256> recv_buf;
      udp::endpoint sender_endpoint;
      size_t len = socket.receive_from(asio::buffer(recv_buf), sender_endpoint);

      int remote_port = sender_endpoint.port();
      std::cout << "Received from " << remote_address << " " << remote_port
                << ", reveived msg: " << std::string(recv_buf.data(), len)
                << std::endl;
    }

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 1;
}