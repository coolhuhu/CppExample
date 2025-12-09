#include <cassert>
#include <iostream>

#include "chat-message.hpp"

void TestChatMessage() {
  std::string str("my name is Jack.");
  ChatMessage msg;
  msg.SetMessageBody(str.data(), str.size());
  assert(msg.BodyLength() == str.size());
  std::cout << "message len: " << msg.BodyLength() << std::endl;
  std::string body_msg(msg.Body(), msg.Body() + msg.BodyLength());
  std::cout << body_msg << std::endl;
}

int main() { TestChatMessage(); }