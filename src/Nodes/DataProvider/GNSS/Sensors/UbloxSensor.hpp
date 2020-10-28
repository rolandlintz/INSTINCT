/// @file UbloxSensor.hpp
/// @brief Ublox Sensor Class
/// @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
/// @date 2020-03-19

#pragma once

#include "NodeData/GNSS/UbloxObs.hpp"
#include "../Gnss.hpp"
#include "../../Protocol/UartSensor.hpp"
#include "util/UartSensors/Ublox/UbloxUartSensor.hpp"

namespace NAV
{
/// Ublox Sensor Class
class UbloxSensor final : public UartSensor, public Gnss
{
  public:
    /// @brief Constructor
    /// @param[in] name Name of the Sensor
    /// @param[in, out] options Program options string list
    UbloxSensor(const std::string& name, const std::map<std::string, std::string>& options);

    /// @brief Default constructor
    UbloxSensor() = default;
    /// @brief Destructor
    ~UbloxSensor() final;
    /// @brief Copy constructor
    UbloxSensor(const UbloxSensor&) = delete;
    /// @brief Move constructor
    UbloxSensor(UbloxSensor&&) = delete;
    /// @brief Copy assignment operator
    UbloxSensor& operator=(const UbloxSensor&) = delete;
    /// @brief Move assignment operator
    UbloxSensor& operator=(UbloxSensor&&) = delete;

    /// @brief Returns the String representation of the Class Type
    /// @return The class type
    [[nodiscard]] constexpr std::string_view type() const final
    {
        return std::string_view("UbloxSensor");
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
        return { { CONFIG_STRING, "Port", "COM port where the sensor is attached to\n"
                                          "- \"COM1\" (Windows format for physical and virtual (USB) serial port)\n"
                                          "- \"/dev/ttyS1\" (Linux format for physical serial port)\n"
                                          "- \"/dev/ttyUSB0\" (Linux format for virtual (USB) serial port)\n"
                                          "- \"/dev/tty.usbserial-FTXXXXXX\" (Mac OS X format for virtual (USB) serial port)\n"
                                          "- \"/dev/ttyS0\" (CYGWIN format. Usually the Windows COM port number minus 1. This would connect to COM1)",
                   { "/dev/ttyUSB0" } } };
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
            return 1U;
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
                return std::make_pair(UbloxObs().type(), std::string_view(""));
            }
        }

        return std::make_pair(std::string_view(""), std::string_view(""));
    }

    /// @brief Handles the data sent on the input port
    /// @param[in] portIndex The input port index
    /// @param[in, out] data The data send on the input port
    void handleInputData([[maybe_unused]] uint8_t portIndex, [[maybe_unused]] std::shared_ptr<NodeData> data) final {}

  private:
    /// @brief Callback handler for notifications of new asynchronous data packets received
    /// @param[in, out] userData Pointer to the data we supplied when we called registerAsyncPacketReceivedHandler
    /// @param[in] p Encapsulation of the data packet. At this state, it has already been validated and identified as an asynchronous data message
    /// @param[in] index Advanced usage item and can be safely ignored for now
    static void asciiOrBinaryAsyncMessageReceived(void* userData, uart::protocol::Packet& p, size_t index);

    /// Sensor Object
    sensors::ublox::UbloxUartSensor sensor;
};

} // namespace NAV