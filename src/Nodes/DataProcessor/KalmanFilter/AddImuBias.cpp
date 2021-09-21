#include "AddImuBias.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/IMU/ImuObs.hpp"

NAV::AddImuBias::AddImuBias()
{
    LOG_TRACE("{}: called", name);

    hasConfig = false;
    kind = Kind::Simple;

    nm::CreateInputPin(this, "ImuObs", Pin::Type::Flow, { ImuObs::type() }, &AddImuBias::recvImuObs);
    nm::CreateInputPin(this, "ImuBiases", Pin::Type::Flow, { ImuBiases::type() }, &AddImuBias::recvImuBiases);
    nm::CreateOutputPin(this, "ImuObs", Pin::Type::Flow, { ImuObs::type() });
}

NAV::AddImuBias::~AddImuBias()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::AddImuBias::typeStatic()
{
    return "AddImuBias";
}

std::string NAV::AddImuBias::type() const
{
    return typeStatic();
}

std::string NAV::AddImuBias::category()
{
    return "Data Processor";
}

bool NAV::AddImuBias::initialize()
{
    LOG_TRACE("{}: called", nameId());

    imuBiases.biasAccel_p = { 0, 0, 0 };
    imuBiases.biasGyro_p = { 0, 0, 0 };

    return true;
}

void NAV::AddImuBias::deinitialize()
{
    LOG_TRACE("{}: called", nameId());
}

void NAV::AddImuBias::recvImuObs(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId /* linkId */)
{
    auto imuObs = std::dynamic_pointer_cast<const ImuObs>(nodeData);

    auto imuObsCorr = std::make_shared<ImuObs>(imuObs->imuPos);

    if (imuObs->accelUncompXYZ.has_value())
    {
        imuObsCorr->accelUncompXYZ = imuObs->accelUncompXYZ.value() - imuObs->imuPos.quatAccel_pb() * imuBiases.biasAccel_p;
    }
    if (imuObs->accelCompXYZ.has_value())
    {
        imuObsCorr->accelCompXYZ = imuObs->accelCompXYZ.value() - imuObs->imuPos.quatAccel_pb() * imuBiases.biasAccel_p;
    }

    if (imuObs->gyroUncompXYZ.has_value())
    {
        imuObsCorr->gyroUncompXYZ = imuObs->gyroUncompXYZ.value() - imuObs->imuPos.quatGyro_pb() * imuBiases.biasGyro_p;
    }
    if (imuObs->gyroCompXYZ.has_value())
    {
        imuObsCorr->gyroCompXYZ = imuObs->gyroCompXYZ.value() - imuObs->imuPos.quatGyro_pb() * imuBiases.biasGyro_p;
    }

    imuObsCorr->insTime = imuObs->insTime;
    imuObsCorr->timeSinceStartup = imuObs->timeSinceStartup;

    imuObsCorr->magUncompXYZ = imuObs->magUncompXYZ;
    // imuObsCorr->accelUncompXYZ = imuObs->accelUncompXYZ;
    // imuObsCorr->gyroUncompXYZ = imuObs->gyroUncompXYZ;

    imuObsCorr->magCompXYZ = imuObs->magCompXYZ;
    // imuObsCorr->accelCompXYZ = imuObs->accelCompXYZ;
    // imuObsCorr->gyroCompXYZ = imuObs->gyroCompXYZ;

    imuObsCorr->temperature = imuObs->temperature;

    invokeCallbacks(OutputPortIndex_ImuObs, imuObsCorr);
}

void NAV::AddImuBias::recvImuBiases(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId /* linkId */)
{
    auto imuBiasObs = std::dynamic_pointer_cast<const ImuBiases>(nodeData);

    imuBiases.biasAccel_p += imuBiasObs->biasAccel_p;
    imuBiases.biasGyro_p += imuBiasObs->biasGyro_p;
}