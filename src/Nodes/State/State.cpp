#include "State.hpp"

#include "util/Logger.hpp"

NAV::State::State(const std::string& name, const std::map<std::string, std::string>& options)
    : Node(name)
{
    initialState = std::make_shared<StateData>();

    if (options.count("Static Initialization"))
    {
        dynamicStateInit = !static_cast<bool>(std::stoi(options.at("Static Initialization")));
    }
    if (options.count("Init Time"))
    {
        averageOverSeconds = std::stod(options.at("Init Time"));
    }
    if (!dynamicStateInit)
    {
        if (options.count("Init LatLonAlt"))
        {
            std::stringstream lineStream(options.at("Init LatLonAlt"));
            std::string value;
            if (std::getline(lineStream, value, ';'))
            {
                initialState->latitude() = trafo::deg2rad(std::stod(value));
                if (std::getline(lineStream, value, ';'))
                {
                    initialState->longitude() = trafo::deg2rad(std::stod(value));
                    if (std::getline(lineStream, value, ';'))
                    {
                        initialState->altitude() = std::stod(value);
                    }
                }
            }
        }
        if (options.count("Init RollPitchYaw"))
        {
            std::stringstream lineStream(options.at("Init RollPitchYaw"));
            std::string value;
            double roll{};
            double pitch{};
            double yaw{};
            if (std::getline(lineStream, value, ';'))
            {
                roll = trafo::deg2rad(std::stod(value));
                if (std::getline(lineStream, value, ';'))
                {
                    pitch = trafo::deg2rad(std::stod(value));
                    if (std::getline(lineStream, value, ';'))
                    {
                        yaw = trafo::deg2rad(std::stod(value));
                    }
                }
            }
            initialState->quaternion_nb() = trafo::quat_nb(roll, pitch, yaw);
        }
        if (options.count("Init Velocity"))
        {
            std::stringstream lineStream(options.at("Init Velocity"));
            std::string value;
            double v_n{};
            double v_e{};
            double v_d{};
            if (std::getline(lineStream, value, ';'))
            {
                v_n = std::stod(value);
                if (std::getline(lineStream, value, ';'))
                {
                    v_e = std::stod(value);
                    if (std::getline(lineStream, value, ';'))
                    {
                        v_d = std::stod(value);
                    }
                }
            }
            initialState->velocity_n() = { v_n, v_e, v_d };
        }

        currentState = std::make_shared<StateData>();
        currentState->quaternion_nb() = initialState->quaternion_nb();
        currentState->latLonAlt() = initialState->latLonAlt();
        currentState->velocity_n() = initialState->velocity_n();
    }
}

void NAV::State::updateState(std::shared_ptr<StateData>& state)
{
    // Process the data here
    currentState = state;

    // For now does not invoke callbacks, as the state is used on request basis
    // invokeCallbacks(currentState);
}

void NAV::State::initAttitude(std::shared_ptr<ImuObs>& obs)
{
    // Get the IMU Position information
    const auto& imuNode = incomingLinks[3].first.lock();
    auto& imuPortIndex = incomingLinks[3].second;
    auto imuPosition = std::static_pointer_cast<ImuPos>(imuNode->requestOutputData(imuPortIndex));

    const auto magUncomp_b = imuPosition->quatMag_bp() * obs->magUncompXYZ.value();
    auto magneticHeading = -std::atan2(magUncomp_b.y(), magUncomp_b.x());

    const auto accelUncomp_b = imuPosition->quatAccel_bp() * obs->accelUncompXYZ.value() * -1;
    auto roll = std::atan2(accelUncomp_b.y(), accelUncomp_b.z());
    auto pitch = std::atan2((-accelUncomp_b.x()), sqrt(std::pow(accelUncomp_b.y(), 2) + std::pow(accelUncomp_b.z(), 2)));

    // Average with previous attitude
    countAveragedAttitude++;
    if (countAveragedAttitude > 1)
    {
        roll = (initialState->rollPitchYaw().x() * (countAveragedAttitude - 1) + roll) / countAveragedAttitude;
        pitch = (initialState->rollPitchYaw().y() * (countAveragedAttitude - 1) + pitch) / countAveragedAttitude;
        magneticHeading = (initialState->rollPitchYaw().z() * (countAveragedAttitude - 1) + magneticHeading) / countAveragedAttitude;
    }

    initialState->quaternion_nb() = trafo::quat_nb(roll, pitch, magneticHeading);

    finalizeInit(obs->insTime.value());
}

void NAV::State::initPositionVelocity(std::shared_ptr<GnssObs>& obs)
{
    if (obs->position_ecef.has_value())
    {
        Vector3d<LLA> latLonAlt = trafo::ecef2lla_WGS84(obs->position_ecef.value());

        double lat = latLonAlt.x();
        double lon = latLonAlt.y();
        double alt = latLonAlt.z();

        // Already received a position, so calculate velocity
        if (countAveragedPosition > 0)
        {
            Vector3d<Earth> pos_e__t0 = trafo::lla2ecef_WGS84({ lat, lon, alt });
            Vector3d<Earth> pos_e__t1 = trafo::lla2ecef_WGS84(initialState->latLonAlt());

            auto dt = (obs->insTime.value() - initialState->insTime.value()).count();

            Vector3d<Earth> velocity_e = (pos_e__t0 - pos_e__t1) / dt;
            Vector3d<Navigation> velocity_n = initialState->quaternion_ne() * velocity_e;

            // Average with previous velocity
            countAveragedVelocity++;
            if (countAveragedVelocity > 1)
            {
                velocity_n = (initialState->velocity_n() * (countAveragedVelocity - 1) + velocity_n) / countAveragedVelocity;
            }

            initialState->velocity_n() = velocity_n;
        }

        // Average with previous position
        countAveragedPosition++;
        if (countAveragedPosition > 1)
        {
            lat = (initialState->latitude() * (countAveragedPosition - 1) + lat) / countAveragedPosition;
            lon = (initialState->longitude() * (countAveragedPosition - 1) + lon) / countAveragedPosition;
            alt = (initialState->altitude() * (countAveragedPosition - 1) + alt) / countAveragedPosition;
        }
        initialState->latLonAlt() = { lat, lon, alt };
        initialState->insTime = obs->insTime;

        finalizeInit(obs->insTime.value());
    }
}

void NAV::State::finalizeInit(const InsTime& currentTime)
{
    constexpr double minimumSamples = 2;
    if (averageStartTime > currentTime)
    {
        averageStartTime = currentTime;
    }
    if ((currentTime - averageStartTime).count() > static_cast<long double>(averageOverSeconds)
        && countAveragedAttitude >= minimumSamples && countAveragedPosition >= minimumSamples && countAveragedVelocity >= minimumSamples)
    {
        dynamicStateInit = false;

        LOG_INFO("{}: State initialized to Lat {:3.4f} [°], Lon {:3.4f} [°], Alt {:4.4f} [m]", name,
                 trafo::rad2deg(initialState->latitude()),
                 trafo::rad2deg(initialState->longitude()),
                 initialState->altitude());
        LOG_INFO("{}: State initialized to v_N {:3.5f} [m/s], v_E {:3.5f}, v_D {:3.5f}", name,
                 initialState->velocity_n().x(),
                 initialState->velocity_n().x(),
                 initialState->velocity_n().z());
        LOG_INFO("{}: State initialized to Roll {:3.5f} [°], Pitch {:3.5f} [°], Yaw {:3.4f} [°]", name,
                 trafo::rad2deg(initialState->rollPitchYaw().x()),
                 trafo::rad2deg(initialState->rollPitchYaw().y()),
                 trafo::rad2deg(initialState->rollPitchYaw().z()));

        currentState = std::make_shared<StateData>();
        currentState->insTime = currentTime;
        currentState->quaternion_nb() = initialState->quaternion_nb();
        currentState->latLonAlt() = initialState->latLonAlt();
        currentState->velocity_n() = initialState->velocity_n();
    }
}