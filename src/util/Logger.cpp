// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Logger.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "internal/ConfigManager.hpp"

#include <chrono>
#include <ctime>
#include <iostream>

#define C_NO "\033[0m"

#define C_BLACK "\033[0;30m"
#define C_DARK_GRAY "\033[1;30m"

#define C_RED "\033[0;31m"
#define C_LIGHT_RED "\033[1;31m"

#define C_GREEN "\033[0;32m"
#define C_LIGHT_GREEN "\033[1;32m"

#define C_ORANGE "\033[0;33m"
#define C_YELLOW "\033[1;33m"

#define C_BLUE "\033[0;34m"
#define C_LIGHT_BLUE "\033[1;34m"

#define C_PURPLE "\033[0;35m"
#define C_LIGHT_PURPLE "\033[1;35m"

#define C_CYAN "\033[0;36m"
#define C_LIGHT_CYAN "\033[1;36m"

#define C_LIGHT_GRAY "\033[0;37m"
#define C_WHITE "\033[1;37m"

// See https://github.com/gabime/spdlog/wiki/3.-Custom-formatting for formatting options
const char* logPatternTrace = "[%H:%M:%S.%e] [%^%L%$] [%s:%#] [%!()] %v";
const char* logPatternTraceColor = "[%H:%M:%S.%e] [%^%L%$] [" C_CYAN "%s:%#" C_NO "] [" C_ORANGE "%!()" C_NO "] %v";
const char* logPatternDebug = "[%H:%M:%S.%e] [%^%L%$] [%s:%#] %v";
const char* logPatternDebugColor = "[%H:%M:%S.%e] [%^%L%$] [" C_CYAN "%s:%#" C_NO "] %v";
const char* logPatternInfo = "[%H:%M:%S.%e] [%^%L%$] %v";

Logger::Logger(const std::string& logpath)
{
#ifndef TESTING

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    // Only edit if console and file should log different levels
    console_sink->set_level(spdlog::level::from_str(NAV::ConfigManager::Get<std::string>("console-log-level", "trace")));
    switch (spdlog::level::from_str(NAV::ConfigManager::Get<std::string>("console-log-level", "trace")))
    {
    case spdlog::level::trace:
        console_sink->set_pattern(logPatternTraceColor);
        break;
    case spdlog::level::debug:
        console_sink->set_pattern(logPatternDebugColor);
        break;
    case spdlog::level::info:
    case spdlog::level::warn:
    case spdlog::level::err:
    case spdlog::level::critical:
        console_sink->set_pattern(logPatternInfo);
        break;
    case spdlog::level::off:
    case spdlog::level::n_levels:
        break;
    }

#endif

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logpath, true);
    // Only edit if console and file should log different levels
    file_sink->set_level(spdlog::level::from_str(NAV::ConfigManager::Get<std::string>("file-log-level", "trace")));
    switch (spdlog::level::from_str(NAV::ConfigManager::Get<std::string>("file-log-level", "trace")))
    {
    case spdlog::level::trace:
        file_sink->set_pattern(logPatternTrace);
        break;
    case spdlog::level::debug:
        file_sink->set_pattern(logPatternDebug);
        break;
    case spdlog::level::info:
    case spdlog::level::warn:
    case spdlog::level::err:
    case spdlog::level::critical:
        file_sink->set_pattern(logPatternInfo);
        break;
    case spdlog::level::off:
    case spdlog::level::n_levels:
        break;
    }

    auto ringbuffer_sink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(1024);
    ringbuffer_sink->set_level(spdlog::level::trace);
    ringbuffer_sink->set_pattern(logPatternInfo);

    // Set the logger as default logger
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({
#ifndef TESTING
                                                                                  console_sink,
#endif
                                                                                  file_sink, ringbuffer_sink })));

    // Level should be smaller or equal to the level of the sinks
    spdlog::set_level(spdlog::level::level_enum::trace);
    // Minimum level which automatically triggers a flush
    spdlog::flush_on(spdlog::level::trace);

    writeHeader();
}

Logger::Logger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);

    // Set the logger as default logger
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("console_sink", spdlog::sinks_init_list({ console_sink })));

    // Level should be smaller or equal to the level of the sinks
    spdlog::set_level(spdlog::level::level_enum::trace);
    // Minimum level which automatically triggers a flush
    spdlog::flush_on(spdlog::level::trace);

    // See https://github.com/gabime/spdlog/wiki/3.-Custom-formatting for formatting options
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    spdlog::set_pattern(logPatternTrace);
#elif SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG
    spdlog::set_pattern(logPatternDebug);
#else
    spdlog::set_pattern(logPatternInfo);
#endif

    writeHeader();
}

Logger::~Logger()
{
    writeFooter();

    spdlog::default_logger()->flush();
}

std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> Logger::GetRingBufferSink()
{
    return std::static_pointer_cast<spdlog::sinks::ringbuffer_sink_mt>(spdlog::get("multi_sink")->sinks().back());
}

void Logger::writeSeparator() noexcept
{
    LOG_INFO("===========================================================================================");
}

void Logger::writeHeader() noexcept
{
    writeSeparator();

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    [[maybe_unused]] tm* t = std::localtime(&now_c); // NOLINT(concurrency-mt-unsafe)

#ifdef NDEBUG
    LOG_INFO("Program started in Release on {:04d}-{:02d}-{:02d}", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
#else
    LOG_INFO("Program started in Debug on {:04d}-{:02d}-{:02d}", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);
#endif

    writeSeparator();
}

void Logger::writeFooter() noexcept
{
    writeSeparator();

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    [[maybe_unused]] tm* t = std::localtime(&now_c); // NOLINT(concurrency-mt-unsafe)

    LOG_INFO("Program finished on {:04d}-{:02d}-{:02d}", 1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday);

    writeSeparator();
}