#include <portaudio.h>

#include <fstream>
#include <iostream>
#include <vector>

#define FRAMES_PER_BUFFER 512
typedef short SAMPLE;

struct WAVHeader {
  char riff[4];            // "RIFF"
  uint32_t chunkSize;      // 文件总大小 - 8
  char wave[4];            // "WAVE"
  char fmt[4];             // "fmt "
  uint32_t subchunk1Size;  // 16 for PCM
  uint16_t audioFormat;    // PCM = 1
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char data[4];  // "data"
  uint32_t dataSize;
};

struct PlaybackData {
  const SAMPLE *audioData;
  size_t totalFrames;
  size_t frameIndex;
  int numChannels;
};

// 回调函数
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  PlaybackData *data = static_cast<PlaybackData *>(userData);
  SAMPLE *out = static_cast<SAMPLE *>(outputBuffer);
  size_t framesLeft = data->totalFrames - data->frameIndex;
  size_t framesToCopy =
      (framesLeft > framesPerBuffer) ? framesPerBuffer : framesLeft;

  const SAMPLE *src = data->audioData + data->frameIndex * data->numChannels;
  std::copy(src, src + framesToCopy * data->numChannels, out);

  // 如果还有数据剩余，则清零输出剩下部分
  if (framesToCopy < framesPerBuffer) {
    std::fill(out + framesToCopy * data->numChannels,
              out + framesPerBuffer * data->numChannels, 0);
    return paComplete;
  }

  data->frameIndex += framesToCopy;
  return paContinue;
}

bool ReadWavBuffer(const std::string &filename, std::vector<SAMPLE> *buffer) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "无法打开 WAV 文件: " << filename << std::endl;
    return false;
  }

  WAVHeader header;
  file.read(reinterpret_cast<char *>(&header), sizeof(header));

  // 检查 WAV 格式
  if (std::string(header.riff, 4) != "RIFF" ||
      std::string(header.wave, 4) != "WAVE" || header.audioFormat != 1 ||
      header.bitsPerSample != 16) {
    std::cerr << "不支持的 WAV 格式，要求 PCM 16-bit。" << std::endl;
    return false;
  }

  size_t totalSamples = header.dataSize / sizeof(SAMPLE);
  buffer->clear();
  buffer->resize(totalSamples);
  file.read(reinterpret_cast<char *>(buffer->data()), header.dataSize);
  file.close();

  return true;
}

bool playWavWithCallback(const std::string &filename) {
  std::vector<SAMPLE> audioBuffer;
  if (!ReadWavBuffer(filename, &audioBuffer)) {
  }

  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "PortAudio 初始化失败: " << Pa_GetErrorText(err) << std::endl;
    return false;
  }

  PaStreamParameters outputParams;
  outputParams.device = Pa_GetDefaultOutputDevice();
  if (outputParams.device == paNoDevice) {
    std::cerr << "找不到输出设备" << std::endl;
    Pa_Terminate();
    return false;
  }

  outputParams.channelCount = 1;
  outputParams.sampleFormat = paInt16;
  outputParams.suggestedLatency =
      Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
  outputParams.hostApiSpecificStreamInfo = nullptr;

  PlaybackData playbackData;
  playbackData.audioData = audioBuffer.data();
  playbackData.totalFrames = audioBuffer.size();
  playbackData.frameIndex = 0;
  playbackData.numChannels = 1;

  PaStream *stream;
  err = Pa_OpenStream(&stream, nullptr, &outputParams, 16000, FRAMES_PER_BUFFER,
                      paClipOff, paCallback, &playbackData);
  if (err != paNoError) {
    std::cerr << "打开音频流失败: " << Pa_GetErrorText(err) << std::endl;
    Pa_Terminate();
    return false;
  }

  err = Pa_StartStream(stream);
  if (err != paNoError) {
    std::cerr << "启动流失败: " << Pa_GetErrorText(err) << std::endl;
    Pa_CloseStream(stream);
    Pa_Terminate();
    return false;
  }

  std::cout << "正在播放：" << filename << std::endl;

  // 等待播放完成
  while ((err = Pa_IsStreamActive(stream)) == 1) {
    Pa_Sleep(100);
  }

  if (err < 0) {
    std::cerr << "播放过程中发生错误: " << Pa_GetErrorText(err) << std::endl;
  }

  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  std::cout << "播放完成。" << std::endl;
  return true;
}

int main(int argc, char *argv[]) {
  std::string wavFile(argv[1]);
  if (!playWavWithCallback(wavFile)) {
    return 1;
  }
  return 0;
}
