#pragma once

#include "network_simulator.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class ConfigManager {
public:
    static NetworkConfig load_from_file(const std::string& filename);
    static NetworkConfig load_from_args(int argc, char* argv[]);
    static void save_to_file(const NetworkConfig& config, const std::string& filename);
    static void print_help();
    static void print_config(const NetworkConfig& config);
    
private:
    static NetworkConfig parse_config_line(const std::string& line);
    static std::vector<std::string> split_string(const std::string& str, char delimiter);
    static double parse_percentage(const std::string& str);
    static int parse_milliseconds(const std::string& str);
};