#include <math.h>
#include <stdio.h>

#include "portaudio.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256
#define TABLE_SIZE 200

typedef struct {
  float sine[TABLE_SIZE];
  int phase;
} paTestData;

static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags, void *userData) {
  paTestData *data = (paTestData *)userData;
  float *out = (float *)outputBuffer;
  for (unsigned long i = 0; i < framesPerBuffer; i++) {
    *out++ = data->sine[data->phase++];
    if (data->phase >= TABLE_SIZE) data->phase = 0;
  }
  return paContinue;
}

int main() {
  PaStream *stream;
  paTestData data;
  for (int i = 0; i < TABLE_SIZE; i++) {
    data.sine[i] = (float)sin(((double)i / TABLE_SIZE) * M_PI * 2.);
  }
  data.phase = 0;

  Pa_Initialize();

  Pa_OpenDefaultStream(&stream,
                       0,          // 输入通道数
                       1,          // 输出通道数
                       paFloat32,  // 采样格式
                       SAMPLE_RATE, FRAMES_PER_BUFFER, paCallback, &data);

  Pa_StartStream(stream);
  Pa_Sleep(3000);  // 播放 3 秒
  Pa_StopStream(stream);
  Pa_CloseStream(stream);

  Pa_Terminate();
  return 0;
}
