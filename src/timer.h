#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <cstdint>

class Timer
{
public:
    using Nanos   = std::chrono::nanoseconds;
    using Micros  = std::chrono::microseconds;
    using Millis  = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours   = std::chrono::hours;

    Timer()
        : m_start(std::chrono::steady_clock::now())
    {
    }

    void restart()
    {
        m_start = std::chrono::steady_clock::now();
    }

    template <typename T>
    uint64_t click()
    {
        uint64_t elapsed = glance<T>();

        restart();

        return elapsed;
    }

    template <typename T>
    uint64_t glance() const
    {
        auto elapsed =
            std::chrono::steady_clock::now() - m_start;

        return std::chrono::duration_cast<T>(elapsed).count();
    }

private:
    std::chrono::steady_clock::time_point m_start;
};


#endif  // TIMER_H

