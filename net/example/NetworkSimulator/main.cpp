#include <csignal>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "config_manager.h"
#include "network_simulator.h"

std::unique_ptr<NetworkSimulator> simulator;
std::atomic<bool> running{true};

void signal_handler(int signal) {
  std::cout << "\nReceived signal " << signal << ", shutting down..."
            << std::endl;
  running = false;
  if (simulator) {
    simulator->stop();
  }
}

int main(int argc, char *argv[]) {
  try {
    std::cout << "UDP Network Simulator" << std::endl;
    std::cout << "=====================" << std::endl;

    NetworkConfig config = ConfigManager::load_from_args(argc, argv);

    if (config.enable_logging) {
      ConfigManager::print_config(config);
      std::cout << std::endl;
    }

    simulator = std::make_unique<NetworkSimulator>(config);

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    simulator->start();

    std::cout << "Network simulator is running. Press Ctrl+C to stop."
              << std::endl;

    while (running && simulator) {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      if (config.enable_statistics) {
        auto &stats = simulator->get_stats();
        std::cout << "\rStats: Sent:" << stats.packets_sent
                  << " Recv:" << stats.packets_received
                  << " Dropped:" << stats.packets_dropped
                  << " Delayed:" << stats.packets_delayed
                  << " Reordered:" << stats.packets_reordered
                  << " Avg Delay:" << std::fixed << std::setprecision(1)
                  << stats.average_delay_ms << "ms" << std::flush;
      }
    }

    std::cout << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}