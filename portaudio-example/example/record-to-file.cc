#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <vector>

#include "portaudio.h"
#include "wav.h"

#define PA_SAMPLE_TYPE paInt16

int SampleRate = 16000;
int NumChannel = 1;
int BitsPerSample = 16;

std::queue<std::vector<int16_t>> samples_queue;
std::condition_variable condition_variable;
std::mutex mutex;
bool stop = false;

static void Handler(int32_t /*sig*/) {
  stop = true;
  condition_variable.notify_one();
  fprintf(stderr, "\nCaught Ctrl + C. Exiting...\n");
}

static int RecordCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo *timeInfo,
                          PaStreamCallbackFlags statusFlags, void *userData) {
  std::lock_guard<std::mutex> lock(mutex);
  samples_queue.emplace(
      reinterpret_cast<const int16_t *>(inputBuffer),
      reinterpret_cast<const int16_t *>(inputBuffer) + framesPerBuffer);
  condition_variable.notify_one();

  return stop ? paComplete : paContinue;
}

int main(int argc, char *argv[]) {
  signal(SIGINT, Handler);

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "sr") == 0 || strcmp(argv[i], "sample_rate") == 0) {
      SampleRate = atoi(argv[++i]);
    } else if (strcmp(argv[i], "nc") == 0 ||
               strcmp(argv[i], "num_channel") == 0) {
      NumChannel = atoi(argv[++i]);
    }
  }

  PaStreamParameters inputParameters;
  PaStream *stream;
  PaError err = paNoError;

  err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "Init Portaudio Error!" << std::endl;
    Pa_Terminate();
    exit(-1);
  }

  inputParameters.device = Pa_GetDefaultInputDevice();
  if (inputParameters.device == paNoDevice) {
    std::cerr << "Error! No default input device." << std::endl;
    Pa_Terminate();
    exit(-1);
  }
  inputParameters.channelCount = NumChannel;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency =
      Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = nullptr;

  err = Pa_OpenStream(&stream, &inputParameters, nullptr, SampleRate, 64,
                      paClipOff, RecordCallback, nullptr);
  if (err != paNoError) {
    std::cerr << "Pa_OpenStream error!" << std::endl;
    Pa_Terminate();
    exit(-1);
  }

  wenet::StreamWavWriter file_stream(NumChannel, SampleRate, BitsPerSample);
  std::string output_file("output.wav");
  file_stream.Open(output_file);

  std::queue<std::vector<int16_t>> record_queue;

  err = Pa_StartStream(stream);
  if (err != paNoError) {
    fprintf(stderr, "Portaudio | start a stream error: %s\n",
            Pa_GetErrorText(err));
    if (stream) {
      PaError e = Pa_CloseStream(stream);
      fprintf(stderr, "Portaudio | close a stream error: %s\n",
              Pa_GetErrorText(e));
    }
    Pa_Terminate();
  }

  std::cout << "Start recording..." << std::endl;
  std::cout << "Press Ctrl+C to stop recording" << std::endl;

  while (!stop) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      while (samples_queue.empty() && !stop) {
        condition_variable.wait(lock);
      }

      while (!samples_queue.empty()) {
        record_queue.emplace(std::move(samples_queue.front()));
        samples_queue.pop();
      }
    }

    while (!record_queue.empty()) {
      auto &data = record_queue.front();
      file_stream.Write(data.data(), data.size());
      record_queue.pop();
    }
  }

  if (stream) {
    while ((err = Pa_IsStreamActive(stream)) == 1) {
      Pa_Sleep(1000);
    }
    if (err < 0) {
      std::cout << "stream error!" << std::endl;
      Pa_Terminate();
      exit(-1);
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
      std::cerr << "CloseStream error!" << std::endl;
      Pa_Terminate();
      exit(-1);
    }
  }

  file_stream.Close();
  Pa_Terminate();

  return 0;
}
