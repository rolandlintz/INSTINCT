/// @file ImuIntegrator.hpp
/// @brief Integrates ImuObs Data
/// @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
/// @date 2020-05-18

#pragma once

#include "Nodes/Node.hpp"

#include "NodeData/IMU/ImuObs.hpp"
#include "NodeData/State/StateData.hpp"
#include "NodeData/IMU/ImuPos.hpp"

#include <deque>

namespace NAV
{
class ImuIntegrator : public Node
{
  public:
    /// @brief Constructor
    /// @param[in] name Name of the object
    /// @param[in] options Program options string map
    ImuIntegrator(const std::string& name, const std::map<std::string, std::string>& options);

    /// @brief Default constructor
    ImuIntegrator() = default;
    /// @brief Destructor
    ~ImuIntegrator() override = default;
    /// @brief Copy constructor
    ImuIntegrator(const ImuIntegrator&) = delete;
    /// @brief Move constructor
    ImuIntegrator(ImuIntegrator&&) = delete;
    /// @brief Copy assignment operator
    ImuIntegrator& operator=(const ImuIntegrator&) = delete;
    /// @brief Move assignment operator
    ImuIntegrator& operator=(ImuIntegrator&&) = delete;

    /// @brief Returns the String representation of the Class Type
    /// @return The class type
    [[nodiscard]] constexpr std::string_view type() const override
    {
        return std::string_view("ImuIntegrator");
    }

    /// @brief Returns the String representation of the Class Category
    /// @return The class category
    [[nodiscard]] constexpr std::string_view category() const override
    {
        return std::string_view("Integrator");
    }

    /// @brief Returns Gui Configuration options for the class
    /// @return The gui configuration
    [[nodiscard]] std::vector<ConfigOptions> guiConfig() const override
    {
        return { { CONFIG_LIST, "Integration Frame", "", { "[ECEF]", "NED" } } };
    }

    /// @brief Returns the context of the class
    /// @return The class context
    [[nodiscard]] constexpr NodeContext context() const override
    {
        return NodeContext::ALL;
    }

    /// @brief Returns the number of Ports
    /// @param[in] portType Specifies the port type
    /// @return The number of ports
    [[nodiscard]] constexpr uint8_t nPorts(PortType portType) const override
    {
        switch (portType)
        {
        case PortType::In:
            return 3U;
        case PortType::Out:
            return 1U;
        }

        return 0U;
    }

    /// @brief Returns the data types provided by this class
    /// @param[in] portType Specifies the port type
    /// @param[in] portIndex Port index on which the data is sent
    /// @return The data type
    [[nodiscard]] constexpr std::string_view dataType(PortType portType, uint8_t portIndex) const override
    {
        switch (portType)
        {
        case PortType::In:
            if (portIndex == 0)
            {
                return ImuObs().type();
            }
            if (portIndex == 1)
            {
                return ImuPos().type();
            }
            if (portIndex == 2)
            {
                return StateData().type();
            }
            break;
        case PortType::Out:
            if (portIndex == 0)
            {
                return StateData().type();
            }
            break;
        }

        return std::string_view("");
    }

    /// @brief Handles the data sent on the input port
    /// @param[in] portIndex The input port index
    /// @param[in, out] data The data send on the input port
    void handleInputData(uint8_t portIndex, std::shared_ptr<NodeData> data) override
    {
        if (portIndex == 0)
        {
            auto obs = std::static_pointer_cast<ImuObs>(data);
            integrateObservation(obs);
        }
    }

  private:
    /// @brief Integrates the Imu Observation data
    /// @param[in] imuObs__t0 ImuObs to process
    void integrateObservation(std::shared_ptr<ImuObs>& imuObs__t0);

    /// @brief Storage class for previous observations.
    ///
    /// [0]: Observation at time tₖ₋₁
    /// [1]: Observation at time tₖ₋₂
    std::deque<std::shared_ptr<ImuObs>> prevObs;

    /// @brief Storage class for previous states.
    ///
    /// [0]: StateData at time tₖ₋₁
    /// [1]: StateData at time tₖ₋₂
    std::deque<std::shared_ptr<StateData>> prevStates;

    enum IntegrationFrame
    {
        ECEF,
        NED
    };
    IntegrationFrame integrationFrame = IntegrationFrame::ECEF;
};

} // namespace NAV
