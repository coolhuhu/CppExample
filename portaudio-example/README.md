# portaudio-example
portaudio 开源库使用指南

portaudio 是一个开源的跨平台的语音输入输出的库，它屏蔽了不同系统的音频输入输出接口，调用者可以使用统一的接口来实现不同平台上的音频输入输出。


## 快速上手
### 使用 cmake 编译并使用
这里将 [sherpa-onnx](https://github.com/k2-fsa/sherpa-onnx) 中的 cmake 脚本搬运过来，通过 cmake 可以很方便的构建 portaudio 静态库并使用它。具体参考 CMakeLists.txt。

### 使用 callback 方式调用输入输出设备
PortAudio V19 支持阻塞读/写的方式，这里不做介绍，可以在官方的 [tutorial](https://files.portaudio.com/docs/v19-doxydocs/tutorial_start.html) 中查看。使用 callback 方式进行设备的输入输出流的读写是更一般的方式，也是推荐的方式。

使用 portaudio 库进行语音输入输出的一般步骤如下：
1. 定义回调函数；
2. Pa_Initialize 初始化 portaudio 库，可以把 portaudio 库理解为一个引擎，提供了调用底层音频输入输出接口的能力；
3. 设置输入/输出相关的参数，这些参数包括：输入/输出设备、采样类型、通道数、延迟等；
4. 打开一个输入输出流（Pa_OpenStream），通过传入的参数来决定是打开一个输入流还是输出流，或者两者一起打开；Pa_OpenStream 函数传入回调函数地址，当 portaudio 内部回调用回调函数；
5. 开始捕获输入输出流；
6. 停止输入输出流；
7. Pa_Terminate 终止 portaudio 库，释放资源；

伪代码调用流程如下：
```cpp
// 1.定义回调函数
static int Callback(const void *inputBuffer, void *outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo *timeInfo,
                    PaStreamCallbackFlags statusFlags, void *userData);

// 2. 
PaError err = paNoError;
rr = Pa_Initialize();

// 3.
PaStream *stream;
PaStreamParameters inputParameters, outputParameters;
inputParameters.device = Pa_GetDefaultInputDevice();
inputParameters.channelCount = 1;
inputParameters.sampleFormat = paFloat32;
// outputParameters ...

// 4.
err = Pa_OpenStream(&stream, &inputParameters, NULL, /* &outputParameters, */
                      SAMPLE_RATE, FRAMES_PER_BUFFER,
                      paClipOff, /* we won't output out of range samples so
                                    don't bother clipping them */
                      recordCallback, &data);

// 5.              
err = Pa_StartStream(stream);

// 6.
err = Pa_CloseStream(stream);

// 7.
Pa_Terminate();
```

**接口详解：**
1. 定义回调函数 `PaStreamCallback`。
    ```cpp
    // 接口定义
    typedef int PaStreamCallback(const void *input,
                                void *output,
                                unsigned long frameCount,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData);
    ```

    定义的回调函数会由 portaudio 引擎在恰当的时间调用。以调用输入设备为例，当你调用麦克风进行录音，portaudio 引擎每获取64帧大小的数据调用一次回调函数，将麦克风捕获的数据传入回调函数的第一个参数 inputBuffer，你可以在回调函数中定义操作 inputBufer 的逻辑来获取麦克风录下的语音数据。

    返回值（int）定义如下，当返回值为0时，表示该回调函数应该继续被 portaudio 引擎触发调用；非0时，表示该回调函数应该停止被 portaudio 引擎触发调用。
    ```cpp
    typedef enum PaStreamCallbackResult
    {
        paContinue=0,   /**< Signal that the stream should continue invoking the callback and processing audio. */
        paComplete=1,   /**< Signal that the stream should stop invoking the callback and finish once all output samples have played. */
        paAbort=2       /**< Signal that the stream should stop invoking the callback and finish as soon as possible. */
    } PaStreamCallbackResult;
    ```

    函数形参定义：
    - `input` portaudio 将语音的输入流传入给 `input` 指针，第一个参数 `input` 与第二个参数 `output` 都和第三个参数 `frameCount` 配合值使用，即 portaudio 一次将 `frameCount` 大小的语音数据以 `input` 指针传入，一次将 `frameCount` 大小的数据传入给音频输出设备。
    - `output` 和 `input` 参数相似，`input` 使用 portaudio 传入的，提供给调用者读取的；`output` 是调用者传入给 portaudio 的。
    - `frameCount` portaudio 在一次回调函数中写入 `input` 的数据量，以及一次从 `output` 中读取的数据量。该参数值由 `Pa_OpenStream` 接口中的第五个参数传入，一般设置为0，让 portaudio 引擎内部决定每次调用回调函数时，传入给 `frameCount` 的值大小。下面介绍 `Pa_OpenStream` 接口时会再次介绍。
    - `const PaStreamCallbackTimeInfo* timeInfo` 这里不会用到这个参数，暂时忽略。
    - `PaStreamCallbackFlags statusFlags` 回调函数中反映输入输出流状态的标识，这里不会用到这个参数，暂时忽略。
    - `void *userData` 用户传入的数据指针。

2. portaudio 资源初始化 `Pa_Initialize` 。
   
   返回 PaError 类型值，若返回值不等于 paNoError，则说明初始化失败。

3. 设置输入输出流相关的参数。
   
   `PaStreamParameters inputParameters, outputParameters;`  `PaStreamParameters` 结构体中定义了如下和输入/输出流相关的参数：

   ```cpp
   typedef struct PaStreamParameters {
     PaDeviceIndex device;
     int channelCount;
     PaSampleFormat sampleFormat;
     PaTime suggestedLatency;
     void *hostApiSpecificStreamInfo;
   } PaStreamParameters;
   ```

   - `PaDeviceIndex` 获取输入/输出的设备 Index，一般调用 `Pa_GetDefaultInputDevice()` 和 `Pa_GetDefaultOutputDevice()` 接口获取设备 Index。
   - `channelCount` 输入/输出通道数。
   - `sampleFormat` 输入/输出数据的采样格式。
   - `suggestedLatency` 输入/输出流建议的延迟，一般为: 
        ```cpp
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
        ```
   - `void *hostApiSpecificStreamInfo` 这里不用关心这个参数，传入 `nullptr` 即可。

4. 打开输入/输出流 `Pa_OpenStream`。

   ```cpp
    PaError Pa_OpenStream(PaStream **stream,
        const PaStreamParameters *inputParameters,
        const PaStreamParameters *outputParameters,
        double sampleRate,
        unsigned long framesPerBuffer,
        PaStreamFlags streamFlags,
        PaStreamCallback *streamCallback,
        void *userData 
    );
   ```

   `Pa_OpenStream` 接口的调用将打开一个输入、输出流，或者输入输出流同时打开，这取决于你传入的 `inputParameters` 和 `outputParameters` 参数是否为 nullptr。

   `sampleRate` 指定采样率。

   `framesPerBuffer` 参数传入给 callback 函数，一般设置为0，让 portaudio库根据延时自行确定。你也可以根据需求自行设置传入。

   `streamFlags` 一般设置为 `paClipOff`。

   `streamCallback` 定义的回调函数。

   `userData` 传入给回调函数。例如你要调用 portaudio 来播放一条传入的语音文件，那么你就要定义对应的数据结构，通过 `userData` 传入给 `Pa_OpenStream` 接口，然后在你定义的回调函数进行解析，将其中的语音数据写入给回调函数中的第二个参数 `output` 。详细使用参考 [play-wav.cc](example/play-wav.cc) 。

5. 开始捕获输入/输出流 (`Pa_StartStream`)。

   `Pa_OpenStream` 接口可以理解为真正的数据流捕获做准备工作，调用 Pa_StartStream 接口，才是真正的数据捕获。
   
6. 停止捕获输入/输出流。
   当完成的输入/输出流的捕获，需要调用 `Pa_StopStream` 接口以关闭输入/输出流，可以简单理解为关闭麦克风和扬声器。

7. 停止 portaudio，释放资源（`Pa_Terminate`）。

结合下面给出的 example，来进行 portaudio 的快速使用。

## Example
- [paex_record.cc](example/paex_record.cc) portaudio 官方 examples 中的一个例子，使用麦克风录音并播放。
- [record-to-file.cc](example/record-to-file.cc) 录音并保存为文件，保存的文件格式为 16k16bit 1 channel wav。
- [play-wav.cc](example/play-wav.cc) 播放一个语音文件。可以先执行 record-to-file 程序，然后执行 play-wav 程序将语音内容播放出来。
- [microphone.h](example/microphone.h) 和 [microphone.cc](example/microphone.cc) 是从 [sherpa-onnx](https://github.com/k2-fsa/sherpa-onnx) 中扣出的代码。封装了 portaudio 读取输入流的功能。
