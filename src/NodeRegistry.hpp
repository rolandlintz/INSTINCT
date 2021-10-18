/// @file NodeRegistry.hpp
/// @brief Utility class which specifies available nodes
/// @author T. Topp (thomas@topp.cc)
/// @date 2020-12-20

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>

#include "internal/Node/Pin.hpp"

namespace NAV
{
class Node;

namespace NodeRegistry
{
/// @brief Holds info of the pins of registered nodes
struct PinInfo
{
    /// @brief Constructor
    /// @param[in] kind Kind of the pin
    /// @param[in] type Type of the pin
    /// @param[in] dataIdentifier Identifiers supplied by the pin
    PinInfo(const Pin::Kind& kind, const Pin::Type& type, std::vector<std::string> dataIdentifier)
        : kind(kind), type(type), dataIdentifier(std::move(dataIdentifier)) {}

    /// Kind of the Pin (Input/Output)
    Pin::Kind kind = Pin::Kind::None;
    /// Type of the Pin
    Pin::Type type = Pin::Type::None;
    /// One or multiple Data Identifiers (Unique name which is used for data flows)
    std::vector<std::string> dataIdentifier;
};

/// @brief Holds information for registered nodes
struct NodeInfo
{
    /// Constructor
    std::function<Node*()> constructor;
    /// Class Type of the node
    std::string type;
    /// List of port data types
    std::vector<PinInfo> pinInfoList;

    /// @brief Checks if the node has a pin which can be linked
    /// @param[in] pin Pin to link to
    [[nodiscard]] bool hasCompatiblePin(const Pin* pin) const;
};

/// @brief Reference to List of all registered Nodes
const std::map<std::string, std::vector<NodeInfo>>& RegisteredNodes();

/// @brief Checks if any of the provided child types is a child of any of the provided parent types
/// @param[in] childTypes Child types to check parentship
/// @param[in] parentTypes Parent types to check parentship
/// @return True if any of the child types is a child of any of the parent types
bool NodeDataTypeAnyIsChildOf(const std::vector<std::string>& childTypes, const std::vector<std::string>& parentTypes);

/// @brief Get the Parent Node Data Types of the specified Node Data Type
/// @param[in] type The Child Node Data Type
std::vector<std::string> GetParentNodeDataTypes(const std::string& type);

/// @brief Register all available Node types for the program
void RegisterNodeTypes();

/// @brief Register all available NodeData types for the program
void RegisterNodeDataTypes();

} // namespace NodeRegistry

} // namespace NAV