#pragma once

#include <chrono>

namespace eng
{
class Timer
{
  public:
    Timer();

    double getElapsedSeconds();

  private:
    std::chrono::time_point<std::chrono::system_clock> mLast;
};
} // namespace eng