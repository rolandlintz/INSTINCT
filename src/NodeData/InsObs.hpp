/// @file InsObs.hpp
/// @brief Parent Class for all Observations
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-03-11

#pragma once

#include <optional>

#include "NodeData.hpp"

#include "util/InsTime.hpp"

namespace NAV
{
/// Parent storage Class for all Observations
class InsObs : public NodeData
{
  public:
    /// @brief Returns the type of the data class
    /// @return The data type
    [[nodiscard]] static std::string type()
    {
        return std::string("InsObs");
    }

    /// @brief Returns the parent types of the data class
    /// @return The parent data types
    [[nodiscard]] static std::vector<std::string> parentTypes()
    {
        return {};
    }

    /// Time at which the message was received
    std::optional<InsTime> insTime;
};

} // namespace NAV
