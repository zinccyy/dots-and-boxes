#include <chrono>
#include <engine/timer.hpp>

namespace eng
{
Timer::Timer() : mLast(std::chrono::system_clock::now())
{
}
double Timer::getElapsedSeconds()
{
    double elapsed = 0;
    auto current = std::chrono::system_clock::now();

    elapsed = static_cast<std::chrono::duration<double>>(current - mLast).count();
    mLast = current;

    return elapsed;
}
} // namespace eng