// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file SppAlgorithm.hpp
/// @brief Single Point Positioning (SPP) / Code Phase Positioning
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2023-05-01

#pragma once

#include <memory>
#include <set>

#include "NodeData/GNSS/GnssObs.hpp"
#include "NodeData/GNSS/GnssNavInfo.hpp"
#include "NodeData/GNSS/SppSolution.hpp"

#include "Navigation/Atmosphere/Ionosphere/Ionosphere.hpp"
#include "Navigation/Atmosphere/Troposphere/Troposphere.hpp"
#include "Navigation/GNSS/Core/ReceiverClock.hpp"
#include "Navigation/GNSS/Positioning/SppAlgorithmTypes.hpp"

#include "Navigation/GNSS/Positioning/SPP/SppKeys.hpp"
#include "Navigation/GNSS/Positioning/SPP/SppKalmanFilter.hpp"
#include "Navigation/GNSS/Errors.hpp"

namespace States = NAV::GNSS::Positioning::SPP::States;
namespace Meas = NAV::GNSS::Positioning::SPP::Meas;

namespace NAV::GNSS::Positioning::SPP
{

/// @brief Calculates the SPP solution with a Least squares estimator
/// @param[in] state Previous SPP state
/// @param[in] gnssObs GNSS observation received
/// @param[in] gnssNavInfos Collection of all connected navigation data providers
/// @param[in] ionosphereModel Ionosphere Model used for the calculation
/// @param[in] troposphereModels Troposphere Models used for the calculation
/// @param[in] gnssMeasurementErrorModel GNSS measurement error model to use
/// @param[in] estimatorType Estimation algorithm used
/// @param[in] filterFreq Frequencies used for calculation (GUI filter)
/// @param[in] filterCode Codes used for calculation (GUI filter)
/// @param[in] excludedSatellites List of satellites to exclude
/// @param[in] elevationMask Elevation cut-off angle for satellites in [rad]
/// @param[in] useDoppler Boolean which enables the use of doppler observations
/// @param[in, out] interSysErrs Inter-system clock error keys
/// @param[in, out] interSysDrifts Inter-system clock drift keys
/// @return Shared pointer to the SPP solution
std::shared_ptr<SppSolution> calcSppSolutionLSE(State state,
                                                const std::shared_ptr<const GnssObs>& gnssObs,
                                                const std::vector<const GnssNavInfo*>& gnssNavInfos,
                                                const IonosphereModel& ionosphereModel,
                                                const TroposphereModelSelection& troposphereModels,
                                                const GnssMeasurementErrorModel& gnssMeasurementErrorModel,
                                                const EstimatorType& estimatorType,
                                                const Frequency& filterFreq,
                                                const Code& filterCode,
                                                const std::vector<SatId>& excludedSatellites,
                                                double elevationMask,
                                                bool useDoppler,
                                                std::vector<States::StateKeyTypes>& interSysErrs,
                                                std::vector<States::StateKeyTypes>& interSysDrifts);

/// @brief Calculates the SPP solution with a Kalman Filter
/// @param[in, out] kalmanFilter Spp Kalman Filter with all settings
/// @param[in] gnssObs GNSS observation received
/// @param[in] gnssNavInfos Collection of all connected navigation data providers
/// @param[in] ionosphereModel Ionosphere Model used for the calculation
/// @param[in] troposphereModels Troposphere Models used for the calculation
/// @param[in] gnssMeasurementErrorModel GNSS measurement error model to use
/// @param[in] filterFreq Frequencies used for calculation (GUI filter)
/// @param[in] filterCode Codes used for calculation (GUI filter)
/// @param[in] excludedSatellites List of satellites to exclude
/// @param[in] elevationMask Elevation cut-off angle for satellites in [rad]
/// @param[in] useDoppler Boolean which enables the use of doppler observations
/// @param[in, out] interSysErrs Inter-system clock error keys
/// @param[in, out] interSysDrifts Inter-system clock drift keys
/// @return Shared pointer to the SPP solution
std::shared_ptr<SppSolution> calcSppSolutionKF(SppKalmanFilter& kalmanFilter,
                                               const std::shared_ptr<const GnssObs>& gnssObs,
                                               const std::vector<const GnssNavInfo*>& gnssNavInfos,
                                               const IonosphereModel& ionosphereModel,
                                               const TroposphereModelSelection& troposphereModels,
                                               const GnssMeasurementErrorModel& gnssMeasurementErrorModel,
                                               const Frequency& filterFreq,
                                               const Code& filterCode,
                                               const std::vector<SatId>& excludedSatellites,
                                               double elevationMask,
                                               bool useDoppler,
                                               std::vector<States::StateKeyTypes>& interSysErrs,
                                               std::vector<States::StateKeyTypes>& interSysDrifts);

} // namespace NAV::GNSS::Positioning::SPP