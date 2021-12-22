#include "ImuError.hpp"

#include "NodeData/IMU/ImuObs.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "internal/gui/widgets/InputWithUnit.hpp"

#include "Navigation/Transformations/CoordinateFrames.hpp"

#include "util/Eigen.hpp"

NAV::ImuError::ImuError()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);
    hasConfig = true;
    guiConfigDefaultWindowSize = { 350, 123 };

    nm::CreateOutputPin(this, "ImuObs", Pin::Type::Flow, { NAV::ImuObs::type() });

    nm::CreateInputPin(this, "ImuObs", Pin::Type::Flow, { NAV::ImuObs::type() }, &ImuError::receiveObs);
}

NAV::ImuError::~ImuError()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::ImuError::typeStatic()
{
    return "ImuError";
}

std::string NAV::ImuError::type() const
{
    return typeStatic();
}

std::string NAV::ImuError::category()
{
    return "DataProcessor";
}

void NAV::ImuError::guiConfig()
{
    float itemWidth = 350;
    float unitWidth = 80;

    if (gui::widgets::InputDouble3WithUnit(fmt::format("Accelerometer Bias (platform)##{}", size_t(id)).c_str(), itemWidth, unitWidth,
                                           imuAccelerometerBias_p.data(), reinterpret_cast<int*>(&accelerometerBiasUnit), "m/s^2\0\0", // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
                                           "%.2e", ImGuiInputTextFlags_CharsScientific))
    {
        LOG_DEBUG("{}: imuAccelerometerBias_p changed to {}", nameId(), imuAccelerometerBias_p.transpose());
        LOG_DEBUG("{}: accelerometerBiasUnit changed to {}", nameId(), accelerometerBiasUnit);
        flow::ApplyChanges();
    }
    if (gui::widgets::InputDouble3WithUnit(fmt::format("Gyroscope Bias (platform)##{}", size_t(id)).c_str(), itemWidth, unitWidth,
                                           imuGyroscopeBias_p.data(), reinterpret_cast<int*>(&gyroscopeBiasUnit), "rad/s\0deg/s\0\0", // NOLINT(hicpp-avoid-c-arrays,modernize-avoid-c-arrays,cppcoreguidelines-avoid-c-arrays)
                                           "%.2e", ImGuiInputTextFlags_CharsScientific))
    {
        LOG_DEBUG("{}: imuGyroscopeBias_p changed to {}", nameId(), imuGyroscopeBias_p.transpose());
        LOG_DEBUG("{}: gyroscopeBiasUnit changed to {}", nameId(), gyroscopeBiasUnit);
        flow::ApplyChanges();
    }
}

[[nodiscard]] json NAV::ImuError::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["accelerometerBiasUnit"] = accelerometerBiasUnit;
    j["imuAccelerometerBias_p"] = imuAccelerometerBias_p;
    j["gyroscopeBiasUnit"] = gyroscopeBiasUnit;
    j["imuGyroscopeBias_p"] = imuGyroscopeBias_p;

    return j;
}

void NAV::ImuError::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("accelerometerBiasUnit"))
    {
        j.at("accelerometerBiasUnit").get_to(accelerometerBiasUnit);
    }
    if (j.contains("imuAccelerometerBias_p"))
    {
        j.at("imuAccelerometerBias_p").get_to(imuAccelerometerBias_p);
    }
    if (j.contains("gyroscopeBiasUnit"))
    {
        j.at("gyroscopeBiasUnit").get_to(gyroscopeBiasUnit);
    }
    if (j.contains("imuGyroscopeBias_p"))
    {
        j.at("imuGyroscopeBias_p").get_to(imuGyroscopeBias_p);
    }
}

bool NAV::ImuError::initialize()
{
    LOG_TRACE("{}: called", nameId());

    return true;
}

void NAV::ImuError::receiveObs(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId /*linkId*/)
{
    auto imuObs = std::make_shared<ImuObs>(*std::static_pointer_cast<const ImuObs>(nodeData));

    Eigen::Vector3d accelerometerBias_p = imuAccelerometerBias_p;

    Eigen::Vector3d gyroscopeBias_p = Eigen::Vector3d::Zero();
    if (gyroscopeBiasUnit == GyroscopeBiasUnits::deg_s)
    {
        gyroscopeBias_p = trafo::deg2rad3(imuGyroscopeBias_p);
    }
    else // if (gyroscopeBiasUnit == GyroscopeBiasUnits::rad_s)
    {
        gyroscopeBias_p = imuGyroscopeBias_p;
    }

    imuObs->accelUncompXYZ.value() += accelerometerBias_p;
    imuObs->gyroUncompXYZ.value() += gyroscopeBias_p;

    invokeCallbacks(OutputPortIndex_ImuObs, imuObs);
}