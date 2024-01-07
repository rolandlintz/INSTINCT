// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file EspressifObs.hpp
/// @brief Espressif Observation Class
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date TODO

#pragma once

#include "NodeData/NodeData.hpp"

namespace NAV
{
/// Espressif Observation Class
class EspressifObs : public NodeData
{
  public:
    /// @brief Returns the type of the data class
    /// @return The data type
    [[nodiscard]] static std::string type()
    {
        return "EspressifObs";
    }

    /// @brief Returns the parent types of the data class
    /// @return The parent data types
    [[nodiscard]] static std::vector<std::string> parentTypes()
    {
        return { NodeData::type() };
    }

    /// Payload length in bytes
    uint16_t payloadLength = 0;

    struct FtmObs
    {
        std::array<uint8_t, 6> macAddress;
        double measuredDistance;
    };

    std::vector<FtmObs> data;
};

} // namespace NAV
