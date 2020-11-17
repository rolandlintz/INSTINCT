/// @file InsMath.hpp
/// @brief
/// @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
/// @date 2020-09-23

#pragma once

#include "InsConstants.hpp"

#include "LinearAlgebra.hpp"

namespace NAV
{
/// @brief Measure the distance between two points
/// @param[in] lat1 Latitude of first point in [rad]
/// @param[in] lon1 Longitude of first point in [rad]
/// @param[in] lat2 Latitude of second point in [rad]
/// @param[in] lon2 Longitude of second point in [rad]
/// @return The distance in [m]
///
/// @note See Haversine Formula (https://www.movable-type.co.uk/scripts/latlong.html)
double measureDistance(double lat1, double lon1, double lat2, double lon2);

/// @brief Calculates the roll angle from a static acceleration measurement
/// @param[in] accel_b Acceleration measurement in static condition in [m/s^2]
/// @return The roll angle in [rad]
///
/// @note See E.-H. Shin (2005) - Estimation Techniques for Low-Cost Inertial Navigation (Chapter 2.6)
double rollFromStaticAccelerationObs(const NAV::Vector3d<NAV::Body>& accel_b);

/// @brief Calculates the pitch angle from a static acceleration measurement
/// @param[in] accel_b Acceleration measurement in static condition in [m/s^2]
/// @return The pitch angle in [rad]
///
/// @note See E.-H. Shin (2005) - Estimation Techniques for Low-Cost Inertial Navigation (Chapter 2.6)
double pitchFromStaticAccelerationObs(const NAV::Vector3d<NAV::Body>& accel_b);

} // namespace NAV