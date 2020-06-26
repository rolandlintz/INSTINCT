/**
 * @file ExampleNode.hpp
 * @brief Example for new Node creation
 * @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
 * @date 2020-05-18
 */

#pragma once

#include "Nodes/Node.hpp"

#include "NodeData/InsObs.hpp"

namespace NAV
{
class ExampleNode : public Node
{
  public:
    /**
     * @brief Construct a new object
     * 
     * @param[in] name Name of the object
     * @param[in] options Program options string map
     */
    ExampleNode(const std::string& name, const std::map<std::string, std::string>& options);

    ExampleNode() = default;                             ///< Default Constructor
    ~ExampleNode() override = default;                   ///< Destructor
    ExampleNode(const ExampleNode&) = delete;            ///< Copy constructor
    ExampleNode(ExampleNode&&) = delete;                 ///< Move constructor
    ExampleNode& operator=(const ExampleNode&) = delete; ///< Copy assignment operator
    ExampleNode& operator=(ExampleNode&&) = delete;      ///< Move assignment operator

    /**
     * @brief Returns the String representation of the Class Type
     * 
     * @retval constexpr std::string_view The class type
     */
    [[nodiscard]] constexpr std::string_view type() const override
    {
        return std::string_view("ExampleNode");
    }

    /**
     * @brief Returns the String representation of the Class Category
     * 
     * @retval constexpr std::string_view The class category
     */
    [[nodiscard]] constexpr std::string_view category() const override
    {
        return std::string_view("Example Category");
    }

    /**
     * @brief Returns Gui Configuration options for the class
     * 
     * @retval std::vector<ConfigOptions> The gui configuration
     */
    [[nodiscard]] std::vector<ConfigOptions> guiConfig() const override
    {
        return {};
    }

    /**
     * @brief Returns the context of the class
     * 
     * @retval constexpr std::string_view The class context
     */
    [[nodiscard]] constexpr NodeContext context() const override
    {
        return NodeContext::ALL;
    }

    /**
     * @brief Returns the number of Ports
     * 
     * @param[in] portType Specifies the port type
     * @retval constexpr uint8_t The number of ports
     */
    [[nodiscard]] constexpr uint8_t nPorts(PortType portType) const override
    {
        switch (portType)
        {
        case PortType::In:
            return 1U;
        case PortType::Out:
            return 1U;
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
    [[nodiscard]] constexpr std::string_view dataType(PortType portType, uint8_t portIndex) const override
    {
        switch (portType)
        {
        case PortType::In:
            if (portIndex == 0)
            {
                return InsObs().type();
            }
            break;
        case PortType::Out:
            if (portIndex == 0)
            {
                return InsObs().type();
            }
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
    void handleInputData(uint8_t portIndex, std::shared_ptr<NodeData> data) override
    {
        if (portIndex == 0)
        {
            auto obs = std::static_pointer_cast<InsObs>(data);
            processObservation(obs);
        }
    }

    /**
     * @brief Requests the node to send out its data
     * 
     * @param[in] portIndex The output port index
     * @retval std::shared_ptr<NodeData> The requested data or nullptr if no data available
     */
    [[nodiscard]] std::shared_ptr<NodeData> requestOutputData(uint8_t /* portIndex */) override { return nullptr; }

    /**
     * @brief Requests the node to peek its output data
     * 
     * @param[in] portIndex The output port index
     * @retval std::shared_ptr<NodeData> The requested data or nullptr if no data available
     */
    [[nodiscard]] std::shared_ptr<NodeData> requestOutputDataPeek(uint8_t /* portIndex */) override { return nullptr; }

  private:
    /**
     * @brief Process the Observation data and invoke callbacks
     * 
     * @param[in] obs Observation to process
     */
    void processObservation(std::shared_ptr<InsObs>& obs);
};

} // namespace NAV