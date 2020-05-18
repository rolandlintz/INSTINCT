#include "DataLogger.hpp"

#include "util/Logger.hpp"

#include "vn/sensors.h"
// #include "ub/sensors/sensors.hpp"

NAV::DataLogger::DataLogger(const std::string& name, std::deque<std::string>& options)
    : Node(name)
{
    LOG_TRACE("called for {}", name);

    if (!options.empty())
    {
        path = options.at(0);
        options.pop_front();
    }
    if (!options.empty())
    {
        if (options.at(0) == "ascii")
        {
            fileType = FileType::ASCII;
        }
        else if (options.at(0) == "binary")
        {
            fileType = FileType::BINARY;
        }
        else
        {
            LOG_CRITICAL("Node {} has unknown file type {}", name, options.at(0));
        }

        options.pop_front();
    }

    if (fileType == FileType::BINARY)
    {
        filestream.open(path, std::ios_base::trunc | std::ios_base::binary);
    }
    else
    {
        filestream.open(path, std::ios_base::trunc);
    }

    if (!filestream.good())
    {
        LOG_CRITICAL("{} could not be opened", name);
    }
}

NAV::DataLogger::~DataLogger()
{
    LOG_TRACE("called for {}", name);

    if (filestream.is_open())
    {
        filestream.flush();
        filestream.close();
    }

    LOG_DEBUG("{} successfully deinitialized", name);
}