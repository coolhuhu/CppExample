#ifndef TIMER_UTIL_H_
#define TIMER_UTIL_H_

#include <chrono>

class Timer {
 public:
  static auto Time() { return std::chrono::steady_clock::now(); }

  static auto GetCurrentSysTime() { return std::chrono::system_clock::now(); }

  template <typename T>
  static int ElapsedMilli(T &start, T &end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
        .count();
  }

  template <typename T>
  static int ElapsedMicro(T &start, T &end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
        .count();
  }
};

#endif  // TIMER_UTIL_H_