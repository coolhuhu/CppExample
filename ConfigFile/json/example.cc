#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

void ParserConfig(const std::string &config_file) {
  std::fstream f(config_file);
  nlohmann::json config = nlohmann::json::parse(f);

  std::string feature_type = config.at("FeatureType");
  std::cout << "FeatureType: " << feature_type << std::endl;

  std::cout << "--------------- LofarConfig: ------------------" << std::endl;
  auto lofar_config = config.at("LofarConfig");
  std::cout << "FrameExtractionOptions:" << std::endl;
  auto frame_options = lofar_config.at("FrameExtractionOptions");
  std::cout << "  samp_freq: " << frame_options.at("samp_freq") << std::endl;
  std::cout << "  frame_shift_ms: " << frame_options.at("frame_shift_ms")
            << std::endl;
  std::cout << "  frame_length_ms: " << frame_options.at("frame_length_ms")
            << std::endl;
  std::cout << "  dither: " << frame_options.at("dither") << std::endl;
  std::cout << "  preemph_coeff: " << frame_options.at("preemph_coeff")
            << std::endl;
  std::cout << "  remove_dc_offset: " << frame_options.at("remove_dc_offset")
            << std::endl;
  std::cout << "  window_type: " << frame_options.at("window_type")
            << std::endl;
  std::cout << "  round_to_power_of_two: "
            << frame_options.at("round_to_power_of_two") << std::endl;
  std::cout << "  blackman_coeff: " << frame_options.at("blackman_coeff")
            << std::endl;
  std::cout << "  snip_edges: " << frame_options.at("snip_edges") << std::endl;
  std::cout << "Other LofarConfig:" << std::endl;
  std::cout << "  energy_floor: " << lofar_config.at("energy_floor")
            << std::endl;
  std::cout << "  do_cmvn: " << lofar_config.at("do_cmvn") << std::endl;
  std::cout << "  low_freq: " << lofar_config.at("low_freq") << std::endl;
  std::cout << "  high_freq: " << lofar_config.at("high_freq") << std::endl;

  bool round_to_power_of_two = frame_options.at("remove_dc_offset");
  std::cout << "round_to_power_of_two: " << round_to_power_of_two << std::endl;
}

int main(int argc, char **argv) {
  std::string config_file(argv[1]);
  ParserConfig(config_file);
}