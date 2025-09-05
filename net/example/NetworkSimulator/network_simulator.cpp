#include "network_simulator.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>

NetworkSimulator::NetworkSimulator(const NetworkConfig& config)
    : config_(config),
      socket_(io_context_),
      rng_(std::random_device{}()),
      dist_(0.0, 1.0) {
    
    try {
        udp::resolver resolver(io_context_);
        auto endpoints = resolver.resolve(udp::v4(), config_.target_host, std::to_string(config_.target_port));
        target_endpoint_ = *endpoints.begin();
        
        socket_.open(udp::v4());
        socket_.bind(udp::endpoint(asio::ip::make_address(config_.listen_host), config_.listen_port));
        
        std::cout << "UDP Network Simulator started" << std::endl;
        std::cout << "Listening on: " << config_.listen_host << ":" << config_.listen_port << std::endl;
        std::cout << "Forwarding to: " << config_.target_host << ":" << config_.target_port << std::endl;
        
        if (config_.enable_logging) {
            std::cout << "Packet loss rate: " << (config_.packet_loss_rate * 100) << "%" << std::endl;
            std::cout << "Delay rate: " << (config_.delay_rate * 100) << "%" << std::endl;
            std::cout << "Jitter rate: " << (config_.jitter_rate * 100) << "%" << std::endl;
            std::cout << "Reordering rate: " << (config_.reordering_rate * 100) << "%" << std::endl;
            std::cout << "Base delay: " << config_.base_delay.count() << "ms" << std::endl;
            std::cout << "Max jitter: " << config_.max_jitter.count() << "ms" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error initializing NetworkSimulator: " << e.what() << std::endl;
        throw;
    }
}

NetworkSimulator::~NetworkSimulator() {
    stop();
}

void NetworkSimulator::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    start_receive();
    
    io_thread_ = std::thread([this]() {
        io_context_.run();
    });
    
    start_packet_processor();
}

void NetworkSimulator::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    if (socket_.is_open()) {
        socket_.close();
    }
    
    io_context_.stop();
    
    if (io_thread_.joinable()) {
        io_thread_.join();
    }
    
    packet_queue_cv_.notify_all();
    if (processor_thread_.joinable()) {
        processor_thread_.join();
    }
    
    if (config_.enable_statistics) {
        print_statistics();
    }
}

void NetworkSimulator::start_receive() {
    if (!running_) {
        return;
    }
    
    socket_.async_receive_from(
        asio::buffer(receive_buffer_),
        remote_endpoint_,
        [this](const asio::error_code& error, size_t bytes_received) {
            handle_receive(error, bytes_received);
        });
}

void NetworkSimulator::handle_receive(const asio::error_code& error, size_t bytes_received) {
    if (!running_) {
        return;
    }
    
    if (!error && bytes_received > 0) {
        PacketInfo packet;
        packet.data.resize(bytes_received);
        std::copy(receive_buffer_.begin(), receive_buffer_.begin() + bytes_received, packet.data.begin());
        packet.source = remote_endpoint_;
        packet.destination = target_endpoint_;
        packet.received_time = std::chrono::steady_clock::now();
        packet.sequence_number = sequence_counter_++;
        
        {
            std::lock_guard<std::mutex> lock(packet_queue_mutex_);
            packet_queue_.push_back(packet);
        }
        
        packet_queue_cv_.notify_one();
        
        if (config_.enable_logging) {
            log_packet(packet, "RECEIVED");
        }
    }
    
    start_receive();
}

void NetworkSimulator::process_packet(PacketInfo packet) {
    stats_.packets_received++;
    stats_.total_bytes_received += packet.data.size();
    
    if (should_drop_packet()) {
        stats_.packets_dropped++;
        if (config_.enable_logging) {
            log_packet(packet, "DROPPED");
        }
        return;
    }
    
    if (should_delay_packet()) {
        stats_.packets_delayed++;
        auto delay = calculate_delay();
        packet.send_time = std::chrono::steady_clock::now() + delay;
        
        {
            std::lock_guard<std::mutex> lock(delayed_packets_mutex_);
            delayed_packets_[packet.sequence_number] = packet;
        }
        
        if (config_.enable_logging) {
            std::cout << "[DELAY] Packet " << packet.sequence_number 
                      << " delayed by " << delay.count() << "ms" << std::endl;
        }
        return;
    }
    
    send_packet(packet);
}

void NetworkSimulator::send_packet(PacketInfo packet) {
    auto now = std::chrono::steady_clock::now();
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - packet.received_time);
    
    if (should_reorder_packet()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        stats_.packets_reordered++;
        if (config_.enable_logging) {
            std::cout << "[REORDER] Packet " << packet.sequence_number << " reordered" << std::endl;
        }
    }
    
    socket_.async_send_to(
        asio::buffer(packet.data),
        packet.destination,
        [this, packet, delay](const asio::error_code& error, size_t bytes_sent) {
            if (!error) {
                stats_.packets_sent++;
                stats_.total_bytes_sent += bytes_sent;
                
                {
                    std::lock_guard<std::mutex> lock(stats_mutex_);
                    double current_delay = delay.count();
                    double current_avg = stats_.average_delay_ms;
                    double current_max = stats_.max_delay_ms;
                    double current_min = stats_.min_delay_ms;
                    
                    stats_.average_delay_ms = (current_avg * (stats_.packets_sent - 1) + current_delay) / stats_.packets_sent;
                    stats_.max_delay_ms = (current_max < current_delay) ? current_delay : current_max;
                    stats_.min_delay_ms = (stats_.packets_sent == 1) ? current_delay : 
                                          ((current_min > current_delay) ? current_delay : current_min);
                }
                
                if (config_.enable_logging) {
                    log_packet(packet, "FORWARDED (" + std::to_string(delay.count()) + "ms)");
                }
            } else {
                std::cerr << "Error sending packet: " << error.message() << std::endl;
            }
        });
}

bool NetworkSimulator::should_drop_packet() {
    return dist_(rng_) < config_.packet_loss_rate;
}

bool NetworkSimulator::should_delay_packet() {
    return dist_(rng_) < config_.delay_rate;
}

bool NetworkSimulator::should_reorder_packet() {
    return dist_(rng_) < config_.reordering_rate;
}

std::chrono::milliseconds NetworkSimulator::calculate_delay() {
    auto delay = config_.base_delay;
    
    if (dist_(rng_) < config_.jitter_rate) {
        std::uniform_int_distribution<int> jitter_dist(0, config_.max_jitter.count());
        delay += std::chrono::milliseconds(jitter_dist(rng_));
    }
    
    return delay;
}

void NetworkSimulator::start_packet_processor() {
    processor_thread_ = std::thread([this]() {
        packet_processor_loop();
    });
}

void NetworkSimulator::packet_processor_loop() {
    while (running_) {
        std::unique_lock<std::mutex> lock(packet_queue_mutex_);
        packet_queue_cv_.wait(lock, [this]() {
            return !packet_queue_.empty() || !running_;
        });
        
        if (!running_) {
            break;
        }
        
        if (!packet_queue_.empty()) {
            auto packet = packet_queue_.front();
            packet_queue_.erase(packet_queue_.begin());
            lock.unlock();
            
            process_packet(packet);
        }
        
        process_delayed_packets();
    }
}

void NetworkSimulator::process_delayed_packets() {
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(delayed_packets_mutex_);
    auto it = delayed_packets_.begin();
    while (it != delayed_packets_.end()) {
        if (now >= it->second.send_time) {
            send_packet(it->second);
            it = delayed_packets_.erase(it);
        } else {
            ++it;
        }
    }
}

void NetworkSimulator::log_packet(const PacketInfo& packet, const std::string& action) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%H:%M:%S") 
              << "." << std::setfill('0') << std::setw(3) << ms.count() << "] "
              << "[Packet #" << packet.sequence_number << "] "
              << action << " (" << packet.data.size() << " bytes) "
              << packet.source.address().to_string() << ":" << packet.source.port()
              << " -> " << packet.destination.address().to_string() << ":" << packet.destination.port()
              << std::endl;
}

void NetworkSimulator::update_statistics(const PacketInfo& packet) {
    // Statistics are updated in the send_packet method
}

void NetworkSimulator::update_config(const NetworkConfig& config) {
    config_ = config;
}

void NetworkSimulator::print_statistics() {
    std::cout << "\n=== Network Statistics ===" << std::endl;
    std::cout << "Packets received: " << stats_.packets_received << std::endl;
    std::cout << "Packets sent: " << stats_.packets_sent << std::endl;
    std::cout << "Packets dropped: " << stats_.packets_dropped << std::endl;
    std::cout << "Packets delayed: " << stats_.packets_delayed << std::endl;
    std::cout << "Packets reordered: " << stats_.packets_reordered << std::endl;
    
    if (stats_.packets_received > 0) {
        double loss_rate = (double)stats_.packets_dropped / stats_.packets_received * 100;
        std::cout << "Actual loss rate: " << std::fixed << std::setprecision(2) << loss_rate << "%" << std::endl;
    }
    
    std::cout << "Total bytes received: " << stats_.total_bytes_received << std::endl;
    std::cout << "Total bytes sent: " << stats_.total_bytes_sent << std::endl;
    
    if (stats_.packets_sent > 0) {
        std::cout << "Average delay: " << std::fixed << std::setprecision(2) << stats_.average_delay_ms << "ms" << std::endl;
        std::cout << "Min delay: " << std::fixed << std::setprecision(2) << stats_.min_delay_ms << "ms" << std::endl;
        std::cout << "Max delay: " << std::fixed << std::setprecision(2) << stats_.max_delay_ms << "ms" << std::endl;
    }
    std::cout << "=========================" << std::endl;
}