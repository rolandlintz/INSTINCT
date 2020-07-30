#include "UbloxFile.hpp"

#include "util/Logger.hpp"
#include <ios>
#include <cmath>
#include <array>

#include "util/UartSensors/Ublox/UbloxUtilities.hpp"

NAV::UbloxFile::UbloxFile(const std::string& name, const std::map<std::string, std::string>& options)
    : FileReader(options), Gnss(name, options), sensor(name)
{
    LOG_TRACE("called for {}", name);

    fileType = determineFileType();

    if (fileType == FileType::BINARY)
    {
        filestream = std::ifstream(path, std::ios_base::in | std::ios_base::binary);
    }
    else
    {
        filestream = std::ifstream(path);
    }

    if (filestream.good())
    {
        dataStart = filestream.tellg();

        if (fileType == FileType::ASCII)
        {
            LOG_DEBUG("{}-ASCII-File successfully initialized", name);
        }
        else
        {
            LOG_DEBUG("{}-Binary-File successfully initialized", name);
        }
    }
    else
    {
        LOG_CRITICAL("{} could not open file {}", name, path);
    }
}

NAV::UbloxFile::~UbloxFile()
{
    LOG_TRACE("called for {}", name);

    // removeAllCallbacks();
    if (filestream.is_open())
    {
        filestream.close();
    }
}

void NAV::UbloxFile::resetNode()
{
    // Return to position
    filestream.clear();
    filestream.seekg(dataStart, std::ios_base::beg);
}

std::shared_ptr<NAV::UbloxObs> NAV::UbloxFile::pollData(bool peek)
{
    if (fileType == FileType::ASCII)
    {
        // TODO: Implement UbloxFile Ascii reading
        LOG_CRITICAL("Ascii UbloxFile pollData is not implemented yet.");
        return nullptr;
    }

    // Get current position
    auto pos = filestream.tellg();
    uint8_t i = 0;
    std::unique_ptr<uart::protocol::Packet> packet = nullptr;
    while (filestream.readsome(reinterpret_cast<char*>(&i), 1))
    {
        packet = sensor.findPacket(i);

        if (packet != nullptr)
        {
            break;
        }
    }

    if (!packet)
    {
        return nullptr;
    }

    auto obs = std::make_shared<UbloxObs>(*packet);

    // Check if package is empty
    if (obs->raw.getRawDataLength() == 0)
    {
        return nullptr;
    }

    sensors::ublox::decryptUbloxObs(obs, currentInsTime, peek);

    if (peek)
    {
        // Return to position before "Read line".
        filestream.seekg(pos, std::ios_base::beg);
    }
    else
    {
        // Calls all the callbacks
        invokeCallbacks(obs);
    }

    return obs;
}

NAV::FileReader::FileType NAV::UbloxFile::determineFileType()
{
    LOG_TRACE("called for {}", name);

    filestream = std::ifstream(path);

    constexpr uint16_t BUFFER_SIZE = 10;

    std::array<char, BUFFER_SIZE> buffer{};
    if (filestream.good())
    {
        filestream.read(buffer.data(), BUFFER_SIZE);

        if ((static_cast<uint8_t>(buffer.at(0)) == sensors::ublox::UbloxUartSensor::BinarySyncChar1
             && static_cast<uint8_t>(buffer.at(1)) == sensors::ublox::UbloxUartSensor::BinarySyncChar2)
            || buffer.at(0) == sensors::ublox::UbloxUartSensor::AsciiStartChar)
        {
            filestream.close();
            LOG_DEBUG("{} has the file type: Binary", name);
            return FileType::BINARY;
        }
        filestream.close();

        LOG_CRITICAL("{} could not determine file type", name);
    }

    LOG_CRITICAL("{} could not open file {}", name, path);
    return FileType::NONE;
}