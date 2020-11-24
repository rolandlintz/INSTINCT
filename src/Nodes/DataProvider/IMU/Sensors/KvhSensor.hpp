/// @file KvhSensor.hpp
/// @brief KVH Sensors
/// @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
/// @date 2020-06-30

#pragma once

#include "NodeData/IMU/KvhObs.hpp"
#include "../Imu.hpp"
#include "../../Protocol/UartSensor.hpp"
#include "util/UartSensors/KVH/KvhUartSensor.hpp"

namespace NAV
{
/// KVH Sensor Class
class KvhSensor final : public UartSensor, public Imu
{
  public:
    /// @brief Constructor
    /// @param[in] name Name of the Sensor
    /// @param[in] options Program options string map
    KvhSensor(const std::string& name, const std::map<std::string, std::string>& options);

    /// @brief Default constructor
    KvhSensor() = default;
    /// @brief Destructor
    ~KvhSensor() final;
    /// @brief Copy constructor
    KvhSensor(const KvhSensor&) = delete;
    /// @brief Move constructor
    KvhSensor(KvhSensor&&) = delete;
    /// @brief Copy assignment operator
    KvhSensor& operator=(const KvhSensor&) = delete;
    /// @brief Move assignment operator
    KvhSensor& operator=(KvhSensor&&) = delete;

    /// @brief Returns the String representation of the Class Type
    /// @return The class type
    [[nodiscard]] constexpr std::string_view type() const final
    {
        return std::string_view("KvhSensor");
    }

    /// @brief Returns the String representation of the Class Category
    /// @return The class category
    [[nodiscard]] constexpr std::string_view category() const final
    {
        return std::string_view("DataProvider");
    }

    /// @brief Returns Gui Configuration options for the class
    /// @return The gui configuration
    [[nodiscard]] std::vector<ConfigOptions> guiConfig() const final
    {
        std::vector<ConfigOptions> configs = { { CONFIG_STRING, "Port", "COM port where the sensor is attached to\n"
                                                                        "- \"COM1\" (Windows format for physical and virtual (USB) serial port)\n"
                                                                        "- \"/dev/ttyS1\" (Linux format for physical serial port)\n"
                                                                        "- \"/dev/ttyUSB0\" (Linux format for virtual (USB) serial port)\n"
                                                                        "- \"/dev/tty.usbserial-FTXXXXXX\" (Mac OS X format for virtual (USB) serial port)\n"
                                                                        "- \"/dev/ttyS0\" (CYGWIN format. Usually the Windows COM port number minus 1. This would connect to COM1)",
                                                 { "/dev/ttyUSB0" } } };
        auto imuConfigs = Imu::guiConfig();
        configs.insert(configs.end(), imuConfigs.begin(), imuConfigs.end());
        return configs;
    }

    /// @brief Returns the context of the class
    /// @return The class context
    [[nodiscard]] constexpr NodeContext context() const final
    {
        return NodeContext::REAL_TIME;
    }

    /// @brief Returns the number of Ports
    /// @param[in] portType Specifies the port type
    /// @return The number of ports
    [[nodiscard]] constexpr uint8_t nPorts(PortType portType) const final
    {
        switch (portType)
        {
        case PortType::In:
            break;
        case PortType::Out:
            return 2U;
        }

        return 0U;
    }

    /// @brief Returns the data types provided by this class
    /// @param[in] portType Specifies the port type
    /// @param[in] portIndex Port index on which the data is sent
    /// @return The data type and subtitle
    [[nodiscard]] constexpr std::pair<std::string_view, std::string_view> dataType(PortType portType, uint8_t portIndex) const final
    {
        switch (portType)
        {
        case PortType::In:
            break;
        case PortType::Out:
            if (portIndex == 0)
            {
                return std::make_pair(KvhObs::type(), std::string_view(""));
            }
            if (portIndex == 1)
            {
                return std::make_pair(ImuPos::type(), std::string_view(""));
            }
        }

        return std::make_pair(std::string_view(""), std::string_view(""));
    }

    /// @brief Handles the data sent on the input port
    /// @param[in] portIndex The input port index
    /// @param[in, out] data The data send on the input port
    void handleInputData([[maybe_unused]] uint8_t portIndex, [[maybe_unused]] std::shared_ptr<NodeData> data) final {}

    /// @brief Requests the node to send out its data
    /// @param[in] portIndex The output port index
    /// @return The requested data or nullptr if no data available
    [[nodiscard]] std::shared_ptr<NodeData> requestOutputData(uint8_t portIndex) final
    {
        if (portIndex == 1)
        {
            return imuPos;
        }

        return nullptr;
    }

  private:
    /// @brief Callback handler for notifications of new asynchronous data packets received
    /// @param[in, out] userData Pointer to the data we supplied when we called registerAsyncPacketReceivedHandler
    /// @param[in] p Encapsulation of the data packet. At this state, it has already been validated and identified as an asynchronous data message
    /// @param[in] index Advanced usage item and can be safely ignored for now
    static void asciiOrBinaryAsyncMessageReceived(void* userData, uart::protocol::Packet& p, size_t index);

    /// Sensor Object
    sensors::kvh::KvhUartSensor sensor;

    /// Previous Sequence number to check for order errors
    uint8_t prevSequenceNumber = UINT8_MAX;
};

} // namespace NAV