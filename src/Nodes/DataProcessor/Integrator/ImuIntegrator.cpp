#include "ImuIntegrator.hpp"

#include "util/Logger.hpp"

#include "util/InsMechanization.hpp"
#include "util/InsConstants.hpp"
#include "util/InsGravity.hpp"

#include "internal/gui/widgets/HelpMarker.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

NAV::ImuIntegrator::ImuIntegrator()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);

    hasConfig = true;
    guiConfigDefaultWindowSize = { 350, 123 };

    nm::CreateInputPin(this, "ImuObs", Pin::Type::Flow, { NAV::ImuObs::type() }, &ImuIntegrator::recvImuObs__t0);
    nm::CreateInputPin(this, "PosVelAtt", Pin::Type::Flow, { NAV::PosVelAtt::type() }, &ImuIntegrator::recvState__t1);

    nm::CreateOutputPin(this, "PosVelAtt", Pin::Type::Flow, NAV::PosVelAtt::type());
}

NAV::ImuIntegrator::~ImuIntegrator()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::ImuIntegrator::typeStatic()
{
    return "ImuIntegrator";
}

std::string NAV::ImuIntegrator::type() const
{
    return typeStatic();
}

std::string NAV::ImuIntegrator::category()
{
    return "Data Processor";
}

void NAV::ImuIntegrator::guiConfig()
{
    ImGui::SetNextItemWidth(100);
    if (ImGui::Combo(fmt::format("Integration Frame##{}", size_t(id)).c_str(), reinterpret_cast<int*>(&integrationFrame), "ECEF\0NED\0\0"))
    {
        LOG_DEBUG("{}: Integration Frame changed to {}", nameId(), integrationFrame ? "NED" : "ECEF");
        flow::ApplyChanges();
    }
    if (ImGui::Combo(fmt::format("Gravity Model##{}", size_t(id)).c_str(), reinterpret_cast<int*>(&gravityModel), "WGS84\0WGS84_Skydel\0Somigliana\0EGM96\0\0"))
    {
        if (gravityModel == WGS84)
        {
            LOG_DEBUG("{}: Gravity Model changed to {}", nameId(), "WGS84");
        }
        else if (gravityModel == WGS84_Skydel)
        {
            LOG_DEBUG("{}: Gravity Model changed to {}", nameId(), "WGS84_Skydel");
        }
        else if (gravityModel == Somigliana)
        {
            LOG_DEBUG("{}: Gravity Model changed to {}", nameId(), "Somigliana");
        }
        else if (gravityModel == EGM96)
        {
            LOG_DEBUG("{}: Gravity Model changed to {}", nameId(), "EGM96");
        }
        flow::ApplyChanges();
    }

    if (ImGui::Checkbox(fmt::format("Prefere TimeSinceStartup over InsTime##{}", size_t(id)).c_str(), &prefereTimeSinceStartupOverInsTime))
    {
        LOG_DEBUG("{}: prefereTimeSinceStartupOverInsTime changed to {}", nameId(), prefereTimeSinceStartupOverInsTime);
        flow::ApplyChanges();
    }
    ImGui::SameLine();
    gui::widgets::HelpMarker("Takes the IMU internal 'TimeSinceStartup' value instead of the absolute 'insTime'");

    if (ImGui::Checkbox(fmt::format("Apply gravity vector compensation to acceleration##{}", size_t(id)).c_str(), &gravityCompensation))
    {
        LOG_DEBUG("{}: gravityCompensation changed to {}", nameId(), gravityCompensation);
        flow::ApplyChanges();
    }

    if (ImGui::Checkbox(fmt::format("Apply centrifugal acceleration compensation##{}", size_t(id)).c_str(), &centrifugalAccCompensation))
    {
        LOG_DEBUG("{}: centrifugalAccCompensation changed to {}", nameId(), centrifugalAccCompensation);
        flow::ApplyChanges();
    }

    if (ImGui::Checkbox(fmt::format("Apply coriolis acceleration compensation##{}", size_t(id)).c_str(), &coriolisCompensation))
    {
        LOG_DEBUG("{}: coriolisCompensation changed to {}", nameId(), coriolisCompensation);
        flow::ApplyChanges();
    }
}

[[nodiscard]] json NAV::ImuIntegrator::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["integrationFrame"] = integrationFrame;
    j["gravityModel"] = gravityModel;
    j["prefereTimeSinceStartupOverInsTime"] = prefereTimeSinceStartupOverInsTime;
    j["gravityCompensation"] = gravityCompensation;
    j["centrifugalAccCompensation"] = centrifugalAccCompensation;
    j["coriolisCompensation"] = coriolisCompensation;

    return j;
}

void NAV::ImuIntegrator::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("integrationFrame"))
    {
        integrationFrame = static_cast<IntegrationFrame>(j.at("integrationFrame").get<int>());
    }
    if (j.contains("gravityModel"))
    {
        gravityModel = static_cast<GravityModel>(j.at("gravityModel").get<int>());
    }
    if (j.contains("prefereTimeSinceStartupOverInsTime"))
    {
        prefereTimeSinceStartupOverInsTime = j.at("prefereTimeSinceStartupOverInsTime");
    }
    if (j.contains("gravityCompensation"))
    {
        gravityCompensation = j.at("gravityCompensation");
    }
    if (j.contains("centrifugalAccCompensation"))
    {
        centrifugalAccCompensation = j.at("centrifugalAccCompensation");
    }
    if (j.contains("coriolisCompensation"))
    {
        coriolisCompensation = j.at("coriolisCompensation");
    }
}

bool NAV::ImuIntegrator::initialize()
{
    LOG_TRACE("{}: called", nameId());

    // This should be dependant on the integration algorithm
    maxSizeImuObservations = 3;
    maxSizeStates = 2;

    imuObservations.clear();
    posVelAttStates.clear();

    posVelAtt__init = nullptr;

    time__init = InsTime();
    timeSinceStartup__init = 0;

    try
    {
        NAV::gravity::readEGM96Coeffs();
    }
    catch (...)
    {
        return false;
    }

    LOG_DEBUG("ImuIntegrator initialized");

    return true;
}

void NAV::ImuIntegrator::deinitialize()
{
    LOG_TRACE("{}: called", nameId());
}

void NAV::ImuIntegrator::recvImuObs__t0(const std::shared_ptr<NodeData>& nodeData, ax::NodeEditor::LinkId /* linkId */)
{
    auto imuObs = std::dynamic_pointer_cast<ImuObs>(nodeData);

    if (!imuObs->insTime.has_value() && !imuObs->timeSinceStartup.has_value())
    {
        LOG_ERROR("{}: Can't set new imuObs__t0 because the observation has no time tag (insTime/timeSinceStartup)", nameId());
        return;
    }

    // Add imuObs tₖ to the start of the list
    imuObservations.push_front(imuObs);

    // Remove observations at the end of the list till the max size is reached
    while (imuObservations.size() > maxSizeImuObservations)
    {
        LOG_WARN("Receive new Imu observation, but list is full --> discarding oldest observation");
        imuObservations.pop_back();
    }

    // If enough imu observations and states received, integrate the observation
    if (imuObservations.size() == maxSizeImuObservations
        && posVelAttStates.size() == maxSizeStates)
    {
        integrateObservation();
    }
}

void NAV::ImuIntegrator::recvState__t1(const std::shared_ptr<NodeData>& nodeData, ax::NodeEditor::LinkId /* linkId */)
{
    auto posVelAtt = std::dynamic_pointer_cast<PosVelAtt>(nodeData);

    // Add imuObs tₖ₋₁ to the start of the list
    if (posVelAttStates.empty())
    {
        while (posVelAttStates.size() < maxSizeStates)
        {
            posVelAttStates.push_front(posVelAtt);
        }
    }
    else
    {
        posVelAttStates.push_front(posVelAtt);
    }

    // Remove states at the end of the list till the max size is reached
    while (posVelAttStates.size() > maxSizeStates)
    {
        LOG_WARN("Receive new state, but list is full --> discarding oldest state");
        posVelAttStates.pop_back();
    }

    /// Initial State
    if (posVelAtt__init == nullptr)
    {
        posVelAtt__init = posVelAtt;
    }

    // If enough imu observations and states received, integrate the observation
    if (imuObservations.size() == maxSizeImuObservations
        && posVelAttStates.size() == maxSizeStates)
    {
        integrateObservation();
    }
}

void NAV::ImuIntegrator::integrateObservation()
{
    /// IMU Observation at the time tₖ
    std::shared_ptr<ImuObs> imuObs__t0 = imuObservations.at(0);
    /// IMU Observation at the time tₖ₋₁
    std::shared_ptr<ImuObs> imuObs__t1 = imuObservations.at(1);
    /// IMU Observation at the time tₖ₋₂
    std::shared_ptr<ImuObs> imuObs__t2 = imuObservations.at(2);

    /// Position, Velocity and Attitude at the time tₖ₋₁
    std::shared_ptr<PosVelAtt> posVelAtt__t1 = posVelAttStates.at(0);
    /// Position, Velocity and Attitude at the time tₖ₋₂
    std::shared_ptr<PosVelAtt> posVelAtt__t2 = posVelAttStates.at(1);

    // Position and rotation information for conversion of IMU data from platform to body frame
    const auto& imuPosition = imuObs__t0->imuPos;

    /// Result State Data at the time tₖ
    auto posVelAtt__t0 = std::make_shared<PosVelAtt>();

    // Δtₖ₋₁ = (tₖ₋₁ - tₖ₋₂) Time difference in [seconds]
    long double timeDifferenceSec__t1 = 0;
    // Δtₖ = (tₖ - tₖ₋₁) Time difference in [seconds]
    long double timeDifferenceSec__t0 = 0;

    if (imuObs__t0->insTime.has_value() && !(prefereTimeSinceStartupOverInsTime && imuObs__t0->timeSinceStartup.has_value()))
    {
        /// tₖ₋₂ Time at prior to previous epoch
        const InsTime& time__t2 = imuObs__t2->insTime.value();
        /// tₖ₋₁ Time at previous epoch
        const InsTime& time__t1 = imuObs__t1->insTime.value();
        /// tₖ Current Time
        const InsTime& time__t0 = imuObs__t0->insTime.value();

        // Δtₖ₋₁ = (tₖ₋₁ - tₖ₋₂) Time difference in [seconds]
        timeDifferenceSec__t1 = (time__t1 - time__t2).count();
        // Δtₖ = (tₖ - tₖ₋₁) Time difference in [seconds]
        timeDifferenceSec__t0 = (time__t0 - time__t1).count();

        // Update time
        posVelAtt__t0->insTime = imuObs__t0->insTime;

        LOG_DATA("{}: time__t2 {}", nameId(), time__t2.toGPSweekTow());
        LOG_DATA("{}: time__t1 {}; DiffSec__t1 {}", nameId(), time__t1.toGPSweekTow(), timeDifferenceSec__t1);
        LOG_DATA("{}: time__t0 {}: DiffSec__t0 {}", nameId(), time__t0.toGPSweekTow(), timeDifferenceSec__t0);
    }
    else
    {
        /// tₖ₋₂ Time at prior to previous epoch
        const auto& time__t2 = imuObs__t2->timeSinceStartup.value();
        /// tₖ₋₁ Time at previous epoch
        const auto& time__t1 = imuObs__t1->timeSinceStartup.value();
        /// tₖ Current Time
        const auto& time__t0 = imuObs__t0->timeSinceStartup.value();

        // Δtₖ₋₁ = (tₖ₋₁ - tₖ₋₂) Time difference in [seconds]
        timeDifferenceSec__t1 = static_cast<long double>(time__t1 - time__t2) * 1e-9L;
        // Δtₖ = (tₖ - tₖ₋₁) Time difference in [seconds]
        timeDifferenceSec__t0 = static_cast<long double>(time__t0 - time__t1) * 1e-9L;

        if (timeSinceStartup__init == 0)
        {
            timeSinceStartup__init = imuObs__t0->timeSinceStartup.value();
            time__init = imuObs__t0->insTime.has_value() ? imuObs__t0->insTime.value() : InsTime(2000, 1, 1, 1, 1, 1);
        }

        // Update time
        posVelAtt__t0->insTime = time__init + std::chrono::nanoseconds(imuObs__t0->timeSinceStartup.value() - timeSinceStartup__init);

        LOG_DATA("{}: time__t2 {}", nameId(), time__t2);
        LOG_DATA("{}: time__t1 {}; DiffSec__t1 {}", nameId(), time__t1, timeDifferenceSec__t1);
        LOG_DATA("{}: time__t0 {}: DiffSec__t0 {}", nameId(), time__t0, timeDifferenceSec__t0);
    }

    /// ω_ip_p (tₖ₋₁) Angular velocity in [rad/s],
    /// of the inertial to platform system, in platform coordinates, at the time tₖ₋₁
    const Eigen::Vector3d& angularVelocity_ip_p__t1 = imuObs__t1->gyroCompXYZ.has_value()
                                                          ? imuObs__t1->gyroCompXYZ.value()
                                                          : imuObs__t1->gyroUncompXYZ.value();
    // LOG_DEBUG("angularVelocity_ip_p__t1 =\n{}", angularVelocity_ip_p__t1);
    /// ω_ip_p (tₖ) Angular velocity in [rad/s],
    /// of the inertial to platform system, in platform coordinates, at the time tₖ
    const Eigen::Vector3d& angularVelocity_ip_p__t0 = imuObs__t0->gyroCompXYZ.has_value()
                                                          ? imuObs__t0->gyroCompXYZ.value()
                                                          : imuObs__t0->gyroUncompXYZ.value();
    // LOG_DEBUG("angularVelocity_ip_p__t0 =\n{}", angularVelocity_ip_p__t0);

    /// a_p (tₖ₋₁) Acceleration in [m/s^2], in platform coordinates, at the time tₖ₋₁
    const Eigen::Vector3d& acceleration_p__t1 = imuObs__t1->accelCompXYZ.has_value()
                                                    ? imuObs__t1->accelCompXYZ.value()
                                                    : imuObs__t1->accelUncompXYZ.value();
    // LOG_DEBUG("acceleration_p__t1 =\n{}", acceleration_p__t1);
    /// a_p (tₖ) Acceleration in [m/s^2], in platform coordinates, at the time tₖ
    const Eigen::Vector3d& acceleration_p__t0 = imuObs__t0->accelCompXYZ.has_value()
                                                    ? imuObs__t0->accelCompXYZ.value()
                                                    : imuObs__t0->accelUncompXYZ.value();
    // LOG_DEBUG("acceleration_p__t0 =\n{}", acceleration_p__t0);
    /// v_n (tₖ₋₁) Velocity in [m/s], in navigation coordinates, at the time tₖ₋₁
    const Eigen::Vector3d& velocity_n__t1 = posVelAtt__t1->velocity_n();
    // LOG_DEBUG("velocity_n__t1 =\n{}", velocity_n__t1);
    /// v_n (tₖ₋₂) Velocity in [m/s], in navigation coordinates, at the time tₖ₋₂
    const Eigen::Vector3d& velocity_n__t2 = posVelAtt__t2->velocity_n();
    // LOG_DEBUG("velocity_n__t2 =\n{}", velocity_n__t2);
    /// v_e (tₖ₋₂) Velocity in [m/s], in earth coordinates, at the time tₖ₋₂
    const Eigen::Vector3d velocity_e__t2 = posVelAtt__t2->quaternion_en() * velocity_n__t2;
    // LOG_DEBUG("velocity_e__t2 =\n{}", velocity_e__t2);
    /// v_e (tₖ₋₁) Velocity in [m/s], in earth coordinates, at the time tₖ₋₁
    const Eigen::Vector3d velocity_e__t1 = posVelAtt__t1->quaternion_en() * velocity_n__t1;
    // LOG_DEBUG("velocity_e__t1 =\n{}", velocity_e__t1);
    /// x_e (tₖ₋₂) Position in [m], in ECEF coordinates, at the time tₖ₋₂
    const Eigen::Vector3d position_e__t2 = posVelAtt__t2->position_ecef();
    // LOG_DEBUG("position_e__t2 =\n{}", position_e__t2);
    /// x_e (tₖ₋₁) Position in [m], in ECEF coordinates, at the time tₖ₋₁
    const Eigen::Vector3d position_e__t1 = posVelAtt__t1->position_ecef();
    // LOG_DEBUG("position_e__t1 =\n{}", position_e__t1);

    LOG_DATA("{}: Integrating Imu data with accel_p {}, {}, {}", nameId(), acceleration_p__t0.x(), acceleration_p__t0.y(), acceleration_p__t0.z());
    [[maybe_unused]] auto acceleration_b__t0 = imuPosition.quatAccel_bp() * acceleration_p__t0;
    LOG_DATA("{}: Integrating Imu data with accel_b {}, {}, {}", nameId(), acceleration_b__t0.x(), acceleration_b__t0.y(), acceleration_b__t0.z());

    /// g_n Gravity vector in [m/s^2], in navigation coordinates
    Eigen::Vector3d gravity_n__t1;

    /// Gravity vector determination
    if (gravityCompensation)
    {
        if (gravityModel == GravityModel::Somigliana)
        {
            LOG_DATA("Gravity calculated with Somigliana model");
            gravity_n__t1 = gravity::gravity_SomiglianaAltitude(posVelAtt__t1->latitude(), posVelAtt__t1->altitude());
        }
        else if (gravityModel == GravityModel::WGS84_Skydel) // TODO: This function becomes obsolete, once the ImuStream is deactivated due to the 'InstinctDataStream'
        {
            LOG_DATA("Gravity calculated with WGS84 model as in the Skydel Simulator plug-in");
            double gravityMagnitude = gravity::gravityMagnitude_WGS84_Skydel(posVelAtt__t1->latitude(), posVelAtt__t1->altitude());
            // Gravity vector NED
            const Eigen::Vector3d gravityVector(0.0, 0.0, gravityMagnitude);
            gravity_n__t1 = gravityVector;
        }
        else if (gravityModel == GravityModel::EGM96)
        {
            LOG_DATA("Gravity calculated with EGM96");
            int egm96degree = 10;
            gravity_n__t1 = gravity::gravity_EGM96(posVelAtt__t1->latitude(), posVelAtt__t1->longitude(), posVelAtt__t1->altitude(), egm96degree);
        }
        else
        {
            LOG_DATA("Gravity calculated with WGS84 model (derivation of the gravity potential after 'r')");
            gravity_n__t1 = gravity::gravity_WGS84(posVelAtt__t1->latitude(), posVelAtt__t1->altitude());
        }
    }
    else
    {
        gravity_n__t1 = Eigen::Vector3d::Zero();
    }

    LOG_DATA("Gravity vector in NED:\n{}", gravity_n__t1);

    /// g_e Gravity vector in [m/s^2], in earth coordinates
    const Eigen::Vector3d gravity_e__t1 = posVelAtt__t1->quaternion_en() * gravity_n__t1;

    if (integrationFrame == IntegrationFrame::ECEF)
    {
        /// q (tₖ₋₂) Quaternion, from gyro platform to earth coordinates, at the time tₖ₋₂
        const Eigen::Quaterniond quaternion_gyro_ep__t2 = posVelAtt__t2->quaternion_eb() * imuPosition.quatGyro_bp();
        /// q (tₖ₋₁) Quaternion, from gyro platform to earth coordinates, at the time tₖ₋₁
        const Eigen::Quaterniond quaternion_gyro_ep__t1 = posVelAtt__t1->quaternion_eb() * imuPosition.quatGyro_bp();

        /// ω_ie_e (tₖ) Angular velocity in [rad/s], of the inertial to earth system, in earth coordinates, at the time tₖ
        const Eigen::Vector3d& angularVelocity_ie_e__t0 = InsConst::angularVelocity_ie_e;

        /// q (tₖ₋₂) Quaternion, from accel platform to earth coordinates, at the time tₖ₋₂
        const Eigen::Quaterniond quaternion_accel_ep__t2 = posVelAtt__t2->quaternion_eb() * imuPosition.quatAccel_bp();
        /// q (tₖ₋₁) Quaternion, from accel platform to earth coordinates, at the time tₖ₋₁
        const Eigen::Quaterniond quaternion_accel_ep__t1 = posVelAtt__t1->quaternion_eb() * imuPosition.quatAccel_bp();

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                              Attitude Update                                             */
        /* -------------------------------------------------------------------------------------------------------- */

        /// q (tₖ) Quaternion, from platform to earth coordinates, at the current time tₖ
        const Eigen::Quaterniond quaternion_gyro_ep__t0 = updateQuaternion_ep_RungeKutta3(timeDifferenceSec__t0, timeDifferenceSec__t1,
                                                                                          angularVelocity_ip_p__t0, angularVelocity_ip_p__t1,
                                                                                          angularVelocity_ie_e__t0,
                                                                                          quaternion_gyro_ep__t1, quaternion_gyro_ep__t2);

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                    Specific force frame transformation                                   */
        /* -------------------------------------------------------------------------------------------------------- */

        /// q (tₖ) Quaternion, from accel platform to earth coordinates, at the time tₖ
        const Eigen::Quaterniond quaternion_accel_ep__t0 = quaternion_gyro_ep__t0 * imuPosition.quatGyro_pb() * imuPosition.quatAccel_bp();

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                              Velocity update                                             */
        /* -------------------------------------------------------------------------------------------------------- */

        /// v (tₖ), Velocity in [m/s], in earth coordinates, at the current time tₖ
        const Eigen::Vector3d velocity_e__t0 = updateVelocity_e_RungeKutta3(timeDifferenceSec__t0, timeDifferenceSec__t1,
                                                                            acceleration_p__t0, acceleration_p__t1,
                                                                            velocity_e__t2,
                                                                            position_e__t2,
                                                                            gravity_e__t1,
                                                                            quaternion_accel_ep__t0,
                                                                            quaternion_accel_ep__t1,
                                                                            quaternion_accel_ep__t2
#ifndef NDEBUG
                                                                            ,
                                                                            coriolisCompensation
#endif
        );

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                              Position update                                             */
        /* -------------------------------------------------------------------------------------------------------- */

        /// x_e (tₖ) Position in [m], in earth coordinates, at the time tₖ
        const Eigen::Vector3d position_e__t0 = updatePosition_e(timeDifferenceSec__t0, position_e__t1, velocity_e__t1);

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                               Store Results                                              */
        /* -------------------------------------------------------------------------------------------------------- */

        // Store position in the state. Important to do before using the quaternion_en.
        posVelAtt__t0->position_ecef() = position_e__t0;
        // Quaternion for rotation from earth to navigation frame. Depends on position which was updated before
        Eigen::Quaterniond quaternion_ne__t0 = posVelAtt__t0->quaternion_ne();
        // Store velocity in the state
        posVelAtt__t0->velocity_n() = quaternion_ne__t0 * velocity_e__t0;
        // Store body to navigation frame quaternion in the state
        posVelAtt__t0->quaternion_nb() = quaternion_ne__t0 * quaternion_gyro_ep__t0 * imuPosition.quatGyro_pb();
    }
    else if (integrationFrame == IntegrationFrame::NED)
    {
        /// ω_ip_b (tₖ₋₁) Angular velocity in [rad/s],
        /// of the inertial to platform system, in body coordinates, at the time tₖ₋₁
        const Eigen::Vector3d angularVelocity_ip_b__t1 = imuPosition.quatGyro_bp() * angularVelocity_ip_p__t1;
        /// ω_ip_b (tₖ) Angular velocity in [rad/s],
        /// of the inertial to platform system, in body coordinates, at the time tₖ
        const Eigen::Vector3d angularVelocity_ip_b__t0 = imuPosition.quatGyro_bp() * angularVelocity_ip_p__t0;

        /// a_b (tₖ₋₁) Acceleration in [m/s^2], in body coordinates, at the time tₖ₋₁
        const Eigen::Vector3d acceleration_b__t1 = imuPosition.quatAccel_bp() * acceleration_p__t1;
        /// a_b (tₖ) Acceleration in [m/s^2], in body coordinates, at the time tₖ
        const Eigen::Vector3d acceleration_b__t0 = imuPosition.quatAccel_bp() * acceleration_p__t0;

        /// q (tₖ₋₁) Quaternion, from body to navigation coordinates, at the time tₖ₋₁
        const Eigen::Quaterniond quaternion_nb__t1 = posVelAtt__t1->quaternion_nb();
        /// q (tₖ₋₂) Quaternion, from body to navigation coordinates, at the time tₖ₋₂
        const Eigen::Quaterniond quaternion_nb__t2 = posVelAtt__t2->quaternion_nb();

        /// ω_ie_n Nominal mean angular velocity of the Earth in [rad/s], in navigation coordinates
        Eigen::Vector3d angularVelocity_ie_n__t1 = posVelAtt__t1->quaternion_ne() * InsConst::angularVelocity_ie_e;

        /// North/South (meridian) earth radius [m]
        double R_N = earthRadius_N(InsConst::WGS84_a, InsConst::WGS84_e_squared, posVelAtt__t1->latitude());
        /// East/West (prime vertical) earth radius [m]
        double R_E = earthRadius_E(InsConst::WGS84_a, InsConst::WGS84_e_squared, posVelAtt__t1->latitude());

        /// ω_en_n (tₖ₋₁) Transport Rate, rotation rate of the Earth frame relative to the navigation frame, in navigation coordinates
        Eigen::Vector3d angularVelocity_en_n__t1 = transportRate(posVelAtt__t1->latLonAlt(), velocity_n__t1, R_N, R_E);

        /// [x_n, x_e, x_d] (tₖ₋₁) Position NED in [m] at the time tₖ₋₁
        Eigen::Vector3d position_n__t1 = trafo::ecef2ned(position_e__t1, posVelAtt__init->latLonAlt());

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                              Attitude Update                                             */
        /* -------------------------------------------------------------------------------------------------------- */

        /// q (tₖ) Quaternion, from body to navigation coordinates, at the current time tₖ
        Eigen::Quaterniond quaternion_nb__t0 = updateQuaternion_nb_RungeKutta3(timeDifferenceSec__t0,
                                                                               timeDifferenceSec__t1,
                                                                               angularVelocity_ip_b__t0,
                                                                               angularVelocity_ip_b__t1,
                                                                               angularVelocity_ie_n__t1,
                                                                               angularVelocity_en_n__t1,
                                                                               quaternion_nb__t1,
                                                                               quaternion_nb__t2);

        /* -------------------------------------------------------------------------------------------------------- */
        /*                           Specific force frame transformation & Velocity update                          */
        /* -------------------------------------------------------------------------------------------------------- */

        /// v (tₖ), Velocity in navigation coordinates, at the current time tₖ
        Eigen::Vector3d velocity_n__t0 = updateVelocity_n_RungeKutta3(timeDifferenceSec__t0, timeDifferenceSec__t1,
                                                                      acceleration_b__t0, acceleration_b__t1,
                                                                      velocity_n__t1, velocity_n__t2,
                                                                      gravity_n__t1,
                                                                      angularVelocity_ie_n__t1,
                                                                      angularVelocity_en_n__t1,
                                                                      quaternion_nb__t0, quaternion_nb__t1, quaternion_nb__t2
#ifndef NDEBUG
                                                                      ,
                                                                      coriolisCompensation
#endif
        );

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                              Position update                                             */
        /* -------------------------------------------------------------------------------------------------------- */

        /// [x_n, x_e, x_d] (tₖ) Position NED in [m] at the time tₖ
        Eigen::Vector3d position_n__t0 = updatePosition_n(timeDifferenceSec__t0, position_n__t1, velocity_n__t1);

        /// x_e (tₖ) Position in [m], in ECEF coordinates, at the time tₖ
        Eigen::Vector3d position_e__t0 = trafo::ned2ecef(position_n__t0, posVelAtt__init->latLonAlt());

        /// Latitude, Longitude and Altitude in [rad, rad, m], at the current time tₖ (see Gleason eq. 6.18 - 6.20)
        // Vector3d<LLA> latLonAlt__t0 = updatePosition_n(timeDifferenceSec__t0, posVelAtt__t1->latLonAlt(),
        //                                                     velocity_n__t1, R_N, R_E);

        /* -------------------------------------------------------------------------------------------------------- */
        /*                                               Store Results                                              */
        /* -------------------------------------------------------------------------------------------------------- */

        // Store position in the state
        posVelAtt__t0->position_ecef() = position_e__t0;
        // Store velocity in the state
        posVelAtt__t0->velocity_n() = velocity_n__t0;
        // Store body to navigation frame quaternion in the state
        posVelAtt__t0->quaternion_nb() = quaternion_nb__t0;
    }

    // Cycle lists
    imuObservations.pop_back();
    posVelAttStates.pop_back();

    // Push out new data
    invokeCallbacks(OutputPortIndex_PosVelAtt__t0, posVelAtt__t0);
}