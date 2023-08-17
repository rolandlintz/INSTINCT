#include "SppAlgorithm.hpp"

#include "Navigation/GNSS/Positioning/SPP/SppCommon.hpp"
#include "Navigation/GNSS/Positioning/SPP/SppLeastSquares.hpp"

#include "Navigation/Transformations/Units.hpp"
#include "internal/gui/widgets/EnumCombo.hpp"

#include <set>

namespace NAV
{

const char* to_string(GNSS::Positioning::SPP::EstimatorType estimatorType)
{
    using namespace GNSS::Positioning::SPP; // NOLINT(google-build-using-namespace)
    switch (estimatorType)
    {
    case EstimatorType::LEAST_SQUARES:
        return "Least Squares";
    case EstimatorType::WEIGHTED_LEAST_SQUARES:
        return "Weighted Least Squares";
    case EstimatorType::KF:
        return "Kalman Filter";
    case EstimatorType::COUNT:
        break;
    }
    return "";
}

namespace GNSS::Positioning::SPP
{

bool ComboSppEstimatorType(const char* label, EstimatorType& estimatorType)
{
    return gui::widgets::EnumCombo(label, estimatorType);
}

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
                                                double elevationMask)
{
    INS_ASSERT_USER_ERROR(estimatorType == EstimatorType::LEAST_SQUARES || estimatorType == EstimatorType::WEIGHTED_LEAST_SQUARES,
                          "This function only works for the estimator types LSE or WLSE.");

    // Collection of all connected Ionospheric Corrections
    IonosphericCorrections ionosphericCorrections(gnssNavInfos);

    auto sppSol = std::make_shared<SppSolution>();
    sppSol->insTime = gnssObs->insTime;

    // Data calculated for each satellite (only satellites filtered by GUI filter & NAV data available)
    std::vector<CalcData> calcData = selectObservations(sppSol, gnssObs, gnssNavInfos, filterFreq, filterCode, excludedSatellites);
    // List of satellite systems
    std::set<SatelliteSystem> availSatelliteSystems;
    for (const auto& calc : calcData) { availSatelliteSystems.insert(calc.obsData.satSigId.toSatId().satSys); }

    size_t nMeasPsr = calcData.size();
    LOG_DATA("nMeasPsr {}", nMeasPsr);
    size_t nParam = 4 + availSatelliteSystems.size() - 1; // 3x pos, 1x clk, (N-1)x clkDiff
    LOG_DATA("nParam {}", nParam);

    // Find all observations providing a doppler measurement (for velocity calculation)
    size_t nDopplerMeas = findDopplerMeasurements(calcData);
    LOG_DATA("nDopplerMeas {}", nDopplerMeas);

    // #####################################################################################################################################
    //                                                          Calculation
    // #####################################################################################################################################

    if (nMeasPsr < nParam)
    {
        LOG_ERROR("[{}] Cannot calculate position because only {} valid measurements ({} needed). Try changing filter settings or reposition your antenna.",
                  (gnssObs->insTime + std::chrono::seconds(gnssObs->insTime.leapGps2UTC())), nMeasPsr, nParam);
        sppSol->nSatellitesPosition = nMeasPsr;
        sppSol->nSatellitesVelocity = nDopplerMeas;
        return sppSol;
    }

    for (size_t o = 0; o < 10; o++)
    {
        LOG_DATA("Iteration {}", o);
        // Latitude, Longitude, Altitude of the receiver [rad, rad, m]
        Eigen::Vector3d lla_pos = trafo::ecef2lla_WGS84(state.e_position);
        LOG_DATA("     e_position {}, {}, {}", state.e_position.x(), state.e_position.y(), state.e_position.z());
        LOG_DATA("     lla_pos {}°, {}°, {}m", rad2deg(lla_pos.x()), rad2deg(lla_pos.y()), lla_pos.z());
        LOG_DATA("     recvClk.bias {}", state.recvClk.bias.value);
        LOG_DATA("     recvClk.drift {}", state.recvClk.drift.value);

        std::vector<SatelliteSystem> satelliteSystems; // Make a copy of available systems, which we can edit every iteration
        satelliteSystems.reserve(availSatelliteSystems.size());
        std::copy(availSatelliteSystems.begin(), availSatelliteSystems.end(), std::back_inserter(satelliteSystems));

        // Keeps track of skipped meausrements (because of elevation mask, ...)
        size_t cntSkippedMeas = 0;
        if (!calcDataBasedOnEstimates(sppSol, satelliteSystems, cntSkippedMeas, calcData, state,
                                      nParam, nMeasPsr, nDopplerMeas, gnssObs->insTime, lla_pos, elevationMask))
        {
            return sppSol;
        }

        // Update the amount of measurements to take skipped measurements because of elevation mask into account
        size_t nMeasPsr_i = nMeasPsr - cntSkippedMeas;
        size_t nDopplerMeas_i = nDopplerMeas - cntSkippedMeas;

        auto [e_H_psr,     // Measurement/Geometry matrix for the pseudorange
              psrEst,      // Pseudorange estimates [m]
              psrMeas,     // Pseudorange measurements [m]
              W_psr,       // Pseudorange measurement error weight matrix
              dpsr,        // Difference between Pseudorange measurements and estimates
              e_H_r,       // Measurement/Geometry matrix for the pseudorange-rate
              psrRateEst,  // Corrected pseudorange-rate estimates [m/s]
              psrRateMeas, // Corrected pseudorange-rate measurements [m/s]
              W_psrRate,   // Pseudorange rate (doppler) measurement error weight matrix
              dpsr_dot     // Difference between Pseudorange rate measurements and estimates
        ] = calcMeasurementEstimatesAndDesignMatrix(sppSol, calcData,
                                                    static_cast<int>(nParam),
                                                    static_cast<int>(nMeasPsr_i),
                                                    static_cast<int>(nDopplerMeas_i),
                                                    gnssObs->insTime,
                                                    state, lla_pos,
                                                    satelliteSystems,
                                                    ionosphericCorrections, ionosphereModel,
                                                    troposphereModels, gnssMeasurementErrorModel, estimatorType);

        // #################################################################################################################################
        //                                                     Least squares solution
        // #################################################################################################################################

        // ---------------------------------------------------------- Position -------------------------------------------------------------

        bool solAccurate = solveLeastSquaresAndAssignSolution(dpsr, e_H_psr, W_psr, estimatorType, satelliteSystems, nMeasPsr_i, nParam,
                                                              // Outputs:
                                                              state.e_position, state.recvClk.bias, state.recvClk.sysTimeDiff,
                                                              sppSol, sppSol->nSatellitesPosition, sppSol->recvClk.bias, sppSol->recvClk.sysTimeDiff,
                                                              &SppSolution::setPositionAndStdDev_e, &SppSolution::setPosition_e,
                                                              &SppSolution::setPositionClockErrorCovarianceMatrix);

        // ---------------------------------------------------------- Velocity -------------------------------------------------------------
        if (nDopplerMeas_i >= nParam)
        {
            solAccurate &= solveLeastSquaresAndAssignSolution(dpsr_dot, e_H_r, W_psrRate, estimatorType, satelliteSystems, nDopplerMeas_i, nParam,
                                                              // Outputs:
                                                              state.e_velocity, state.recvClk.drift, state.recvClk.sysDriftDiff,
                                                              sppSol, sppSol->nSatellitesVelocity, sppSol->recvClk.drift, sppSol->recvClk.sysDriftDiff,
                                                              &SppSolution::setVelocityAndStdDev_e, &SppSolution::setVelocity_e,
                                                              &SppSolution::setVelocityClockDriftCovarianceMatrix);
        }
        else
        {
            LOG_WARN("[{}] Cannot calculate velocity because only {} valid doppler measurements ({} needed). Try changing filter settings or reposition your antenna.",
                     gnssObs->insTime, nDopplerMeas_i, nParam);
            continue;
        }

        if (solAccurate)
        {
            break;
        }
    }

    return sppSol;
}

} // namespace GNSS::Positioning::SPP

} // namespace NAV