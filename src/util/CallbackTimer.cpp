#include "CallbackTimer.hpp"

#include <chrono>

CallbackTimer::~CallbackTimer()
{
    if (_execute.load(std::memory_order_acquire))
    {
        stop();
    };
}

void CallbackTimer::stop()
{
    _execute.store(false, std::memory_order_release);
    if (_thd.joinable())
    {
        _thd.join();
    }
}

void CallbackTimer::start(int interval, const std::function<void(void*)>& func, void* userData)
{
    if (_execute.load(std::memory_order_acquire))
    {
        stop();
    };
    _execute.store(true, std::memory_order_release);
    _thd = std::thread([this, interval, func, userData]() {
        while (_execute.load(std::memory_order_acquire))
        {
            auto start = std::chrono::high_resolution_clock::now();
            func(userData);
            // std::this_thread::sleep_for(
            //     std::chrono::milliseconds(interval));

            std::this_thread::sleep_until(start + std::chrono::milliseconds(interval));
        }
    });
}

bool CallbackTimer::is_running() const noexcept
{
    return (_execute.load(std::memory_order_acquire) && _thd.joinable());
}