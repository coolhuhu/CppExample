#include "config_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

NetworkConfig ConfigManager::load_from_file(const std::string& filename) {
    NetworkConfig config;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file " << filename << ", using defaults." << std::endl;
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t"));
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        NetworkConfig line_config = parse_config_line(line);
        if (line_config.packet_loss_rate >= 0) {
            config.packet_loss_rate = line_config.packet_loss_rate;
        }
        if (line_config.delay_rate >= 0) {
            config.delay_rate = line_config.delay_rate;
        }
        if (line_config.jitter_rate >= 0) {
            config.jitter_rate = line_config.jitter_rate;
        }
        if (line_config.reordering_rate >= 0) {
            config.reordering_rate = line_config.reordering_rate;
        }
        if (line_config.base_delay.count() >= 0) {
            config.base_delay = line_config.base_delay;
        }
        if (line_config.max_jitter.count() >= 0) {
            config.max_jitter = line_config.max_jitter;
        }
        if (!line_config.listen_host.empty()) {
            config.listen_host = line_config.listen_host;
        }
        if (line_config.listen_port > 0) {
            config.listen_port = line_config.listen_port;
        }
        if (!line_config.target_host.empty()) {
            config.target_host = line_config.target_host;
        }
        if (line_config.target_port > 0) {
            config.target_port = line_config.target_port;
        }
    }
    
    return config;
}

NetworkConfig ConfigManager::load_from_args(int argc, char* argv[]) {
    NetworkConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            print_help();
            exit(0);
        }
        else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config = load_from_file(argv[++i]);
            }
        }
        else if (arg == "--listen-host") {
            if (i + 1 < argc) {
                config.listen_host = argv[++i];
            }
        }
        else if (arg == "--listen-port") {
            if (i + 1 < argc) {
                config.listen_port = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--target-host") {
            if (i + 1 < argc) {
                config.target_host = argv[++i];
            }
        }
        else if (arg == "--target-port") {
            if (i + 1 < argc) {
                config.target_port = std::stoi(argv[++i]);
            }
        }
        else if (arg == "--packet-loss") {
            if (i + 1 < argc) {
                config.packet_loss_rate = parse_percentage(argv[++i]);
            }
        }
        else if (arg == "--delay-rate") {
            if (i + 1 < argc) {
                config.delay_rate = parse_percentage(argv[++i]);
            }
        }
        else if (arg == "--jitter-rate") {
            if (i + 1 < argc) {
                config.jitter_rate = parse_percentage(argv[++i]);
            }
        }
        else if (arg == "--reorder-rate") {
            if (i + 1 < argc) {
                config.reordering_rate = parse_percentage(argv[++i]);
            }
        }
        else if (arg == "--base-delay") {
            if (i + 1 < argc) {
                config.base_delay = std::chrono::milliseconds(parse_milliseconds(argv[++i]));
            }
        }
        else if (arg == "--max-jitter") {
            if (i + 1 < argc) {
                config.max_jitter = std::chrono::milliseconds(parse_milliseconds(argv[++i]));
            }
        }
        else if (arg == "--no-log") {
            config.enable_logging = false;
        }
        else if (arg == "--no-stats") {
            config.enable_statistics = false;
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            print_help();
            exit(1);
        }
    }
    
    return config;
}

void ConfigManager::save_to_file(const NetworkConfig& config, const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not save config to " << filename << std::endl;
        return;
    }
    
    file << "# UDP Network Simulator Configuration" << std::endl;
    file << "# This file was automatically generated" << std::endl;
    file << std::endl;
    file << "# Network Settings" << std::endl;
    file << "listen_host=" << config.listen_host << std::endl;
    file << "listen_port=" << config.listen_port << std::endl;
    file << "target_host=" << config.target_host << std::endl;
    file << "target_port=" << config.target_port << std::endl;
    file << std::endl;
    file << "# Network Simulation Parameters" << std::endl;
    file << "packet_loss_rate=" << (config.packet_loss_rate * 100) << "%" << std::endl;
    file << "delay_rate=" << (config.delay_rate * 100) << "%" << std::endl;
    file << "jitter_rate=" << (config.jitter_rate * 100) << "%" << std::endl;
    file << "reorder_rate=" << (config.reordering_rate * 100) << "%" << std::endl;
    file << "base_delay=" << config.base_delay.count() << "ms" << std::endl;
    file << "max_jitter=" << config.max_jitter.count() << "ms" << std::endl;
    file << std::endl;
    file << "# Features" << std::endl;
    file << "enable_logging=" << (config.enable_logging ? "true" : "false") << std::endl;
    file << "enable_statistics=" << (config.enable_statistics ? "true" : "false") << std::endl;
    
    file.close();
}

void ConfigManager::print_help() {
    std::cout << "UDP Network Simulator - Usage:" << std::endl;
    std::cout << std::endl;
    std::cout << "Command Line Arguments:" << std::endl;
    std::cout << "  -h, --help                 Show this help message" << std::endl;
    std::cout << "  -c, --config <file>        Load configuration from file" << std::endl;
    std::cout << "  --listen-host <host>       Listen host (default: 0.0.0.0)" << std::endl;
    std::cout << "  --listen-port <port>       Listen port (default: 8080)" << std::endl;
    std::cout << "  --target-host <host>       Target host (default: 127.0.0.1)" << std::endl;
    std::cout << "  --target-port <port>       Target port (default: 8081)" << std::endl;
    std::cout << "  --packet-loss <rate>       Packet loss rate (0-100%)" << std::endl;
    std::cout << "  --delay-rate <rate>        Delay rate (0-100%)" << std::endl;
    std::cout << "  --jitter-rate <rate>       Jitter rate (0-100%)" << std::endl;
    std::cout << "  --reorder-rate <rate>      Reordering rate (0-100%)" << std::endl;
    std::cout << "  --base-delay <ms>          Base delay in milliseconds" << std::endl;
    std::cout << "  --max-jitter <ms>          Maximum jitter in milliseconds" << std::endl;
    std::cout << "  --no-log                   Disable logging" << std::endl;
    std::cout << "  --no-stats                 Disable statistics" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration File Format:" << std::endl;
    std::cout << "  # Comments start with #" << std::endl;
    std::cout << "  listen_host=0.0.0.0" << std::endl;
    std::cout << "  listen_port=8080" << std::endl;
    std::cout << "  target_host=127.0.0.1" << std::endl;
    std::cout << "  target_port=8081" << std::endl;
    std::cout << "  packet_loss_rate=5%" << std::endl;
    std::cout << "  delay_rate=10%" << std::endl;
    std::cout << "  jitter_rate=15%" << std::endl;
    std::cout << "  reorder_rate=5%" << std::endl;
    std::cout << "  base_delay=50ms" << std::endl;
    std::cout << "  max_jitter=100ms" << std::endl;
    std::cout << "  enable_logging=true" << std::endl;
    std::cout << "  enable_statistics=true" << std::endl;
}

void ConfigManager::print_config(const NetworkConfig& config) {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << "  Listen: " << config.listen_host << ":" << config.listen_port << std::endl;
    std::cout << "  Target: " << config.target_host << ":" << config.target_port << std::endl;
    std::cout << "  Packet Loss: " << (config.packet_loss_rate * 100) << "%" << std::endl;
    std::cout << "  Delay Rate: " << (config.delay_rate * 100) << "%" << std::endl;
    std::cout << "  Jitter Rate: " << (config.jitter_rate * 100) << "%" << std::endl;
    std::cout << "  Reorder Rate: " << (config.reordering_rate * 100) << "%" << std::endl;
    std::cout << "  Base Delay: " << config.base_delay.count() << "ms" << std::endl;
    std::cout << "  Max Jitter: " << config.max_jitter.count() << "ms" << std::endl;
    std::cout << "  Logging: " << (config.enable_logging ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Statistics: " << (config.enable_statistics ? "Enabled" : "Disabled") << std::endl;
}

NetworkConfig ConfigManager::parse_config_line(const std::string& line) {
    NetworkConfig config;
    auto parts = split_string(line, '=');
    
    if (parts.size() != 2) {
        return config;
    }
    
    std::string key = parts[0];
    std::string value = parts[1];
    
    if (key == "listen_host") {
        config.listen_host = value;
    }
    else if (key == "listen_port") {
        config.listen_port = std::stoi(value);
    }
    else if (key == "target_host") {
        config.target_host = value;
    }
    else if (key == "target_port") {
        config.target_port = std::stoi(value);
    }
    else if (key == "packet_loss_rate") {
        config.packet_loss_rate = parse_percentage(value);
    }
    else if (key == "delay_rate") {
        config.delay_rate = parse_percentage(value);
    }
    else if (key == "jitter_rate") {
        config.jitter_rate = parse_percentage(value);
    }
    else if (key == "reorder_rate") {
        config.reordering_rate = parse_percentage(value);
    }
    else if (key == "base_delay") {
        config.base_delay = std::chrono::milliseconds(parse_milliseconds(value));
    }
    else if (key == "max_jitter") {
        config.max_jitter = std::chrono::milliseconds(parse_milliseconds(value));
    }
    else if (key == "enable_logging") {
        config.enable_logging = (value == "true" || value == "1");
    }
    else if (key == "enable_statistics") {
        config.enable_statistics = (value == "true" || value == "1");
    }
    
    return config;
}

std::vector<std::string> ConfigManager::split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

double ConfigManager::parse_percentage(const std::string& str) {
    std::string clean_str = str;
    clean_str.erase(std::remove_if(clean_str.begin(), clean_str.end(), ::isspace), clean_str.end());
    
    if (clean_str.back() == '%') {
        clean_str.pop_back();
    }
    
    try {
        double value = std::stod(clean_str);
        return std::max(0.0, std::min(100.0, value)) / 100.0;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing percentage: " << str << std::endl;
        return 0.0;
    }
}

int ConfigManager::parse_milliseconds(const std::string& str) {
    std::string clean_str = str;
    clean_str.erase(std::remove_if(clean_str.begin(), clean_str.end(), ::isspace), clean_str.end());
    
    if (clean_str.length() > 2 && clean_str.substr(clean_str.length() - 2) == "ms") {
        clean_str = clean_str.substr(0, clean_str.length() - 2);
    }
    
    try {
        return std::stoi(clean_str);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing milliseconds: " << str << std::endl;
        return 0;
    }
}