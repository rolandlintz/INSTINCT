/**
 * @file VectorNavDataLogger.hpp
 * @brief Data Logger for VectorNav observations
 * @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
 * @date 2020-03-17
 */

#pragma once

#include "../DataLogger.hpp"

#include "NodeData/IMU/VectorNavObs.hpp"

namespace NAV
{
/// Data Logger for VectorNav observations
class VectorNavDataLogger final : public DataLogger
{
  public:
    /**
     * @brief Construct a new Data Logger object
     * 
     * @param[in] name Name of the Logger
     * @param[in] options Program options string map
     */
    VectorNavDataLogger(const std::string& name, const std::map<std::string, std::string>& options);

    VectorNavDataLogger() = default;                                     ///< Default Constructor
    ~VectorNavDataLogger() final;                                        ///< Destructor
    VectorNavDataLogger(const VectorNavDataLogger&) = delete;            ///< Copy constructor
    VectorNavDataLogger(VectorNavDataLogger&&) = delete;                 ///< Move constructor
    VectorNavDataLogger& operator=(const VectorNavDataLogger&) = delete; ///< Copy assignment operator
    VectorNavDataLogger& operator=(VectorNavDataLogger&&) = delete;      ///< Move assignment operator

    /**
     * @brief Returns the String representation of the Class Type
     * 
     * @retval constexpr std::string_view The class type
     */
    [[nodiscard]] constexpr std::string_view type() const final
    {
        return std::string_view("VectorNavDataLogger");
    }

    /**
     * @brief Returns the String representation of the Class Category
     * 
     * @retval constexpr std::string_view The class category
     */
    [[nodiscard]] constexpr std::string_view category() const final
    {
        return std::string_view("DataLogger");
    }

    /**
     * @brief Returns Gui Configuration options for the class
     * 
     * @retval std::vector<ConfigOptions> The gui configuration
     */
    [[nodiscard]] std::vector<ConfigOptions> guiConfig() const final
    {
        return { { CONFIG_STRING, "Path", "Path where to save the data to", { "logs/vn-log.csv" } },
                 { CONFIG_LIST, "Type", "Type of the output file", { "[ascii]", "binary" } } };
    }

    /**
     * @brief Returns the context of the class
     * 
     * @retval constexpr std::string_view The class context
     */
    [[nodiscard]] constexpr NodeContext context() const final
    {
        return NodeContext::ALL;
    }

    /**
     * @brief Returns the number of Ports
     * 
     * @param[in] portType Specifies the port type
     * @retval constexpr uint8_t The number of ports
     */
    [[nodiscard]] constexpr uint8_t nPorts(PortType portType) const final
    {
        switch (portType)
        {
        case PortType::In:
            return 1U;
        case PortType::Out:
            break;
        }

        return 0U;
    }

    /**
     * @brief Returns the data types provided by this class
     * 
     * @param[in] portType Specifies the port type
     * @param[in] portIndex Port index on which the data is sent
     * @retval constexpr std::string_view The data type
     */
    [[nodiscard]] constexpr std::string_view dataType(PortType portType, uint8_t portIndex) const final
    {
        switch (portType)
        {
        case PortType::In:
            if (portIndex == 0)
            {
                return VectorNavObs().type();
            }
        case PortType::Out:
            break;
        }

        return std::string_view("");
    }

    /**
     * @brief Handles the data sent on the input port
     * 
     * @param[in] portIndex The input port index
     * @param[in, out] data The data send on the input port
     */
    void handleInputData(uint8_t portIndex, std::shared_ptr<NodeData> data) final
    {
        if (portIndex == 0)
        {
            auto obs = std::static_pointer_cast<VectorNavObs>(data);
            writeObservation(obs);
        }
    }

  private:
    /**
     * @brief Write Observation to the file
     * 
     * @param[in] obs The received observation
     */
    void writeObservation(std::shared_ptr<VectorNavObs>& obs);
};

} // namespace NAV
