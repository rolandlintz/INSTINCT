#include "ImuIntegrator.hpp"

#include "util/Logger.hpp"

#include "util/InsMechanization.hpp"

NAV::ImuIntegrator::ImuIntegrator(const std::string& name, [[maybe_unused]] const std::map<std::string, std::string>& options)
    : Node(name)
{
}

void NAV::ImuIntegrator::integrateObservation(std::shared_ptr<NAV::ImuObs>& obs)
{
    // Get the current state data
    const auto& sourceNode = incomingLinks[1].first.lock();
    auto& sourcePortIndex = incomingLinks[1].second;
    auto currentStateData = std::static_pointer_cast<StateData>(sourceNode->requestOutputData(sourcePortIndex));

    // Fill if empty
    if (prevObs.empty())
    {
        prevObs.push_back(obs);
        prevObs.push_back(obs);
        prevStates.push_back(currentStateData);
        prevStates.push_back(currentStateData);
    }

    /// tₖ₋₂ Time at prior to previous epoch
    const auto& time__t2 = prevObs.at(1)->insTime.value();
    /// tₖ₋₁ Time at previous epoch
    const auto& time__t1 = prevObs.at(0)->insTime.value();
    /// tₖ Current Time
    const auto& time__t0 = obs->insTime.value();

    /// ω_ip_p (tₖ₋₁) Rotation rate in [rad/s],
    /// of the inertial to platform system, in platform coordinates, at the time tₖ₋₁
    const auto& rotationRate_ip__t1 = prevObs.at(0)->gyroUncompXYZ.value();
    /// ω_ip_p (tₖ) Rotation rate in [rad/s],
    /// of the inertial to platform system, in platform coordinates, at the time tₖ
    const auto& rotationRate_ip__t0 = obs->gyroUncompXYZ.value();

    // Δtₖ₋₁ = (tₖ₋₁ - tₖ₋₂) Time difference in [seconds]
    auto timeDifferenceSec__t1 = (time__t1 - time__t2).count();
    // Δtₖ = (tₖ - tₖ₋₁) Time difference in [seconds]
    auto timeDifferenceSec__t0 = (time__t0 - time__t1).count();

    /// q (tₖ₋₂) Quaternion, from platform to earth coordinates, at the time tₖ₋₂
    Eigen::Quaterniond quaternion_p2e__t2 = prevStates.at(1)->quaternion_p2e();
    /// q (tₖ₋₁) Quaternion, from platform to earth coordinates, at the time tₖ₋₁
    Eigen::Quaterniond quaternion_p2e__t1 = prevStates.at(0)->quaternion_p2e();

    // TODO: Determine Earth Rotation
    /// ω_ie_e (tₖ) Rotation rate in [rad/s], of the inertial to earth system, in earth coordinates, at the time tₖ
    Eigen::Vector3d rotationRate_ie__t0 = Eigen::Vector3d::Zero();

    // q (tₖ) Quaternion, from platform to earth coordinates, at the current time tₖ
    Eigen::Quaterniond quaternion_p2e__t0 = updateQuaternionsRungeKutta3(timeDifferenceSec__t0, timeDifferenceSec__t1,
                                                                         rotationRate_ip__t0, rotationRate_ip__t1,
                                                                         rotationRate_ie__t0,
                                                                         quaternion_p2e__t1, quaternion_p2e__t2);

    LOG_INFO("quaternion_p2e__t0: {}", quaternion_p2e__t0.coeffs());

    // Rotate Data
    prevObs.pop_back();
    prevStates.pop_back();
    prevObs.push_front(obs);
    prevStates.push_front(currentStateData);

    invokeCallbacks(obs);
}