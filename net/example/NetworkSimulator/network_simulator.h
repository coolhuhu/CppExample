#pragma once

#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

using asio::ip::udp;

struct NetworkConfig {
  double packet_loss_rate = 0.0;  // 丢包率 (0.0-1.0)
  double delay_rate = 0.0;        // 延迟率 (0.0-1.0)
  double jitter_rate = 0.0;       // 抖动率 (0.0-1.0)
  double reordering_rate = 0.0;   // 乱序率 (0.0-1.0)

  std::chrono::milliseconds base_delay{0};  // 基础延迟
  std::chrono::milliseconds max_jitter{0};  // 最大抖动

  std::string listen_host = "0.0.0.0";
  uint16_t listen_port = 8080;
  std::string target_host = "127.0.0.1";
  uint16_t target_port = 8081;

  bool enable_logging = true;
  bool enable_statistics = true;
};

struct PacketInfo {
  std::vector<uint8_t> data;
  udp::endpoint source;
  udp::endpoint destination;
  std::chrono::steady_clock::time_point received_time;
  uint32_t sequence_number;
  std::chrono::steady_clock::time_point send_time;
};

struct NetworkStats {
  std::atomic<uint64_t> packets_sent{0};
  std::atomic<uint64_t> packets_received{0};
  std::atomic<uint64_t> packets_dropped{0};
  std::atomic<uint64_t> packets_delayed{0};
  std::atomic<uint64_t> packets_reordered{0};
  std::atomic<uint64_t> total_bytes_sent{0};
  std::atomic<uint64_t> total_bytes_received{0};

  std::atomic<double> average_delay_ms{0.0};
  std::atomic<double> max_delay_ms{0.0};
  std::atomic<double> min_delay_ms{0.0};
};

class NetworkSimulator {
 public:
  explicit NetworkSimulator(const NetworkConfig &config);
  ~NetworkSimulator();

  void start();
  void stop();

  const NetworkStats &get_stats() const { return stats_; }
  void reset_stats() { 
    stats_.packets_sent = 0;
    stats_.packets_received = 0;
    stats_.packets_dropped = 0;
    stats_.packets_delayed = 0;
    stats_.packets_reordered = 0;
    stats_.total_bytes_sent = 0;
    stats_.total_bytes_received = 0;
    stats_.average_delay_ms = 0.0;
    stats_.max_delay_ms = 0.0;
    stats_.min_delay_ms = 0.0;
  }

  void update_config(const NetworkConfig &config);
  const NetworkConfig &get_config() const { return config_; }

 private:
  void start_receive();
  void handle_receive(const asio::error_code &error, size_t bytes_received);
  void handle_send(const asio::error_code &error, size_t bytes_sent);

  void process_packet(PacketInfo packet);
  void send_packet(PacketInfo packet);

  bool should_drop_packet();
  bool should_delay_packet();
  bool should_reorder_packet();
  std::chrono::milliseconds calculate_delay();

  void start_packet_processor();
  void packet_processor_loop();

  void log_packet(const PacketInfo &packet, const std::string &action);
  void update_statistics(const PacketInfo &packet);

  NetworkConfig config_;
  NetworkStats stats_;

  asio::io_context io_context_;
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  udp::endpoint target_endpoint_;

  std::array<uint8_t, 65535> receive_buffer_;

  std::thread io_thread_;
  std::thread processor_thread_;
  std::atomic<bool> running_{false};

  std::mt19937 rng_;
  std::uniform_real_distribution<double> dist_;

  std::mutex packet_queue_mutex_;
  std::condition_variable packet_queue_cv_;
  std::vector<PacketInfo> packet_queue_;

  std::atomic<uint32_t> sequence_counter_{0};

  std::mutex delayed_packets_mutex_;
  std::unordered_map<uint32_t, PacketInfo> delayed_packets_;

  std::mutex stats_mutex_;

  void process_delayed_packets();
  void print_statistics();
};