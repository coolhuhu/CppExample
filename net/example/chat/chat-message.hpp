#include <array>
#include <cstdlib>
#include <cstring>

/**
 * 由消息头和消息体组成，消息体的最大长度为 512 个字节；
 * 消息头占 4 个字节，用来表示实际的消息体长度
 */
class ChatMessage {
 public:
  static constexpr std::size_t header_length = 4;
  static constexpr std::size_t max_body_length = 512;

  ChatMessage() : body_length_(0) {}

  ChatMessage(const char *msg, std::size_t msg_len) : body_length_(msg_len) {
    SetMessageBody(msg, msg_len);
  }

  char *Data() { return data_.data(); }

  const char *Data() const { return data_.data(); }

  char *Body() { return data_.data() + header_length; }

  const char *Body() const { return data_.data() + header_length; }

  std::size_t Length() const { return header_length + body_length_; }

  std::size_t BodyLength() const { return body_length_; }

  void EncodeHeadr() {
    std::memcpy(data_.data(), &body_length_, header_length);
  }

  bool DecodeHeader() {
    char header[header_length + 1] = "";
    std::copy(data_.data(), data_.data() + header_length, header);
    body_length_ = std::atoi(header);
    if (body_length_ > max_body_length) {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void SetBodyLength(std::size_t new_length) {
    body_length_ = new_length;
    if (body_length_ > max_body_length) {
      body_length_ = max_body_length;
    }
  }

  void SetMessageBody(const char *msg, std::size_t msg_len) {
    SetBodyLength(msg_len);
    msg_len = BodyLength();
    std::copy(msg, msg + msg_len, Body());
  }

 private:
  std::array<char, header_length + max_body_length> data_;
  std::size_t body_length_;
};