#include <iostream>
#include <string>
#include <toml.hpp>

void ParseConfig(const std::string &config_file) {
  auto config = toml::parse(config_file);

  std::string feature_type = toml::find<std::string>(config, "FeatureType");
  std::cout << "FeatureType: " << feature_type << std::endl;

  auto lofar_config = toml::find(config, "LofarConfig");
  std::cout << "LofarConfig:" << std::endl;
  std::cout << "  energy_floor: "
            << toml::find<float>(lofar_config, "energy_floor") << std::endl;
  std::cout << "  do_cmvn: "
            << (toml::find<bool>(lofar_config, "do_cmvn") ? "true" : "false")
            << std::endl;
  std::cout << "  low_freq: " << toml::find<int32_t>(lofar_config, "low_freq")
            << std::endl;
  std::cout << "  high_freq: " << toml::find<int32_t>(lofar_config, "high_freq")
            << std::endl;

  auto lofar_frame_options = toml::find(lofar_config, "FrameExtractionOptions");
  std::cout << "  FrameExtractionOptions:" << std::endl;
  std::cout << "    samp_freq: "
            << toml::find<int32_t>(lofar_frame_options, "samp_freq")
            << std::endl;
  std::cout << "    frame_shift_ms: "
            << toml::find<float>(lofar_frame_options, "frame_shift_ms")
            << std::endl;
  std::cout << "    frame_length_ms: "
            << toml::find<float>(lofar_frame_options, "frame_length_ms")
            << std::endl;
  std::cout << "    dither: "
            << toml::find<float>(lofar_frame_options, "dither") << std::endl;
  std::cout << "    preemph_coeff: "
            << toml::find<float>(lofar_frame_options, "preemph_coeff")
            << std::endl;
  std::cout << "    remove_dc_offset: "
            << (toml::find<bool>(lofar_frame_options, "remove_dc_offset")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    window_type: "
            << toml::find<std::string>(lofar_frame_options, "window_type")
            << std::endl;
  std::cout << "    round_to_power_of_two: "
            << (toml::find<bool>(lofar_frame_options, "round_to_power_of_two")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    blackman_coeff: "
            << toml::find<float>(lofar_frame_options, "blackman_coeff")
            << std::endl;
  std::cout << "    snip_edges: "
            << (toml::find<bool>(lofar_frame_options, "snip_edges") ? "true"
                                                                    : "false")
            << std::endl;

  auto demon_config = toml::find(config, "DemonConfig");
  std::cout << "DemonConfig:" << std::endl;
  std::cout << "  nfft: " << toml::find<int32_t>(demon_config, "nfft")
            << std::endl;
  std::cout << "  low_freq: " << toml::find<float>(demon_config, "low_freq")
            << std::endl;
  std::cout << "  high_freq: " << toml::find<float>(demon_config, "high_freq")
            << std::endl;
  std::cout << "  sample_rate: "
            << toml::find<int32_t>(demon_config, "sample_rate") << std::endl;
  std::cout << "  output_len: "
            << toml::find<int32_t>(demon_config, "output_len") << std::endl;

  auto fbank_config = toml::find(config, "FbankConfig");
  std::cout << "FbankConfig:" << std::endl;
  std::cout << "  use_energy: "
            << (toml::find<bool>(fbank_config, "use_energy") ? "true" : "false")
            << std::endl;
  std::cout << "  energy_floor: "
            << toml::find<float>(fbank_config, "energy_floor") << std::endl;
  std::cout << "  raw_energy: "
            << (toml::find<bool>(fbank_config, "raw_energy") ? "true" : "false")
            << std::endl;
  std::cout << "  htk_compat: "
            << (toml::find<bool>(fbank_config, "htk_compat") ? "true" : "false")
            << std::endl;
  std::cout << "  use_log_fbank: "
            << (toml::find<bool>(fbank_config, "use_log_fbank") ? "true"
                                                                : "false")
            << std::endl;
  std::cout << "  use_power: "
            << (toml::find<bool>(fbank_config, "use_power") ? "true" : "false")
            << std::endl;

  auto fbank_frame_options = toml::find(fbank_config, "FrameExtractionOptions");
  std::cout << "  FrameExtractionOptions:" << std::endl;
  std::cout << "    samp_freq: "
            << toml::find<int32_t>(fbank_frame_options, "samp_freq")
            << std::endl;
  std::cout << "    frame_shift_ms: "
            << toml::find<float>(fbank_frame_options, "frame_shift_ms")
            << std::endl;
  std::cout << "    frame_length_ms: "
            << toml::find<float>(fbank_frame_options, "frame_length_ms")
            << std::endl;
  std::cout << "    dither: "
            << toml::find<float>(fbank_frame_options, "dither") << std::endl;
  std::cout << "    preemph_coeff: "
            << toml::find<float>(fbank_frame_options, "preemph_coeff")
            << std::endl;
  std::cout << "    remove_dc_offset: "
            << (toml::find<bool>(fbank_frame_options, "remove_dc_offset")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    window_type: "
            << toml::find<std::string>(fbank_frame_options, "window_type")
            << std::endl;
  std::cout << "    round_to_power_of_two: "
            << (toml::find<bool>(fbank_frame_options, "round_to_power_of_two")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    blackman_coeff: "
            << toml::find<float>(fbank_frame_options, "blackman_coeff")
            << std::endl;
  std::cout << "    snip_edges: "
            << (toml::find<bool>(fbank_frame_options, "snip_edges") ? "true"
                                                                    : "false")
            << std::endl;

  auto mel_banks_options = toml::find(fbank_config, "MelBanksOptions");
  std::cout << "  MelBanksOptions:" << std::endl;
  std::cout << "    num_bins: "
            << toml::find<int32_t>(mel_banks_options, "num_bins") << std::endl;
  std::cout << "    low_freq: "
            << toml::find<float>(mel_banks_options, "low_freq") << std::endl;
  std::cout << "    high_freq: "
            << toml::find<float>(mel_banks_options, "high_freq") << std::endl;
  std::cout << "    vtln_low: "
            << toml::find<float>(mel_banks_options, "vtln_low") << std::endl;
  std::cout << "    vtln_high: "
            << toml::find<float>(mel_banks_options, "vtln_high") << std::endl;
  std::cout << "    debug_mel: "
            << (toml::find<bool>(mel_banks_options, "debug_mel") ? "true"
                                                                 : "false")
            << std::endl;
  std::cout << "    htk_mode: "
            << (toml::find<bool>(mel_banks_options, "htk_mode") ? "true"
                                                                : "false")
            << std::endl;
  std::cout << "    is_librosa: "
            << (toml::find<bool>(mel_banks_options, "is_librosa") ? "true"
                                                                  : "false")
            << std::endl;
  std::cout << "    norm: "
            << toml::find<std::string>(mel_banks_options, "norm") << std::endl;

  auto stft_config = toml::find(config, "StftConfig");
  std::cout << "StftConfig:" << std::endl;
  std::cout << "  energy_floor: "
            << toml::find<float>(stft_config, "energy_floor") << std::endl;
  std::cout << "  do_cmvn: "
            << (toml::find<bool>(stft_config, "do_cmvn") ? "true" : "false")
            << std::endl;
  std::cout << "  low_freq: " << toml::find<int32_t>(stft_config, "low_freq")
            << std::endl;
  std::cout << "  high_freq: " << toml::find<int32_t>(stft_config, "high_freq")
            << std::endl;

  auto stft_frame_options = toml::find(stft_config, "FrameExtractionOptions");
  std::cout << "  FrameExtractionOptions:" << std::endl;
  std::cout << "    samp_freq: "
            << toml::find<int32_t>(stft_frame_options, "samp_freq")
            << std::endl;
  std::cout << "    frame_shift_ms: "
            << toml::find<float>(stft_frame_options, "frame_shift_ms")
            << std::endl;
  std::cout << "    frame_length_ms: "
            << toml::find<float>(stft_frame_options, "frame_length_ms")
            << std::endl;
  std::cout << "    dither: " << toml::find<float>(stft_frame_options, "dither")
            << std::endl;
  std::cout << "    preemph_coeff: "
            << toml::find<float>(stft_frame_options, "preemph_coeff")
            << std::endl;
  std::cout << "    remove_dc_offset: "
            << (toml::find<bool>(stft_frame_options, "remove_dc_offset")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    window_type: "
            << toml::find<std::string>(stft_frame_options, "window_type")
            << std::endl;
  std::cout << "    round_to_power_of_two: "
            << (toml::find<bool>(stft_frame_options, "round_to_power_of_two")
                    ? "true"
                    : "false")
            << std::endl;
  std::cout << "    blackman_coeff: "
            << toml::find<float>(stft_frame_options, "blackman_coeff")
            << std::endl;
  std::cout << "    snip_edges: "
            << (toml::find<bool>(stft_frame_options, "snip_edges") ? "true"
                                                                   : "false")
            << std::endl;
}

int main(int argc, char **argv) {
  std::string config_file(argv[1]);
  ParseConfig(config_file);
}