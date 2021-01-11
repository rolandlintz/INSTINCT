/// @file KvhUtilities.hpp
/// @brief Helper Functions to work with Kvh Sensors
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-07-28

#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "NodeData/IMU/KvhObs.hpp"

namespace NAV::sensors::kvh
{
/// @brief Calculates the checksum of the provided rawData vector
/// @param[in, out] rawData Vector of the raw data including header and checksum
/// @return The calculated checksum
uint32_t ui32CalcImuCRC(const std::vector<uint8_t>& rawData);

/// @brief Decrypts the provided Kvh observation
///
/// @param[in, out] obs Kvh Observation to decrypt
void decryptKvhObs(std::shared_ptr<NAV::KvhObs>& obs);

} // namespace NAV::sensors::kvh
