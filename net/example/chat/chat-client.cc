#include <iostream>
#include <queue>
#include <string>

#include "asio.hpp"
#include "chat-message.hpp"

using ChatMessageQueue = std::queue<ChatMessage>;

using asio::ip::tcp;

std::string make_daytime_string() {
  std::time_t now = std::time(0);
  return std::ctime(&now);
}

class ChatClient {
 public:
  explicit ChatClient(asio::io_context &io_ctx, const std::string &ip,
                      const std::string &port)
      : io_ctx_(io_ctx),
        socket_(io_ctx, tcp::endpoint(tcp::v4(), 0)),
        resolver_(io_ctx_) {
    Connect(ip, port);
  }

  void Connect(const std::string &ip, const std::string &port) {
    auto server_endpoint = resolver_.resolve(ip, port);
    asio::async_connect(socket_, server_endpoint,
                        [this](asio::error_code ec, tcp::endpoint endpoint) {
                          if (!ec) {
                            std::string current_time = make_daytime_string();
                            std::cout << current_time;
                            std::cout << endpoint << " connect success!"
                                      << std::endl;

                            ReadMessageHeader();
                          }
                        });
  }

  void Close() { socket_.close(); }

  /// Not Thread Safe
  void WriteMessage(const char *data, std::size_t data_len) {
    if (data_len > ChatMessage::max_body_length) {
      std::cout << "[warning] input message len is " << data_len
                << ", and greater " << ChatMessage::max_body_length;
      data_len = ChatMessage::max_body_length;
    }

    ChatMessage msg(data, data_len);
    asio::post(io_ctx_, [this, msg] {
      bool is_writing_process = !write_msgs_.empty();
      write_msgs_.push(msg);
      // 有积压的消息没有发出
      if (!is_writing_process) {
        WriteMessage();
      }
    });
  }

 private:
  void ReadMessageHeader() {
    socket_.async_receive(
        asio::buffer(read_msg_.Data(), ChatMessage::header_length),
        [this](asio::error_code ec, std::size_t len) {
          if (!ec) {
            if (len != ChatMessage::header_length) {
              std::cerr << "Read Message Header Error!\n";
              std::cout << "Excepted read " << ChatMessage::header_length
                        << " bytes, but read " << len << " bytes.\n";
              Close();
              return;
            }

            if (read_msg_.DecodeHeader()) {
              ReadMessageBody();
            } else {
              Close();
              return;
            }

          } else {
            std::cerr << ec.message() << std::endl;
            Close();
            return;
          }
        });
  }

  void ReadMessageBody() {
    socket_.async_receive(
        asio::buffer(read_msg_.Body(), read_msg_.BodyLength()),
        [this](asio::error_code ec, size_t len) {
          if (!ec) {
            if (len != read_msg_.BodyLength()) {
              std::cerr << "Read Message Body Error!\n";
              std::cout << "Excepted read " << read_msg_.BodyLength()
                        << " bytes, but read " << len << " bytes.\n";
              Close();
              return;
            }
            std::cout.write(read_msg_.Body(), read_msg_.BodyLength());
            std::cout << "\n";
            ReadMessageHeader();
          } else {
            std::cerr << ec.message() << std::endl;
            Close();
            return;
          }
        });
  }

  void WriteMessage() {
    socket_.async_send(asio::buffer(write_msgs_.front().Body(),
                                    write_msgs_.front().BodyLength()),
                       [this](asio::error_code ec, std::size_t len) {
                         if (!ec) {
                           if (len != write_msgs_.front().BodyLength()) {
                             std::cerr << "send msg error.\n";
                             Close();
                             return;
                           }
                           write_msgs_.pop();
                           if (!write_msgs_.empty()) {
                             WriteMessage();
                           }
                         } else {
                           std::cerr << ec.message() << "\n";
                           Close();
                         }
                       });
  }

 private:
  asio::io_context &io_ctx_;
  tcp::socket socket_;
  tcp::resolver resolver_;
  ChatMessage read_msg_;
  ChatMessageQueue write_msgs_;
};

int main(int argc, char **argv) {}