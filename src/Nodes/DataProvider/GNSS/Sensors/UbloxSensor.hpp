/// @file UbloxSensor.hpp
/// @brief Ublox Sensor Class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-03-19

#pragma once

#include "Nodes/Node.hpp"
#include "Nodes/DataProvider/Protocol/UartSensor.hpp"
#include "util/UartSensors/Ublox/UbloxUartSensor.hpp"

namespace NAV
{
/// Ublox Sensor Class
class UbloxSensor : public Node, public UartSensor
{
  public:
    /// @brief Default constructor
    UbloxSensor();
    /// @brief Destructor
    ~UbloxSensor() override;
    /// @brief Copy constructor
    UbloxSensor(const UbloxSensor&) = delete;
    /// @brief Move constructor
    UbloxSensor(UbloxSensor&&) = delete;
    /// @brief Copy assignment operator
    UbloxSensor& operator=(const UbloxSensor&) = delete;
    /// @brief Move assignment operator
    UbloxSensor& operator=(UbloxSensor&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

    /// @brief ImGui config window which is shown on double click
    /// @attention Don't forget to set hasConfig to true in the constructor of the node
    void guiConfig() override;

    /// @brief Saves the node into a json object
    [[nodiscard]] json save() const override;

    /// @brief Restores the node from a json object
    /// @param[in] j Json object with the node state
    void restore(const json& j) override;

    /// @brief Resets the node. It is guaranteed that the node is initialized when this is called.
    bool resetNode() override;

  private:
    constexpr static size_t OutputPortIndex_UbloxObs = 0; ///< @brief Flow (UbloxObs)

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// @brief Callback handler for notifications of new asynchronous data packets received
    /// @param[in, out] userData Pointer to the data we supplied when we called registerAsyncPacketReceivedHandler
    /// @param[in] p Encapsulation of the data packet. At this state, it has already been validated and identified as an asynchronous data message
    /// @param[in] index Advanced usage item and can be safely ignored for now
    static void asciiOrBinaryAsyncMessageReceived(void* userData, uart::protocol::Packet& p, size_t index);

    /// Sensor Object
    sensors::ublox::UbloxUartSensor sensor;
};

} // namespace NAV