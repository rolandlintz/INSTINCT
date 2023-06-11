// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file RealTimeKinematic.hpp
/// @brief Real-Time Kinematic (RTK) carrier-phase DGNSS
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2023-04-13

#pragma once

#include "util/Eigen.hpp"
#include <vector>

#include "internal/Node/Node.hpp"
#include "Navigation/GNSS/Core/Frequency.hpp"
#include "Navigation/GNSS/Core/Code.hpp"
#include "Navigation/GNSS/Core/SatelliteIdentifier.hpp"
#include "Navigation/GNSS/Core/ReceiverClock.hpp"
#include "Navigation/GNSS/Satellite/internal/SatNavData.hpp"
#include "Navigation/Atmosphere/Ionosphere/Ionosphere.hpp"
#include "Navigation/Atmosphere/Troposphere/Troposphere.hpp"
#include "Navigation/Transformations/Units.hpp"
#include "Navigation/Math/KalmanFilter.hpp"

#include "NodeData/GNSS/RtkSolution.hpp"
#include "NodeData/GNSS/GnssObs.hpp"
#include "NodeData/GNSS/GnssNavInfo.hpp"

namespace NAV
{
/// @brief Numerically integrates Imu data
class RealTimeKinematic : public Node
{
  public:
    /// @brief Default constructor
    RealTimeKinematic();
    /// @brief Destructor
    ~RealTimeKinematic() override;
    /// @brief Copy constructor
    RealTimeKinematic(const RealTimeKinematic&) = delete;
    /// @brief Move constructor
    RealTimeKinematic(RealTimeKinematic&&) = delete;
    /// @brief Copy assignment operator
    RealTimeKinematic& operator=(const RealTimeKinematic&) = delete;
    /// @brief Move assignment operator
    RealTimeKinematic& operator=(RealTimeKinematic&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

    /// @brief ImGui config window which is shown on double click
    /// @attention Don't forget to set _hasConfig to true in the constructor of the node
    void guiConfig() override;

    /// @brief Saves the node into a json object
    [[nodiscard]] json save() const override;

    /// @brief Restores the node from a json object
    /// @param[in] j Json object with the node state
    void restore(const json& j) override;

  private:
    constexpr static size_t INPUT_PORT_INDEX_BASE_POS = 0;       ///< @brief Pos
    constexpr static size_t INPUT_PORT_INDEX_BASE_GNSS_OBS = 1;  ///< @brief GnssObs
    constexpr static size_t INPUT_PORT_INDEX_ROVER_GNSS_OBS = 2; ///< @brief GnssObs
    constexpr static size_t INPUT_PORT_INDEX_GNSS_NAV_INFO = 3;  ///< @brief GnssNavInfo

    constexpr static size_t OUTPUT_PORT_INDEX_RTKSOL = 0; ///< @brief Flow (RtkSol)

    // --------------------------------------------------------------- Gui -----------------------------------------------------------------

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// Index of the Pin currently being dragged
    int _dragAndDropPinIndex = -1;
    /// Amount of NavInfo input pins
    size_t _nNavInfoPins = 1;
    /// @brief Adds/Deletes Input Pins depending on the variable _nNavInfoPins
    void updateNumberOfInputPins();

    /// Frequencies used for calculation (GUI filter)
    Frequency _filterFreq = G01;
    /// Codes used for calculation (GUI filter)
    Code _filterCode = Code_ALL;
    /// List of satellites to exclude
    std::vector<SatId> _excludedSatellites;
    /// Elevation cut-off angle for satellites in [rad]
    double _elevationMask = static_cast<double>(15.0_deg);

    /// Ionosphere Model used for the calculation
    IonosphereModel _ionosphereModel = IonosphereModel::Klobuchar;

    /// Troposphere Models used for the calculation
    TroposphereModelSelection _troposphereModels;

    // #####################################################################################

    /// Possible Units for the Standard deviation of the acceleration due to user motion
    enum class StdevAccelUnits
    {
        m_sqrts3, ///< [ m / √(s^3) ]
    };
    /// Gui selection for the Unit of the input stdev_accel parameter for the StDev due to acceleration due to user motion
    StdevAccelUnits _stdevAccelUnits = StdevAccelUnits::m_sqrts3;

    /// @brief 𝜎_a Standard deviation of the acceleration due to user motion in horizontal and vertical component
    /// @note See Groves (2013) eq. (9.156)
    std::array<double, 2> _stdev_accel = { { 3.0, 1.5 } } /* [ m / √(s^3) ] */;

    // ------------------------------------------------------------ Algorithm --------------------------------------------------------------

    /// Measured and calculated data per satellite
    struct SatData
    {
        /// @brief Constructor
        /// @param satId Satellite Identifier
        /// @param navData Satellite Navigation data
        /// @param e_satPos Satellite position in e frame
        /// @param lla_satPos Satellite position in lla frame
        /// @param e_satVel Satellite velocity in e frame
        /// @param e_roverPos Rover receiver position in e frame
        /// @param e_basePos Base receiver position in e frame
        SatData(const SatId& satId, std::shared_ptr<SatNavData> navData,
                const Eigen::Vector3d& e_satPos, const Eigen::Vector3d& lla_satPos, Eigen::Vector3d e_satVel,
                const Eigen::Vector3d& e_roverPos, const Eigen::Vector3d& e_basePos)
            : satId(satId), navData(std::move(navData)), e_satPos(e_satPos), lla_satPos(lla_satPos), e_satVel(std::move(e_satVel)), rover(e_satPos, lla_satPos, e_roverPos), base(e_satPos, lla_satPos, e_basePos) {}

        SatId satId;                                   ///< Satellite Identifier
        std::shared_ptr<SatNavData> navData = nullptr; ///< Satellite Navigation data

        Eigen::Vector3d e_satPos;   ///< Satellite position in ECEF frame coordinates [m]
        Eigen::Vector3d lla_satPos; ///< Satellite position in LLA frame coordinates [rad, rad, m]
        Eigen::Vector3d e_satVel;   ///< Satellite velocity in ECEF frame coordinates [m/s]

        /// Receiver specific data
        struct ReceiverSpecificData
        {
            /// @brief Constructor
            /// @param[in] e_satPos Satellite position in e frame
            /// @param[in] lla_satPos Satellite position in lla frame
            /// @param[in] e_recPos Receiver position in e frame
            ReceiverSpecificData(const Eigen::Vector3d& e_satPos,
                                 const Eigen::Vector3d& lla_satPos,
                                 const Eigen::Vector3d& e_recPos);

            Eigen::Vector3d e_lineOfSightUnitVector; ///< Line-of-sight unit vector in ECEF frame coordinates
            double satElevation = 0.0;               ///< Satellite Elevation [rad]
            double satAzimuth = 0.0;                 ///< Satellite Azimuth [rad]
        };

        ReceiverSpecificData rover; ///< Satellite data concerning the rover
        ReceiverSpecificData base;  ///< Satellite data concerning the base

        /// Data calculated for each observation
        struct Signal
        {
            /// @brief Constructor
            /// @param gnssObsRover GNSS observation of the rover
            /// @param obsIdxRover GNSS observation data index of the rover
            /// @param gnssObsBase GNSS observation of the base
            /// @param obsIdxBase GNSS observation data index of the base
            Signal(const std::shared_ptr<const GnssObs>& gnssObsRover, size_t obsIdxRover,
                   const std::shared_ptr<const GnssObs>& gnssObsBase, size_t obsIdxBase)
                : obsRover({ gnssObsRover, obsIdxRover }), obsBase({ gnssObsBase, obsIdxBase }) {}

            double doubleDifferenceMeasurementPseudorange = 0.0;  ///< Double difference of the pseudorange measurement
            double doubleDifferenceMeasurementCarrierPhase = 0.0; ///< Double difference of the carrier-phase measurement

            double doubleDifferenceEstimatePseudorange = 0.0;  ///< Double difference estimate of the pseudorange
            double doubleDifferenceEstimateCarrierPhase = 0.0; ///< Double difference estimate of the carrier-phase

            /// Receiver specific data
            struct Observation
            {
                std::shared_ptr<const GnssObs> gnssObs = nullptr; ///< GNSS observation
                size_t obsIdx = 0;                                ///< Gnss observation data index

                /// @brief Returns the observation data
                [[nodiscard]] const GnssObs::ObservationData& obs() const
                {
                    return gnssObs->data.at(obsIdx);
                }
            };
            Observation obsRover; ///< Rover observation
            Observation obsBase;  ///< Base observation
        };

        /// @brief Measurements and calculations concerning each signal received by this satellite
        std::unordered_map<Frequency, Signal> signals;
    };

    /// Base position in ECEF frame [m]
    Eigen::Vector3d _e_basePosition = Eigen::Vector3d::Zero();

    /// Estimated position in ECEF frame [m]
    Eigen::Vector3d _e_roverPosition = Eigen::Vector3d::Zero();
    /// Estimated position in LLA frame [rad, rad, m]
    Eigen::Vector3d _lla_roverPosition = Eigen::Vector3d::Zero();
    /// Estimated velocity in ECEF frame [m/s]
    Eigen::Vector3d _e_roverVelocity = Eigen::Vector3d::Zero();

    /// @brief Pivot Satellite information
    struct PivotSatellite
    {
        bool reevaluate = false; ///< Flag whether to reevaluate the pivot satellite in the current epoch
        SatData satData;         ///< Satellite Data
    };

    /// Pivot satellites for each constellation
    std::map<SatelliteSystem, PivotSatellite> _pivotSatellites;

    /// Kalman Filter representation
    KalmanFilter _kalmanFilter{ 8, 8 };

    /// Latest GNSS Observation from the base station
    std::shared_ptr<const GnssObs> _gnssObsBase = nullptr;
    /// Latest GNSS Observation from the rover
    std::shared_ptr<const GnssObs> _gnssObsRover = nullptr;

    /// @brief Receive Function for the Base Position
    /// @param[in] queue Queue with all the received data messages
    /// @param[in] pinIdx Index of the pin the data is received on
    void recvBasePos(InputPin::NodeDataQueue& queue, size_t pinIdx);

    /// @brief Receive Function for the Base GNSS Observations
    /// @param[in] queue Queue with all the received data messages
    /// @param[in] pinIdx Index of the pin the data is received on
    void recvBaseGnssObs(InputPin::NodeDataQueue& queue, size_t pinIdx);

    /// @brief Receive Function for the RoverGNSS Observations
    /// @param[in] queue Queue with all the received data messages
    /// @param[in] pinIdx Index of the pin the data is received on
    void recvRoverGnssObs(InputPin::NodeDataQueue& queue, size_t pinIdx);

    /// @brief Calculates the RTK solution
    void calcRealTimeKinematicSolution();

    /// @brief Calculates a SPP solution as fallback in case no base data is available
    std::shared_ptr<RtkSolution> calcFallbackSppSolution();

    /// @brief Returns a list of observations used for calculation of RTK (only satellites filtered by GUI filter & NAV data available & ...)
    /// @param[in] gnssNavInfos Collection of all connected navigation data providers
    std::vector<SatData> selectSatellitesForCalculation(const std::vector<const GnssNavInfo*>& gnssNavInfos);

    /// @brief Update the pivot satellites for each constellation
    /// @param satelliteData List of GNSS observation data used for the calculation of this epoch
    void updatePivotSatellites(const std::vector<SatData>& satelliteData);

    /// @brief Calculates the measured double differences for each satellite
    /// @param satelliteData List of GNSS observation data used for the calculation of this epoch
    /// @return The amount of double differences calculated
    size_t calculateMeasurementDoubleDifferences(std::vector<SatData>& satelliteData);

    /// @brief Calculates the estimated double differences for each satellite
    /// @param satelliteData List of GNSS observation data used for the calculation of this epoch
    /// @param ionosphericCorrections Ionospheric correction parameters collected from the Nav data
    void calculateEstimatedDoubleDifferences(std::vector<SatData>& satelliteData, const IonosphericCorrections& ionosphericCorrections);
};

} // namespace NAV