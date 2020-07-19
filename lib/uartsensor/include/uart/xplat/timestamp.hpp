#pragma once

#include "uart/xplat/int.hpp"
#include "uart/xplat/export.hpp"

namespace uart::xplat
{
struct proglib_DLLEXPORT TimeStamp
{
  public:
    TimeStamp();

  private:
    TimeStamp(int64_t sec, uint64_t usec);

  public:
    // \brief Returns a timestamp.
    //
    // \return The timestamp.
    static TimeStamp get();

    // HACK: Current values are made public until the TimeStamp interface
    // is fully worked out.
    //private:
  public:
    int64_t _sec;   // Seconds.
    uint64_t _usec; // Microseconds.
};

/// \brief Provides simple timing capabilities.
class proglib_DLLEXPORT Stopwatch
{
  public:
    /// \brief Creates a new Stopwatch and starts timing.
    Stopwatch();

    ~Stopwatch();

    /// \brief Resets the Stopwatch.
    void reset();

    /// \brief Gets the elapsed time in milliseconds.
    ///
    /// \return The elapsed time in milliseconds.
    float elapsedMs();

  private:
    struct Impl;
    Impl* _pi;
};

} // namespace uart::xplat