#include "VectorNavSensor.hpp"

#include "util/Debug.hpp"
#include "util/Logger.hpp"
#include "vn/searcher.h"
#include "vn/util.h"

#include "internal/gui/widgets/HelpMarker.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include <imgui_internal.h>

#include "NodeData/General/StringObs.hpp"
#include "NodeData/IMU/VectorNavBinaryOutput.hpp"

#include "util/Time/TimeBase.hpp"
#include "util/InsTransformations.hpp"

#include <map>

// to_json / from_json
namespace vn
{
namespace math
{
void to_json(json& j, const vn::math::vec3f& vec)
{
    j = vec.c;
}
void from_json(const json& j, vn::math::vec3f& vec)
{
    j.get_to(vec.c);
}
void to_json(json& j, const vn::math::vec3d& vec)
{
    j = vec.c;
}
void from_json(const json& j, vn::math::vec3d& vec)
{
    j.get_to(vec.c);
}
void to_json(json& j, const vn::math::mat3f& mat)
{
    j = mat.e;
}
void from_json(const json& j, vn::math::mat3f& mat)
{
    j.get_to(mat.e);
}
} // namespace math

namespace sensors
{
// ###########################################################################################################
//                                               SYSTEM MODULE
// ###########################################################################################################

void to_json(json& j, const SynchronizationControlRegister& synchronizationControlRegister)
{
    j = json{
        { "syncInMode", synchronizationControlRegister.syncInMode },
        { "syncInEdge", synchronizationControlRegister.syncInEdge },
        { "syncInSkipFactor", synchronizationControlRegister.syncInSkipFactor },
        { "syncOutMode", synchronizationControlRegister.syncOutMode },
        { "syncOutPolarity", synchronizationControlRegister.syncOutPolarity },
        { "syncOutSkipFactor", synchronizationControlRegister.syncOutSkipFactor },
        { "syncOutPulseWidth", synchronizationControlRegister.syncOutPulseWidth },
    };
}
void from_json(const json& j, SynchronizationControlRegister& synchronizationControlRegister)
{
    if (j.contains("syncInMode"))
    {
        j.at("syncInMode").get_to(synchronizationControlRegister.syncInMode);
    }
    if (j.contains("syncInEdge"))
    {
        j.at("syncInEdge").get_to(synchronizationControlRegister.syncInEdge);
    }
    if (j.contains("syncInSkipFactor"))
    {
        j.at("syncInSkipFactor").get_to(synchronizationControlRegister.syncInSkipFactor);
    }
    if (j.contains("syncOutMode"))
    {
        j.at("syncOutMode").get_to(synchronizationControlRegister.syncOutMode);
    }
    if (j.contains("syncOutPolarity"))
    {
        j.at("syncOutPolarity").get_to(synchronizationControlRegister.syncOutPolarity);
    }
    if (j.contains("syncOutSkipFactor"))
    {
        j.at("syncOutSkipFactor").get_to(synchronizationControlRegister.syncOutSkipFactor);
    }
    if (j.contains("syncOutPulseWidth"))
    {
        j.at("syncOutPulseWidth").get_to(synchronizationControlRegister.syncOutPulseWidth);
    }
}

void to_json(json& j, const CommunicationProtocolControlRegister& communicationProtocolControlRegister)
{
    j = json{
        { "serialCount", communicationProtocolControlRegister.serialCount },
        { "serialStatus", communicationProtocolControlRegister.serialStatus },
        { "spiCount", communicationProtocolControlRegister.spiCount },
        { "spiStatus", communicationProtocolControlRegister.spiStatus },
        { "serialChecksum", communicationProtocolControlRegister.serialChecksum },
        { "spiChecksum", communicationProtocolControlRegister.spiChecksum },
        { "errorMode", communicationProtocolControlRegister.errorMode },
    };
}
void from_json(const json& j, CommunicationProtocolControlRegister& communicationProtocolControlRegister)
{
    if (j.contains("serialCount"))
    {
        j.at("serialCount").get_to(communicationProtocolControlRegister.serialCount);
    }
    if (j.contains("serialStatus"))
    {
        j.at("serialStatus").get_to(communicationProtocolControlRegister.serialStatus);
    }
    if (j.contains("spiCount"))
    {
        j.at("spiCount").get_to(communicationProtocolControlRegister.spiCount);
    }
    if (j.contains("spiStatus"))
    {
        j.at("spiStatus").get_to(communicationProtocolControlRegister.spiStatus);
    }
    if (j.contains("serialChecksum"))
    {
        j.at("serialChecksum").get_to(communicationProtocolControlRegister.serialChecksum);
    }
    if (j.contains("spiChecksum"))
    {
        j.at("spiChecksum").get_to(communicationProtocolControlRegister.spiChecksum);
    }
    if (j.contains("errorMode"))
    {
        j.at("errorMode").get_to(communicationProtocolControlRegister.errorMode);
    }
}

void to_json(json& j, const BinaryOutputRegister& binaryOutputRegister)
{
    j = json{
        { "asyncMode", binaryOutputRegister.asyncMode },
        { "rateDivisor", binaryOutputRegister.rateDivisor },
        { "commonField", binaryOutputRegister.commonField },
        { "timeField", binaryOutputRegister.timeField },
        { "imuField", binaryOutputRegister.imuField },
        { "gpsField", binaryOutputRegister.gpsField },
        { "attitudeField", binaryOutputRegister.attitudeField },
        { "insField", binaryOutputRegister.insField },
        { "gps2Field", binaryOutputRegister.gps2Field },
    };
}
void from_json(const json& j, BinaryOutputRegister& binaryOutputRegister)
{
    if (j.contains("asyncMode"))
    {
        j.at("asyncMode").get_to(binaryOutputRegister.asyncMode);
    }
    if (j.contains("rateDivisor"))
    {
        j.at("rateDivisor").get_to(binaryOutputRegister.rateDivisor);
    }
    if (j.contains("commonField"))
    {
        j.at("commonField").get_to(binaryOutputRegister.commonField);
    }
    if (j.contains("timeField"))
    {
        j.at("timeField").get_to(binaryOutputRegister.timeField);
    }
    if (j.contains("imuField"))
    {
        j.at("imuField").get_to(binaryOutputRegister.imuField);
    }
    if (j.contains("gpsField"))
    {
        j.at("gpsField").get_to(binaryOutputRegister.gpsField);
    }
    if (j.contains("attitudeField"))
    {
        j.at("attitudeField").get_to(binaryOutputRegister.attitudeField);
    }
    if (j.contains("insField"))
    {
        j.at("insField").get_to(binaryOutputRegister.insField);
    }
    if (j.contains("gps2Field"))
    {
        j.at("gps2Field").get_to(binaryOutputRegister.gps2Field);
    }
}

// ###########################################################################################################
//                                               IMU SUBSYSTEM
// ###########################################################################################################

void to_json(json& j, const ImuFilteringConfigurationRegister& imuFilteringConfigurationRegister)
{
    j = json{
        { "magWindowSize", imuFilteringConfigurationRegister.magWindowSize },
        { "accelWindowSize", imuFilteringConfigurationRegister.accelWindowSize },
        { "gyroWindowSize", imuFilteringConfigurationRegister.gyroWindowSize },
        { "tempWindowSize", imuFilteringConfigurationRegister.tempWindowSize },
        { "presWindowSize", imuFilteringConfigurationRegister.presWindowSize },
        { "magFilterMode", imuFilteringConfigurationRegister.magFilterMode },
        { "accelFilterMode", imuFilteringConfigurationRegister.accelFilterMode },
        { "gyroFilterMode", imuFilteringConfigurationRegister.gyroFilterMode },
        { "tempFilterMode", imuFilteringConfigurationRegister.tempFilterMode },
        { "presFilterMode", imuFilteringConfigurationRegister.presFilterMode },
    };
}
void from_json(const json& j, ImuFilteringConfigurationRegister& imuFilteringConfigurationRegister)
{
    if (j.contains("magWindowSize"))
    {
        j.at("magWindowSize").get_to(imuFilteringConfigurationRegister.magWindowSize);
    }
    if (j.contains("accelWindowSize"))
    {
        j.at("accelWindowSize").get_to(imuFilteringConfigurationRegister.accelWindowSize);
    }
    if (j.contains("gyroWindowSize"))
    {
        j.at("gyroWindowSize").get_to(imuFilteringConfigurationRegister.gyroWindowSize);
    }
    if (j.contains("tempWindowSize"))
    {
        j.at("tempWindowSize").get_to(imuFilteringConfigurationRegister.tempWindowSize);
    }
    if (j.contains("presWindowSize"))
    {
        j.at("presWindowSize").get_to(imuFilteringConfigurationRegister.presWindowSize);
    }
    if (j.contains("magFilterMode"))
    {
        j.at("magFilterMode").get_to(imuFilteringConfigurationRegister.magFilterMode);
    }
    if (j.contains("accelFilterMode"))
    {
        j.at("accelFilterMode").get_to(imuFilteringConfigurationRegister.accelFilterMode);
    }
    if (j.contains("gyroFilterMode"))
    {
        j.at("gyroFilterMode").get_to(imuFilteringConfigurationRegister.gyroFilterMode);
    }
    if (j.contains("tempFilterMode"))
    {
        j.at("tempFilterMode").get_to(imuFilteringConfigurationRegister.tempFilterMode);
    }
    if (j.contains("presFilterMode"))
    {
        j.at("presFilterMode").get_to(imuFilteringConfigurationRegister.presFilterMode);
    }
}

void to_json(json& j, const DeltaThetaAndDeltaVelocityConfigurationRegister& deltaThetaAndDeltaVelocityConfigurationRegister)
{
    j = json{
        { "integrationFrame", deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame },
        { "gyroCompensation", deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation },
        { "accelCompensation", deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation },
        { "earthRateCorrection", deltaThetaAndDeltaVelocityConfigurationRegister.earthRateCorrection },
    };
}
void from_json(const json& j, DeltaThetaAndDeltaVelocityConfigurationRegister& deltaThetaAndDeltaVelocityConfigurationRegister)
{
    if (j.contains("integrationFrame"))
    {
        j.at("integrationFrame").get_to(deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame);
    }
    if (j.contains("gyroCompensation"))
    {
        j.at("gyroCompensation").get_to(deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation);
    }
    if (j.contains("accelCompensation"))
    {
        j.at("accelCompensation").get_to(deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation);
    }
    if (j.contains("earthRateCorrection"))
    {
        j.at("earthRateCorrection").get_to(deltaThetaAndDeltaVelocityConfigurationRegister.earthRateCorrection);
    }
}

// ###########################################################################################################
//                                              GNSS SUBSYSTEM
// ###########################################################################################################

void to_json(json& j, const GpsConfigurationRegister& gpsConfigurationRegister)
{
    j = json{
        { "mode", gpsConfigurationRegister.mode },
        { "ppsSource", gpsConfigurationRegister.ppsSource },
        { "rate", gpsConfigurationRegister.rate },
        { "antPow", gpsConfigurationRegister.antPow },
    };
}
void from_json(const json& j, GpsConfigurationRegister& gpsConfigurationRegister)
{
    if (j.contains("mode"))
    {
        j.at("mode").get_to(gpsConfigurationRegister.mode);
    }
    if (j.contains("ppsSource"))
    {
        j.at("ppsSource").get_to(gpsConfigurationRegister.ppsSource);
    }
    if (j.contains("rate"))
    {
        j.at("rate").get_to(gpsConfigurationRegister.rate);
    }
    if (j.contains("antPow"))
    {
        j.at("antPow").get_to(gpsConfigurationRegister.antPow);
    }
}

void to_json(json& j, const GpsCompassBaselineRegister& gpsCompassBaselineRegister)
{
    j = json{
        { "position", gpsCompassBaselineRegister.position },
        { "uncertainty", gpsCompassBaselineRegister.uncertainty },
    };
}
void from_json(const json& j, GpsCompassBaselineRegister& gpsCompassBaselineRegister)
{
    if (j.contains("position"))
    {
        j.at("position").get_to(gpsCompassBaselineRegister.position);
    }
    if (j.contains("uncertainty"))
    {
        j.at("uncertainty").get_to(gpsCompassBaselineRegister.uncertainty);
    }
}

// ###########################################################################################################
//                                            ATTITUDE SUBSYSTEM
// ###########################################################################################################

void to_json(json& j, const VpeBasicControlRegister& vpeBasicControlRegister)
{
    j = json{
        { "enable", vpeBasicControlRegister.enable },
        { "headingMode", vpeBasicControlRegister.headingMode },
        { "filteringMode", vpeBasicControlRegister.filteringMode },
        { "tuningMode", vpeBasicControlRegister.tuningMode },
    };
}
void from_json(const json& j, VpeBasicControlRegister& vpeBasicControlRegister)
{
    if (j.contains("enable"))
    {
        j.at("enable").get_to(vpeBasicControlRegister.enable);
    }
    if (j.contains("headingMode"))
    {
        j.at("headingMode").get_to(vpeBasicControlRegister.headingMode);
    }
    if (j.contains("filteringMode"))
    {
        j.at("filteringMode").get_to(vpeBasicControlRegister.filteringMode);
    }
    if (j.contains("tuningMode"))
    {
        j.at("tuningMode").get_to(vpeBasicControlRegister.tuningMode);
    }
}

void to_json(json& j, const VpeMagnetometerBasicTuningRegister& vpeMagnetometerBasicTuningRegister)
{
    j = json{
        { "baseTuning", vpeMagnetometerBasicTuningRegister.baseTuning },
        { "adaptiveTuning", vpeMagnetometerBasicTuningRegister.adaptiveTuning },
        { "adaptiveFiltering", vpeMagnetometerBasicTuningRegister.adaptiveFiltering },
    };
}
void from_json(const json& j, VpeMagnetometerBasicTuningRegister& vpeMagnetometerBasicTuningRegister)
{
    if (j.contains("baseTuning"))
    {
        j.at("baseTuning").get_to(vpeMagnetometerBasicTuningRegister.baseTuning);
    }
    if (j.contains("adaptiveTuning"))
    {
        j.at("adaptiveTuning").get_to(vpeMagnetometerBasicTuningRegister.adaptiveTuning);
    }
    if (j.contains("adaptiveFiltering"))
    {
        j.at("adaptiveFiltering").get_to(vpeMagnetometerBasicTuningRegister.adaptiveFiltering);
    }
}

void to_json(json& j, const VpeAccelerometerBasicTuningRegister& vpeAccelerometerBasicTuningRegister)
{
    j = json{
        { "baseTuning", vpeAccelerometerBasicTuningRegister.baseTuning },
        { "adaptiveTuning", vpeAccelerometerBasicTuningRegister.adaptiveTuning },
        { "adaptiveFiltering", vpeAccelerometerBasicTuningRegister.adaptiveFiltering },
    };
}
void from_json(const json& j, VpeAccelerometerBasicTuningRegister& vpeAccelerometerBasicTuningRegister)
{
    if (j.contains("baseTuning"))
    {
        j.at("baseTuning").get_to(vpeAccelerometerBasicTuningRegister.baseTuning);
    }
    if (j.contains("adaptiveTuning"))
    {
        j.at("adaptiveTuning").get_to(vpeAccelerometerBasicTuningRegister.adaptiveTuning);
    }
    if (j.contains("adaptiveFiltering"))
    {
        j.at("adaptiveFiltering").get_to(vpeAccelerometerBasicTuningRegister.adaptiveFiltering);
    }
}

void to_json(json& j, const VpeGyroBasicTuningRegister& vpeGyroBasicTuningRegister)
{
    j = json{
        { "angularWalkVariance", vpeGyroBasicTuningRegister.angularWalkVariance },
        { "baseTuning", vpeGyroBasicTuningRegister.baseTuning },
        { "adaptiveTuning", vpeGyroBasicTuningRegister.adaptiveTuning },
    };
}
void from_json(const json& j, VpeGyroBasicTuningRegister& vpeGyroBasicTuningRegister)
{
    if (j.contains("angularWalkVariance"))
    {
        j.at("angularWalkVariance").get_to(vpeGyroBasicTuningRegister.angularWalkVariance);
    }
    if (j.contains("baseTuning"))
    {
        j.at("baseTuning").get_to(vpeGyroBasicTuningRegister.baseTuning);
    }
    if (j.contains("adaptiveTuning"))
    {
        j.at("adaptiveTuning").get_to(vpeGyroBasicTuningRegister.adaptiveTuning);
    }
}

// ###########################################################################################################
//                                               INS SUBSYSTEM
// ###########################################################################################################

void to_json(json& j, const InsBasicConfigurationRegisterVn300& insBasicConfigurationRegisterVn300)
{
    j = json{
        { "scenario", insBasicConfigurationRegisterVn300.scenario },
        { "ahrsAiding", insBasicConfigurationRegisterVn300.ahrsAiding },
        { "estBaseline", insBasicConfigurationRegisterVn300.estBaseline },
    };
}
void from_json(const json& j, InsBasicConfigurationRegisterVn300& insBasicConfigurationRegisterVn300)
{
    if (j.contains("scenario"))
    {
        j.at("scenario").get_to(insBasicConfigurationRegisterVn300.scenario);
    }
    if (j.contains("ahrsAiding"))
    {
        j.at("ahrsAiding").get_to(insBasicConfigurationRegisterVn300.ahrsAiding);
    }
    if (j.contains("estBaseline"))
    {
        j.at("estBaseline").get_to(insBasicConfigurationRegisterVn300.estBaseline);
    }
}

void to_json(json& j, const StartupFilterBiasEstimateRegister& startupFilterBiasEstimateRegister)
{
    j = json{
        { "gyroBias", startupFilterBiasEstimateRegister.gyroBias },
        { "accelBias", startupFilterBiasEstimateRegister.accelBias },
        { "pressureBias", startupFilterBiasEstimateRegister.pressureBias },
    };
}
void from_json(const json& j, StartupFilterBiasEstimateRegister& startupFilterBiasEstimateRegister)
{
    if (j.contains("gyroBias"))
    {
        j.at("gyroBias").get_to(startupFilterBiasEstimateRegister.gyroBias);
    }
    if (j.contains("accelBias"))
    {
        j.at("accelBias").get_to(startupFilterBiasEstimateRegister.accelBias);
    }
    if (j.contains("pressureBias"))
    {
        j.at("pressureBias").get_to(startupFilterBiasEstimateRegister.pressureBias);
    }
}

// ###########################################################################################################
//                                    HARD/SOFT IRON ESTIMATOR SUBSYSTEM
// ###########################################################################################################

void to_json(json& j, const MagnetometerCalibrationControlRegister& magnetometerCalibrationControlRegister)
{
    j = json{
        { "hsiMode", magnetometerCalibrationControlRegister.hsiMode },
        { "hsiOutput", magnetometerCalibrationControlRegister.hsiOutput },
        { "convergeRate", magnetometerCalibrationControlRegister.convergeRate },
    };
}
void from_json(const json& j, MagnetometerCalibrationControlRegister& magnetometerCalibrationControlRegister)
{
    if (j.contains("hsiMode"))
    {
        j.at("hsiMode").get_to(magnetometerCalibrationControlRegister.hsiMode);
    }
    if (j.contains("hsiOutput"))
    {
        j.at("hsiOutput").get_to(magnetometerCalibrationControlRegister.hsiOutput);
    }
    if (j.contains("convergeRate"))
    {
        j.at("convergeRate").get_to(magnetometerCalibrationControlRegister.convergeRate);
    }
}

// ###########################################################################################################
//                                      WORLD MAGNETIC & GRAVITY MODULE
// ###########################################################################################################

void to_json(json& j, const MagneticAndGravityReferenceVectorsRegister& magneticAndGravityReferenceVectorsRegister)
{
    j = json{
        { "magRef", magneticAndGravityReferenceVectorsRegister.magRef },
        { "accRef", magneticAndGravityReferenceVectorsRegister.accRef },
    };
}
void from_json(const json& j, MagneticAndGravityReferenceVectorsRegister& magneticAndGravityReferenceVectorsRegister)
{
    if (j.contains("magRef"))
    {
        j.at("magRef").get_to(magneticAndGravityReferenceVectorsRegister.magRef);
    }
    if (j.contains("accRef"))
    {
        j.at("accRef").get_to(magneticAndGravityReferenceVectorsRegister.accRef);
    }
}

void to_json(json& j, const ReferenceVectorConfigurationRegister& referenceVectorConfigurationRegister)
{
    j = json{
        { "useMagModel", referenceVectorConfigurationRegister.useMagModel },
        { "useGravityModel", referenceVectorConfigurationRegister.useGravityModel },
        { "recalcThreshold", referenceVectorConfigurationRegister.recalcThreshold },
        { "year", referenceVectorConfigurationRegister.year },
        { "position", referenceVectorConfigurationRegister.position },
    };
}
void from_json(const json& j, ReferenceVectorConfigurationRegister& referenceVectorConfigurationRegister)
{
    if (j.contains("useMagModel"))
    {
        j.at("useMagModel").get_to(referenceVectorConfigurationRegister.useMagModel);
    }
    if (j.contains("useGravityModel"))
    {
        j.at("useGravityModel").get_to(referenceVectorConfigurationRegister.useGravityModel);
    }
    if (j.contains("recalcThreshold"))
    {
        j.at("recalcThreshold").get_to(referenceVectorConfigurationRegister.recalcThreshold);
    }
    if (j.contains("year"))
    {
        j.at("year").get_to(referenceVectorConfigurationRegister.year);
    }
    if (j.contains("position"))
    {
        j.at("position").get_to(referenceVectorConfigurationRegister.position);
    }
}

// ###########################################################################################################
//                                              VELOCITY AIDING
// ###########################################################################################################

void to_json(json& j, const VelocityCompensationControlRegister& velocityCompensationControlRegister)
{
    j = json{
        { "mode", velocityCompensationControlRegister.mode },
        { "velocityTuning", velocityCompensationControlRegister.velocityTuning },
        { "rateTuning", velocityCompensationControlRegister.rateTuning },
    };
}
void from_json(const json& j, VelocityCompensationControlRegister& velocityCompensationControlRegister)
{
    if (j.contains("mode"))
    {
        j.at("mode").get_to(velocityCompensationControlRegister.mode);
    }
    if (j.contains("velocityTuning"))
    {
        j.at("velocityTuning").get_to(velocityCompensationControlRegister.velocityTuning);
    }
    if (j.contains("rateTuning"))
    {
        j.at("rateTuning").get_to(velocityCompensationControlRegister.rateTuning);
    }
}

} // namespace sensors
} // namespace vn

const std::array<NAV::VectorNavSensor::BinaryGroupData, 15> NAV::VectorNavSensor::binaryGroupCommon = { {
    /*  0 */ { "TimeStartup", vn::protocol::uart::CommonGroup::COMMONGROUP_TIMESTARTUP, []() { ImGui::TextUnformatted("Time since startup.\n\nThe system time since startup measured in nano seconds. The time since startup is based upon the internal\nTXCO oscillator for the MCU. The accuracy of the internal TXCO is +/- 20ppm (-40C to 85C). This field is\nequivalent to the TimeStartup field in group 2."); } },
    /*  1 */ { "TimeGps", vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPS, []() { ImGui::TextUnformatted("GPS time.\n\nThe absolute GPS time since start of GPS epoch 1980 expressed in nano seconds. This field is equivalent to\nthe TimeGps field in group 2."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& bor, uint32_t& /* binaryField */) { (bor.commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPS) && (bor.timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS); } },
    /*  2 */ { "TimeSyncIn", vn::protocol::uart::CommonGroup::COMMONGROUP_TIMESYNCIN, []() { ImGui::TextUnformatted("Time since last SyncIn trigger.\n\nThe time since the last SyncIn trigger event expressed in nano seconds. This field is equivalent to the\nTimeSyncIn field in group 2."); } },
    /*  3 */ { "YawPitchRoll", vn::protocol::uart::CommonGroup::COMMONGROUP_YAWPITCHROLL, []() { ImGui::TextUnformatted("Estimated attitude as yaw pitch and roll angles.\n\nThe estimated attitude Yaw, Pitch, and Roll angles measured in degrees. The attitude is given as a 3,2,1 Euler\nangle sequence describing the body frame with respect to the local North East Down (NED) frame. This field\nis equivalent to the YawPitchRoll field in group 5.\n\nYaw [+/- 180°]\nPitch [+/- 90°]\nRoll [+/- 180°]"); } },
    /*  4 */ { "Quaternion", vn::protocol::uart::CommonGroup::COMMONGROUP_QUATERNION, []() { ImGui::TextUnformatted("Estimated attitude as a quaternion.\n\nThe estimated attitude quaternion. The last term is the scalar value. The attitude is given as the body frame\nwith respect to the local North East Down (NED) frame. This field is equivalent to the Quaternion field in\ngroup 5."); } },
    /*  5 */ { "AngularRate", vn::protocol::uart::CommonGroup::COMMONGROUP_ANGULARRATE, []() { ImGui::TextUnformatted("Compensated angular rate.\n\nThe estimated angular rate measured in rad/s. The angular rates are compensated by the onboard filter bias\nestimates. The angular rate is expressed in the body frame. This field is equivalent to the AngularRate field\nin group 3."); } },
    /*  6 */ { "Position", vn::protocol::uart::CommonGroup::COMMONGROUP_POSITION, []() { ImGui::TextUnformatted("Estimated position. (LLA)\n\nThe estimated position given as latitude, longitude, and altitude given in [deg, deg, m] respectively. This field\nis equivalent to the PosLla field in group 6."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  7 */ { "Velocity", vn::protocol::uart::CommonGroup::COMMONGROUP_VELOCITY, []() { ImGui::TextUnformatted("Estimated velocity. (NED)\n\nThe estimated velocity in the North East Down (NED) frame, given in m/s. This field is equivalent to the\nVelNed field in group 6."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  8 */ { "Accel", vn::protocol::uart::CommonGroup::COMMONGROUP_ACCEL, []() { ImGui::TextUnformatted("Estimated acceleration (compensated). (Body)\n\nThe estimated acceleration in the body frame, given in m/s^2. This acceleration includes gravity and has\nbeen bias compensated by the onboard INS Kalman filter. This field is equivalent to the Accel field in group 3."); } },
    /*  9 */ { "Imu", vn::protocol::uart::CommonGroup::COMMONGROUP_IMU, []() { ImGui::TextUnformatted("Calibrated uncompensated gyro and accelerometer measurements.\n\nThe uncompensated IMU acceleration and angular rate measurements. The acceleration is given in m/s^2,\nand the angular rate is given in rad/s. These measurements correspond to the calibrated angular rate and\nacceleration measurements straight from the IMU. The measurements have not been corrected for bias\noffset by the onboard Kalman filter. These are equivalent to the UncompAccel and UncompGyro fields in\ngroup 3."); } },
    /* 10 */ { "MagPres", vn::protocol::uart::CommonGroup::COMMONGROUP_MAGPRES, []() { ImGui::TextUnformatted("Calibrated magnetic (compensated), temperature, and pressure measurements.\n\nThe compensated magnetic, temperature, and pressure measurements from the IMU. The magnetic\nmeasurement is given in Gauss, and has been corrected for hard/soft iron corrections (if enabled). The\ntemperature measurement is given in Celsius. The pressure measurement is given in kPa. This field is\nequivalent to the Mag, Temp, and Pres fields in group 3.\n\nThe IP-68 enclosure on the tactical series forms an airtight (hermetic) seal isolating the internal\nsensors from the external environment. The pressure sensor is internal to this seal, and as such\nwill not measure the outside environment atmospheric pressure. It will instead read the pressure\ninside the sealed enclosure. The purpose of this sensor is to provide a means of ensuring the\nseal integrity over the lifetime of the product. Based on the Ideal Gas Law the ratio of pressure\ndivided by temperature should remain constant over both time and environmental temperature.\nWhen this is no longer the case, it can be assumed that the seal integrity has been compromised."); } },
    /* 11 */ { "DeltaTheta", vn::protocol::uart::CommonGroup::COMMONGROUP_DELTATHETA, []() { ImGui::TextUnformatted("Delta time, theta, and velocity.\n\nThe delta time, angle, and velocity measurements. The delta time (dtime) is the time interval that the delta\nangle and velocities are integrated over. The delta theta (dtheta) is the delta rotation angles incurred due to\nrotation, by the local body reference frame, since the last time the values were outputted by the device. The\ndelta velocity (dvel) is the delta velocity incurred due to motion, by the local body reference frame, since the\nlast time the values were outputted by the device. The frame of reference of these delta measurements are\ndetermined by the IntegrationFrame field in the Delta Theta and Delta Velocity Configuration Register\n(Register 82). These delta angles and delta velocities are calculated based upon the onboard coning and\nsculling integration performed onboard the sensor at the full IMU rate (default 800Hz). The integration for\nboth the delta angles and velocities are reset each time either of the values are either polled or sent out due\nto a scheduled asynchronous ASCII or binary output. Delta Theta and Delta Velocity values correctly capture\nthe nonlinearities involved in measuring motion from a rotating strapdown platform (as opposed to the older\nmechanically inertial navigation systems), thus providing you with the ability to integrate velocity and angular\nrate at much lower speeds (say for example 10 Hz, reducing bandwidth and computational complexity), while\nstill maintaining the same numeric precision as if you had performed the integration at the full IMU\nmeasurement rate of 800Hz. This field is equivalent to the DeltaTheta and DeltaVel fields in group 3 with the\ninclusion of the additional delta time parameter."); } },
    /* 12 */ { "InsStatus", vn::protocol::uart::CommonGroup::COMMONGROUP_INSSTATUS, []() { ImGui::TextUnformatted("INS status.\n\nThe INS status bitfield. This field is equivalent to the InsSatus field in group 6. See INS Solution LLA Register\nfor more information on the individual bits in this field."); } },
    /* 13 */ { "SyncInCnt", vn::protocol::uart::CommonGroup::COMMONGROUP_SYNCINCNT, []() { ImGui::TextUnformatted("SyncIn count.\n\nThe number of SyncIn trigger events that have occurred. This field is equivalent to the SyncInCnt field in\ngroup 2."); } },
    /* 14 */ { "TimeGpsPps", vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPSPPS, []() { ImGui::TextUnformatted("Time since last GNSS PPS trigger.\n\nThe time since the last GPS PPS trigger event expressed in nano seconds. This field is equivalent to the\nTimePPS field in group 2."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
} };

const std::array<NAV::VectorNavSensor::BinaryGroupData, 10> NAV::VectorNavSensor::binaryGroupTime = { {
    /*  0 */ { "TimeStartup", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTARTUP, []() { ImGui::TextUnformatted("Time since startup.\n\nThe system time since startup measured in nano seconds. The time since startup is based upon the internal\nTXCO oscillator for the MCU. The accuracy of the internal TXCO is +/- 20ppm (-40C to 85C)."); } },
    /*  1 */ { "TimeGps", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS, []() { ImGui::TextUnformatted("Absolute GPS time.\n\nThe absolute GPS time since start of GPS epoch 1980 expressed in nano seconds."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& bor, uint32_t& /* binaryField */) { (bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS) && (bor.timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS); } },
    /*  2 */ { "GpsTow", vn::protocol::uart::TimeGroup::TIMEGROUP_GPSTOW, []() { ImGui::TextUnformatted("Time since start of GPS week.\n\nThe time since the start of the current GPS time week expressed in nano seconds."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& bor, uint32_t& /* binaryField */) { (bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSTOW) && (bor.timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS); } },
    /*  3 */ { "GpsWeek", vn::protocol::uart::TimeGroup::TIMEGROUP_GPSWEEK, []() { ImGui::TextUnformatted("GPS week.\n\nThe current GPS week."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& bor, uint32_t& /* binaryField */) { (bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSWEEK) && (bor.timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS); } },
    /*  4 */ { "TimeSyncIn", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESYNCIN, []() { ImGui::TextUnformatted("Time since last SyncIn trigger.\n\nThe time since the last SyncIn event trigger expressed in nano seconds."); } },
    /*  5 */ { "TimeGpsPps", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPSPPS, []() { ImGui::TextUnformatted("Time since last GPS PPS trigger.\n\nThe time since the last GPS PPS trigger event expressed in nano seconds."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  6 */ { "TimeUTC", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEUTC, []() { ImGui::TextUnformatted("UTC time.\n\nThe current UTC time. The year is given as a signed byte year offset from the year 2000. For example the\nyear 2013 would be given as year 13."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& bor, uint32_t& /* binaryField */) { (bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEUTC) && (bor.timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS); } },
    /*  7 */ { "SyncInCnt", vn::protocol::uart::TimeGroup::TIMEGROUP_SYNCINCNT, []() { ImGui::TextUnformatted("SyncIn trigger count.\n\nThe number of SyncIn trigger events that have occurred."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  8 */ { "SyncOutCnt", vn::protocol::uart::TimeGroup::TIMEGROUP_SYNCOUTCNT, []() { ImGui::TextUnformatted("SyncOut trigger count.\n\nThe number of SyncOut trigger events that have occurred."); } },
    /*  9 */ { "TimeStatus", vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS, []() { ImGui::TextUnformatted("Time valid status flags.");
                                                                                         if (ImGui::BeginTable("VectorNavTimeStatusTooltip", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                                                                                         {
                                                                                             ImGui::TableSetupColumn("Bit Offset", ImGuiTableColumnFlags_WidthFixed);
                                                                                             ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed);
                                                                                             ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                                                                                             ImGui::TableHeadersRow();

                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("timeOk");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - GpsTow is valid");

                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("dateOk");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - TimeGps and GpsWeek are valid");

                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("utcTimeValid");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - UTC time is valid");

                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("3 - 7");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("resv");
                                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for future use");

                                                                                             ImGui::EndTable();
                                                                                         } }, [](VectorNavModel /* sensorModel */, const vn::sensors::BinaryOutputRegister& bor, uint32_t /* binaryField */) { return !(bor.commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPS)
                                                                                                                                                                                                                    && !(bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSTOW)
                                                                                                                                                                                                                    && !(bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSWEEK)
                                                                                                                                                                                                                    && !(bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS)
                                                                                                                                                                                                                    && !(bor.timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEUTC); } },
} };

const std::array<NAV::VectorNavSensor::BinaryGroupData, 11> NAV::VectorNavSensor::binaryGroupIMU{ {
    /*  0 */ { "ImuStatus", vn::protocol::uart::ImuGroup::IMUGROUP_IMUSTATUS, []() { ImGui::TextUnformatted("Status is reserved for future use. Not currently used in the current code, as such will always report 0."); }, [](VectorNavModel /* sensorModel */, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return false; } },
    /*  1 */ { "UncompMag", vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPMAG, []() { ImGui::TextUnformatted("Uncompensated magnetic measurement.\n\nThe IMU magnetic field measured in units of Gauss, given in the body frame. This measurement is\ncompensated by the static calibration (individual factory calibration stored in flash), and the user\ncompensation, however it is not compensated by the onboard Hard/Soft Iron estimator."); } },
    /*  2 */ { "UncompAccel", vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPACCEL, []() { ImGui::TextUnformatted("Uncompensated acceleration measurement.\n\nThe IMU acceleration measured in units of m/s^2, given in the body frame. This measurement is\ncompensated by the static calibration (individual factory calibration stored in flash), however it is not\ncompensated by any dynamic calibration such as bias compensation from the onboard INS Kalman filter."); } },
    /*  3 */ { "UncompGyro", vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPGYRO, []() { ImGui::TextUnformatted("Uncompensated angular rate measurement.\n\nThe IMU angular rate measured in units of rad/s, given in the body frame. This measurement is compensated\nby the static calibration (individual factory calibration stored in flash), however it is not compensated by any\ndynamic calibration such as the bias compensation from the onboard AHRS/INS Kalman filters."); } },
    /*  4 */ { "Temp", vn::protocol::uart::ImuGroup::IMUGROUP_TEMP, []() { ImGui::TextUnformatted("Temperature measurement.\n\nThe IMU temperature measured in units of Celsius."); } },
    /*  5 */ { "Pres", vn::protocol::uart::ImuGroup::IMUGROUP_PRES, []() { ImGui::TextUnformatted("Pressure measurement.\n\nThe IMU pressure measured in kilopascals. This is an absolute pressure measurement. Typical pressure at sea level would be around 100 kPa."); } },
    /*  6 */ { "DeltaTheta", vn::protocol::uart::ImuGroup::IMUGROUP_DELTATHETA, []() { ImGui::TextUnformatted("Delta theta angles.\n\nThe delta time (dtime) is the time interval that the delta angle and velocities are integrated over. The delta\ntheta (dtheta) is the delta rotation angles incurred due to rotation, by the local body reference frame, since\nthe last time the values were outputted by the device. The delta velocity (dvel) is the delta velocity incurred\ndue to motion, by the local body reference frame, since the last time the values were outputted by the device.\nThe frame of reference of these delta measurements are determined by the IntegrationFrame field in the\nDelta Theta and Delta Velocity Configuration Register (Register 82). These delta angles and delta velocities\nare calculated based upon the onboard coning and sculling integration performed onboard the sensor at the\nfull IMU rate (default 800Hz). The integration for both the delta angles and velocities are reset each time\neither of the values are either polled or sent out due to a scheduled asynchronous ASCII or binary output.\nDelta Theta and Delta Velocity values correctly capture the nonlinearities involved in measuring motion from\na rotating strapdown platform (as opposed to the older mechanically inertial navigation systems), thus\nproviding you with the ability to integrate velocity and angular rate at much lower speeds (say for example\n10 Hz, reducing bandwidth and computational complexity), while still maintaining the same numeric\nprecision as if you had performed the integration at the full IMU measurement rate of 800Hz. Time is given\nin seconds. Delta angles are given in degrees."); } },
    /*  7 */ { "DeltaVel", vn::protocol::uart::ImuGroup::IMUGROUP_DELTAVEL, []() { ImGui::TextUnformatted("Delta velocity.\n\nThe delta velocity (dvel) is the delta velocity incurred due to motion, since the last time the values were output\nby the device. The delta velocities are calculated based upon the onboard conning and sculling integration\nperformed onboard the sensor at the IMU sampling rate (nominally 800Hz). The integration for the delta\nvelocities are reset each time the values are either polled or sent out due to a scheduled asynchronous ASCII\nor binary output. Delta velocity is given in meters per second."); } },
    /*  8 */ { "Mag", vn::protocol::uart::ImuGroup::IMUGROUP_MAG, []() { ImGui::TextUnformatted("Compensated magnetic measurement.\n\nThe IMU compensated magnetic field measured units of Gauss, and given in the body frame. This\nmeasurement is compensated by the static calibration (individual factory calibration stored in flash), the user\ncompensation, and the dynamic calibration from the onboard Hard/Soft Iron estimator."); } },
    /*  9 */ { "Accel", vn::protocol::uart::ImuGroup::IMUGROUP_ACCEL, []() { ImGui::TextUnformatted("Compensated acceleration measurement.\n\nThe compensated acceleration measured in units of m/s^2, and given in the body frame. This measurement\nis compensated by the static calibration (individual factory calibration stored in flash), the user\ncompensation, and the dynamic bias compensation from the onboard INS Kalman filter."); } },
    /* 10 */ { "AngularRate", vn::protocol::uart::ImuGroup::IMUGROUP_ANGULARRATE, []() { ImGui::TextUnformatted("Compensated angular rate measurement.\n\nThe compensated angular rate measured in units of rad/s, and given in the body frame. This measurement\nis compensated by the static calibration (individual factor calibration stored in flash), the user compensation,\nand the dynamic bias compensation from the onboard INS Kalman filter."); } },
} };

const std::array<NAV::VectorNavSensor::BinaryGroupData, 16> NAV::VectorNavSensor::binaryGroupGNSS{ {
    /*  0 */ { "UTC", vn::protocol::uart::GpsGroup::GPSGROUP_UTC, []() { ImGui::TextUnformatted("GPS UTC Time\n\nThe current UTC time. The year is given as a signed byte year offset from the year 2000. For example the\nyear 2013 would be given as year 13."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& /* bor */, uint32_t& binaryField) { (static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_UTC) && (binaryField |= vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO); } },
    /*  1 */ { "Tow", vn::protocol::uart::GpsGroup::GPSGROUP_TOW, []() { ImGui::TextUnformatted("GPS time of week\n\nThe GPS time of week given in nano seconds."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& /* bor */, uint32_t& binaryField) { (static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_TOW) && (binaryField |= vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO); } },
    /*  2 */ { "Week", vn::protocol::uart::GpsGroup::GPSGROUP_WEEK, []() { ImGui::TextUnformatted("GPS week\n\nThe current GPS week."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; }, [](vn::sensors::BinaryOutputRegister& /* bor */, uint32_t& binaryField) { (static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK) && (binaryField |= vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO); } },
    /*  3 */ { "NumSats", vn::protocol::uart::GpsGroup::GPSGROUP_NUMSATS, []() { ImGui::TextUnformatted("Number of tracked satellites\n\nThe number of tracked GNSS satellites."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  4 */ { "Fix", vn::protocol::uart::GpsGroup::GPSGROUP_FIX, []() { ImGui::TextUnformatted("GNSS fix\n\nThe current GNSS fix.");
                                                                         if (ImGui::BeginTable("VectorNavFixTooltip", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                                                                         {
                                                                             ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
                                                                             ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                                                                             ImGui::TableHeadersRow();

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("No fix");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("Time only");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("2D");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("3D");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("SBAS");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("7");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("RTK Float (only GNSS1)");

                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("8");
                                                                             ImGui::TableNextColumn(); ImGui::TextUnformatted("RTK Fixed (only GNSS1)");

                                                                             ImGui::EndTable();
                                                                         } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  5 */ { "PosLla", vn::protocol::uart::GpsGroup::GPSGROUP_POSLLA, []() { ImGui::TextUnformatted("GNSS position (latitude, longitude, altitude)\n\nThe current GNSS position measurement given as the geodetic latitude, longitude and altitude above the\nellipsoid. The units are in [deg, deg, m] respectively."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  6 */ { "PosEcef", vn::protocol::uart::GpsGroup::GPSGROUP_POSECEF, []() { ImGui::TextUnformatted("GNSS position (ECEF)\n\nThe current GNSS position given in the Earth centered Earth fixed (ECEF) coordinate frame, given in meters."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  7 */ { "VelNed", vn::protocol::uart::GpsGroup::GPSGROUP_VELNED, []() { ImGui::TextUnformatted("GNSS velocity (NED)\n\nThe current GNSS velocity in the North East Down (NED) coordinate frame, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  8 */ { "VelEcef", vn::protocol::uart::GpsGroup::GPSGROUP_VELECEF, []() { ImGui::TextUnformatted("GNSS velocity (ECEF)\n\nThe current GNSS velocity in the Earth centered Earth fixed (ECEF) coordinate frame, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  9 */ { "PosU", vn::protocol::uart::GpsGroup::GPSGROUP_POSU, []() { ImGui::TextUnformatted("GNSS position uncertainty (NED)\n\nThe current GNSS position uncertainty in the North East Down (NED) coordinate frame, given in meters (1 Sigma)."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 10 */ { "VelU", vn::protocol::uart::GpsGroup::GPSGROUP_VELU, []() { ImGui::TextUnformatted("GNSS velocity uncertainty\n\nThe current GNSS velocity uncertainty, given in m/s (1 Sigma)."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 11 */ { "TimeU", vn::protocol::uart::GpsGroup::GPSGROUP_TIMEU, []() { ImGui::TextUnformatted("GNSS time uncertainty\n\nThe current GPS time uncertainty, given in seconds (1 Sigma)."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 12 */ { "TimeInfo", vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO, []() { ImGui::TextUnformatted("GNSS time status and leap seconds\n\nFlags for valid GPS TOW, week number and UTC and current leap seconds.");
                                                                                   if (ImGui::BeginTable("VectorNavTimeInfoTooltip", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                   {
                                                                                       ImGui::TableSetupColumn("Bit Offset");
                                                                                       ImGui::TableSetupColumn("Field");
                                                                                       ImGui::TableSetupColumn("Description");
                                                                                       ImGui::TableHeadersRow();

                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("timeOk");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - GpsTow is valid");

                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("dateOk");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - TimeGps and GpsWeek are valid");

                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("utcTimeValid");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("1 - UTC time is valid");

                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("3 - 7");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("resv");
                                                                                       ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for future use");

                                                                                       ImGui::EndTable();
                                                                                   } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t binaryField) { return sensorModel == VectorNavModel::VN310
                                                                                                                                                                                                        && !(static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                                                                                                                                                                                                        && !(static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK)
                                                                                                                                                                                                        && !(static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_UTC)
                                                                                                                                                                                                        && !(static_cast<vn::protocol::uart::GpsGroup>(binaryField) & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS); } },
    /* 13 */ { "DOP", vn::protocol::uart::GpsGroup::GPSGROUP_DOP, []() { ImGui::TextUnformatted("Dilution of precision"); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 14 */ { "SatInfo", vn::protocol::uart::GpsGroup::GPSGROUP_SATINFO, []() { ImGui::TextUnformatted("Satellite Information\n\nInformation and measurements pertaining to each GNSS satellite in view.\n\nSatInfo Element:");
                                                                                 if (ImGui::BeginTable("VectorNavSatInfoTooltip", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Name");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("sys");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GNSS constellation indicator. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("svId");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Space vehicle Id");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("flags");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Tracking info flags. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("cno");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Carrier-to-noise density ratio (signal strength) [dB-Hz]");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("qi");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Quality Indicator. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("el");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Elevation in degrees");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("az");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Azimuth angle in degrees");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::BeginChild("VectorNavSatInfoTooltipGNSSConstelationChild", ImVec2(230, 217));
                                                                                 ImGui::TextUnformatted("\nGNSS constellation indicator:");
                                                                                 if (ImGui::BeginTable("VectorNavSatInfoTooltipGNSSConstelation", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Value");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GPS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("SBAS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Galileo");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("BeiDou");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("IMES");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("QZSS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GLONASS");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::EndChild();
                                                                                 ImGui::SameLine();
                                                                                 ImGui::BeginChild("VectorNavSatInfoTooltipFlagsChild", ImVec2(260, 217));
                                                                                 ImGui::TextUnformatted("\nTracking info flags:");
                                                                                 if (ImGui::BeginTable("VectorNavSatInfoTooltipFlags", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Bit Offset");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Healthy");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Almanac");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Ephemeris");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Differential Correction");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Used for Navigation");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Azimuth / Elevation Valid");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Used for RTK");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::EndChild();
                                                                                 ImGui::TextUnformatted("\nQuality Indicators:");
                                                                                 if (ImGui::BeginTable("VectorNavSatInfoTooltipQuality", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Value");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("No signal");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Searching signal");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Signal acquired");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Signal detected but unstable");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Code locked and time synchronized");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5, 6, 7");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Code and carrier locked and time synchronized");

                                                                                     ImGui::EndTable();
                                                                                 } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 15 */ { "RawMeas", vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS, []() { ImGui::TextUnformatted("GNSS Raw Measurements.\n\nSatRaw Element:");
                                                                                 if (ImGui::BeginTable("VectorNavSatRawTooltip", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Name");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("sys");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GNSS constellation indicator. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("svId");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Space vehicle Id");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("freq");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Frequency indicator. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("chan");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Channel Indicator. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("slot");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Slot Id");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("cno");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Carrier-to-noise density ratio (signal strength) [dB-Hz]");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("flags");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Tracking info flags. See table below for details.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("pr");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Pseudorange measurement in meters.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("cp");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Carrier phase measurement in cycles.");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("dp");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Doppler measurement in Hz. Positive sign for approaching satellites.");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::BeginChild("VectorNavSatRawTooltipGNSSConstelationChild", ImVec2(180, 217));
                                                                                 ImGui::TextUnformatted("\nConstellation indicator:");
                                                                                 if (ImGui::BeginTable("VectorNavSatRawTooltipGNSSConstelation", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Value");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GPS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("SBAS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Galileo");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("BeiDou");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("IMES");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("QZSS");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("GLONASS");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::EndChild();
                                                                                 ImGui::SameLine();
                                                                                 ImGui::BeginChild("VectorNavSatRawTooltipFreqChild", ImVec2(270, 235));
                                                                                 ImGui::TextUnformatted("\nFrequency indicator:");
                                                                                 if (ImGui::BeginTable("VectorNavSatRawTooltipFreq", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Value");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Rx Channel");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("L1(GPS,QZSS,SBAS), G1(GLO),\nE2-L1-E1(GAL), B1(BDS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("L2(GPS,QZSS), G2(GLO)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("L5(GPS,QZSS,SBAS), E5a(GAL)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("E6(GAL), LEX(QZSS), B3(BDS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("E5b(GAL), B2(BDS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("E5a+b(GAL)");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::EndChild();
                                                                                 ImGui::SameLine();
                                                                                 ImGui::BeginChild("VectorNavSatRawTooltipFlagChild", ImVec2(255, 260));
                                                                                 ImGui::TextUnformatted("\nTracking info flags:");
                                                                                 if (ImGui::BeginTable("VectorNavSatRawTooltipFlags", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Bit Offset");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Searching");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Tracking");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Time Valid");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Code Lock");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Phase Lock");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Phase Half Ambiguity");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Phase Half Sub");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("7");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Phase Slip");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("8");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Pseudorange Smoothed");

                                                                                     ImGui::EndTable();
                                                                                 }
                                                                                 ImGui::EndChild();
                                                                                 ImGui::TextUnformatted("\nChannel indicator:");
                                                                                 if (ImGui::BeginTable("VectorNavSatRawTooltipChan", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                 {
                                                                                     ImGui::TableSetupColumn("Value");
                                                                                     ImGui::TableSetupColumn("Description");
                                                                                     ImGui::TableHeadersRow();

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("P-code (GPS,GLO)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("1");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("C/A-code (GPS,GLO,SBAS,QZSS), C chan (GAL)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("semi-codeless (GPS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Y-code (GPS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("M-code (GPS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("5");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("codeless (GPS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("A chan (GAL)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("7");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("B chan (GAL)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("8");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("I chan (GPS,GAL,QZSS,BDS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("9");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("Q chan (GPS,GAL,QZSS,BDS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("10");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("M chan (L2CGPS, L2CQZSS), D chan (GPS,QZSS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("11");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("L chan (L2CGPS, L2CQZSS), P chan (GPS,QZSS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("12");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("B+C chan (GAL), I+Q chan (GPS,GAL,QZSS,BDS),\nM+L chan (GPS,QZSS), D+P chan (GPS,QZSS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("13");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("based on Z-tracking (GPS)");

                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("14");
                                                                                     ImGui::TableNextColumn(); ImGui::TextUnformatted("A+B+C (GAL)");

                                                                                     ImGui::EndTable();
                                                                                 } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
} };

const std::array<NAV::VectorNavSensor::BinaryGroupData, 9> NAV::VectorNavSensor::binaryGroupAttitude{ {
    /*  0 */ { "VpeStatus", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_VPESTATUS, []() { ImGui::TextUnformatted("VPE Status bitfield\n\n");
                                                                                               if (ImGui::BeginTable("VectorNavSatRawTooltipChan", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                                                                                               {
                                                                                                   ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                                                                                   ImGui::TableSetupColumn("Bit Offset", ImGuiTableColumnFlags_WidthFixed);
                                                                                                   ImGui::TableSetupColumn("Format", ImGuiTableColumnFlags_WidthFixed);
                                                                                                   ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                                                                                                   ImGui::TableHeadersRow();

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("AttitudeQuality");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("2 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Provides an indication of the quality of the attitude solution.\n0 - Excellent\n1 - Good\n2 - Bad\n3 - Not tracking");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("GyroSaturation");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("At least one gyro axis is currently saturated.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("GyroSaturationRecovery");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Filter is in the process of recovering from a gyro\nsaturation event.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("MagDisturbance");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("4");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("2 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("A magnetic DC disturbance has been detected.\n0 - No magnetic disturbance\n1 to 3 - Magnetic disturbance is present.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("MagSaturation");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("6");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("At least one magnetometer axis is currently saturated.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("AccDisturbance");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("7");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("2 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("A strong acceleration disturbance has been detected.\n0 - No acceleration disturbance.\n1 to 3 - Acceleration disturbance has been detected.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("AccSaturation");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("9");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("At least one accelerometer axis is currently saturated.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("10");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for internal use. May change state at run- time.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("KnownMagDisturbance");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("11");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("A known magnetic disturbance has been reported by\nthe user and the magnetometer is currently tuned out.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("KnownAccelDisturbance");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("12");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("A known acceleration disturbance has been reported by\nthe user and the accelerometer is currently tuned out.");

                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("13");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("3 bits");
                                                                                                   ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for future use.");

                                                                                                   ImGui::EndTable();
                                                                                               } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN100_VN110; } },
    /*  1 */ { "YawPitchRoll", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_YAWPITCHROLL, []() { ImGui::TextUnformatted("Yaw Pitch Roll\n\nThe estimated attitude Yaw, Pitch, and Roll angles measured in degrees. The attitude is given as a 3,2,1\nEuler angle sequence describing the body frame with respect to the local North East Down (NED) frame.\n\nYaw [+/- 180°]\nPitch [+/- 90°]\nRoll [+/- 180°]"); } },
    /*  2 */ { "Quaternion", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_QUATERNION, []() { ImGui::TextUnformatted("Quaternion\n\nThe estimated attitude quaternion. The last term is the scalar value. The attitude is given as the body\nframe with respect to the local North East Down (NED) frame."); } },
    /*  3 */ { "DCM", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_DCM, []() { ImGui::TextUnformatted("Directional Cosine Matrix\n\nThe estimated attitude directional cosine matrix given in column major order. The DCM maps vectors\nfrom the North East Down (NED) frame into the body frame."); } },
    /*  4 */ { "MagNed", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_MAGNED, []() { ImGui::TextUnformatted("Compensated magnetic (NED)\n\nThe current estimated magnetic field (Gauss), given in the North East Down (NED) frame. The current\nattitude solution is used to map the measurement from the measured body frame to the inertial (NED)\nframe. This measurement is compensated by both the static calibration (individual factory calibration\nstored in flash), and the dynamic calibration such as the user or onboard Hard/Soft Iron compensation\nregisters."); } },
    /*  5 */ { "AccelNed", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_ACCELNED, []() { ImGui::TextUnformatted("Compensated acceleration (NED)\n\nThe estimated acceleration (with gravity) reported in m/s^2, given in the North East Down (NED) frame.\nThe acceleration measurement has been bias compensated by the onboard INS filter. This measurement\nis attitude dependent, since the attitude is used to map the measurement from the body frame into the\ninertial (NED) frame. If the device is stationary and the INS filter is tracking, the measurement should be\nnominally equivalent to the gravity reference vector in the inertial frame (NED)."); } },
    /*  6 */ { "LinearAccelBody", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_LINEARACCELBODY, []() { ImGui::TextUnformatted("Compensated linear acceleration (no gravity)\n\nThe estimated linear acceleration (without gravity) reported in m/s^2, and given in the body frame. The\nacceleration measurement has been bias compensated by the onboard INS filter, and the gravity\ncomponent has been removed using the current gravity reference vector model. This measurement is\nattitude dependent, since the attitude solution is required to map the gravity reference vector (known in\nthe inertial NED frame), into the body frame so that it can be removed from the measurement. If the\ndevice is stationary and the onboard INS filter is tracking, the measurement nominally will read 0 in all\nthree axes."); } },
    /*  7 */ { "LinearAccelNed", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_LINEARACCELNED, []() { ImGui::TextUnformatted("Compensated linear acceleration (no gravity) (NED)\n\nThe estimated linear acceleration (without gravity) reported in m/s^2, and given in the North East Down\n(NED) frame. This measurement is attitude dependent as the attitude solution is used to map the\nmeasurement from the body frame into the inertial (NED) frame. This acceleration measurement has\nbeen bias compensated by the onboard INS filter, and the gravity component has been removed using the\ncurrent gravity reference vector estimate. If the device is stationary and the onboard INS filter is tracking,\nthe measurement nominally will read 0 in all three axes."); } },
    /*  8 */ { "YprU", vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_YPRU, []() { ImGui::TextUnformatted("Yaw Pitch Roll uncertainty\n\nThe estimated attitude (Yaw, Pitch, Roll) uncertainty (1 Sigma), reported in degrees.\n\nThe estimated attitude (YprU) field is not valid when the INS Scenario mode in the INS Basic\nConfiguration register is set to AHRS mode. See the INS Basic Configuration Register in the INS\nsection for more details."); } },
} };

const std::array<NAV::VectorNavSensor::BinaryGroupData, 11> NAV::VectorNavSensor::binaryGroupINS{ {
    /*  0 */ { "InsStatus", vn::protocol::uart::InsGroup::INSGROUP_INSSTATUS, []() { ImGui::TextUnformatted("Ins Status bitfield:");
                                                                                     if (ImGui::BeginTable("VectorNavInsStatusTooltip", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                                                                                     {
                                                                                         ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
                                                                                         ImGui::TableSetupColumn("Bit Offset", ImGuiTableColumnFlags_WidthFixed);
                                                                                         ImGui::TableSetupColumn("Format", ImGuiTableColumnFlags_WidthFixed);
                                                                                         ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthFixed);
                                                                                         ImGui::TableHeadersRow();

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Mode");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("0");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("2 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Indicates the current mode of the INS filter.\n\n0 = Not tracking. GNSS Compass is initializing. Output heading is based on\nmagnetometer measurements.\n1 = Aligning.\nINS Filter is dynamically aligning.\nFor a stationary startup: GNSS Compass has initialized and INS Filter is\naligning from the magnetic heading to the GNSS Compass heading.\nFor a dynamic startup: INS Filter has initialized and is dynamically aligning to\nTrue North heading.\nIn operation, if the INS Filter drops from INS Mode 2 back down to 1, the\nattitude uncertainty has increased above 2 degrees.\n2 = Tracking. The INS Filter is tracking and operating within specification.\n3 = Loss of GNSS. A GNSS outage has lasted more than 45 seconds. The INS\nFilter will no longer update the position and velocity outputs, but the attitude\nremains valid.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("GpsFix");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("2");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Indicates whether the GNSS has a proper fix.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Error");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("3");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("4 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Sensor measurement error code. See table below.\n0 = No errors detected.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("7");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for internal use. May toggle state during runtime and should be ignored.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("GpsHeadingIns");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("8");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("In stationary operation, if set the INS Filter has fully aligned to the GNSS\nCompass solution.\nIn dynamic operation, the GNSS Compass solution is currently aiding the INS\nFilter heading solution.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("GpsCompass");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("9");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("1 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Indicates if the GNSS compass is operational and reporting a heading\nsolution.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("10");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("6 bits");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for internal use. These bits will toggle state and should be ignored.");

                                                                                         ImGui::EndTable();
                                                                                     }
                                                                                     ImGui::TextUnformatted("\nError Bitfield:");
                                                                                     if (ImGui::BeginTable("VectorNavInsStatusTooltipError", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                                                                                     {
                                                                                         ImGui::TableSetupColumn("Name");
                                                                                         ImGui::TableSetupColumn("Description");
                                                                                         ImGui::TableHeadersRow();

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Reserved for future use and not currently used.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("IMU Error");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("High if IMU communication error is detected.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("Mag/Pres Error");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("High if Magnetometer or Pressure sensor error is detected.");

                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("GNSS Error");
                                                                                         ImGui::TableNextColumn(); ImGui::TextUnformatted("High if GNSS communication error is detected.");

                                                                                         ImGui::EndTable();
                                                                                     } }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  1 */ { "PosLla", vn::protocol::uart::InsGroup::INSGROUP_POSLLA, []() { ImGui::TextUnformatted("Ins Position (latitude, longitude, altitude)\n\nThe estimated position given as latitude, longitude, and altitude given in [deg, deg, m] respectively."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  2 */ { "PosEcef", vn::protocol::uart::InsGroup::INSGROUP_POSECEF, []() { ImGui::TextUnformatted("Ins Position (ECEF)\n\nThe estimated position given in the Earth centered Earth fixed (ECEF) frame, reported in meters."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  3 */ { "VelBody", vn::protocol::uart::InsGroup::INSGROUP_VELBODY, []() { ImGui::TextUnformatted("Ins Velocity (Body)\n\nThe estimated velocity in the body frame, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  4 */ { "VelNed", vn::protocol::uart::InsGroup::INSGROUP_VELNED, []() { ImGui::TextUnformatted("Ins Velocity (NED)\n\nThe estimated velocity in the North East Down (NED) frame, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  5 */ { "VelEcef", vn::protocol::uart::InsGroup::INSGROUP_VELECEF, []() { ImGui::TextUnformatted("Ins Velocity (ECEF)\n\nThe estimated velocity in the Earth centered Earth fixed (ECEF) frame, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  6 */ { "MagEcef", vn::protocol::uart::InsGroup::INSGROUP_MAGECEF, []() { ImGui::TextUnformatted("Compensated magnetic (ECEF)\n\nThe compensated magnetic measurement in the Earth centered Earth fixed (ECEF) frame, given in Gauss."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  7 */ { "AccelEcef", vn::protocol::uart::InsGroup::INSGROUP_ACCELECEF, []() { ImGui::TextUnformatted("Compensated acceleration (ECEF)\n\nThe estimated acceleration (with gravity) reported in m/s^2, given in the Earth centered Earth fixed (ECEF)\nframe. The acceleration measurement has been bias compensated by the onboard INS filter. This\nmeasurement is attitude dependent, since the attitude is used to map the measurement from the body frame\ninto the inertial (ECEF) frame. If the device is stationary and the INS filter is tracking, the measurement\nshould be nominally equivalent to the gravity reference vector in the inertial frame (ECEF)."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  8 */ { "LinearAccelEcef", vn::protocol::uart::InsGroup::INSGROUP_LINEARACCELECEF, []() { ImGui::TextUnformatted("Compensated linear acceleration (no gravity) (ECEF)\n\nThe estimated linear acceleration (without gravity) reported in m/s^2, and given in the Earth centered Earth\nfixed (ECEF) frame. This measurement is attitude dependent as the attitude solution is used to map the\nmeasurement from the body frame into the inertial (ECEF) frame. This acceleration measurement has been\nbias compensated by the onboard INS filter, and the gravity component has been removed using the current\ngravity reference vector estimate. If the device is stationary and the onboard INS filter is tracking, the\nmeasurement will nominally read 0 in all three axes."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /*  9 */ { "PosU", vn::protocol::uart::InsGroup::INSGROUP_POSU, []() { ImGui::TextUnformatted("Ins Position Uncertainty\n\nThe estimated uncertainty (1 Sigma) in the current position estimate, given in meters."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
    /* 10 */ { "VelU", vn::protocol::uart::InsGroup::INSGROUP_VELU, []() { ImGui::TextUnformatted("Ins Velocity Uncertainty\n\nThe estimated uncertainty (1 Sigma) in the current velocity estimate, given in m/s."); }, [](VectorNavModel sensorModel, const vn::sensors::BinaryOutputRegister& /* bor */, uint32_t /* binaryField */) { return sensorModel == VectorNavModel::VN310; } },
} };

NAV::VectorNavSensor::VectorNavSensor()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);

    hasConfig = true;
    guiConfigDefaultWindowSize = { 954, 783 };

    nm::CreateOutputPin(this, "Ascii Output", Pin::Type::Flow, { NAV::StringObs::type() });
    nm::CreateOutputPin(this, "Binary Output 1", Pin::Type::Flow, { NAV::VectorNavBinaryOutput::type() });
    nm::CreateOutputPin(this, "Binary Output 2", Pin::Type::Flow, { NAV::VectorNavBinaryOutput::type() });
    nm::CreateOutputPin(this, "Binary Output 3", Pin::Type::Flow, { NAV::VectorNavBinaryOutput::type() });

    dividerFrequency = []() {
        std::map<int, int, std::greater<>> divFreq;
        for (int freq = 1; freq <= IMU_DEFAULT_FREQUENCY; freq++)
        {
            int divider = static_cast<int>(std::round(IMU_DEFAULT_FREQUENCY / freq));

            if (!divFreq.contains(divider)
                || std::abs(divider - IMU_DEFAULT_FREQUENCY / freq) < std::abs(divider - IMU_DEFAULT_FREQUENCY / divFreq.at(divider)))
            {
                divFreq[divider] = freq;
            }
        }
        std::vector<uint16_t> divs;
        std::vector<std::string> freqs;
        for (auto& [divider, freq] : divFreq)
        {
            divs.push_back(static_cast<uint16_t>(divider));
            freqs.push_back(std::to_string(freq) + " Hz");
            LOG_DEBUG("VectorNavSensor: RateDivisor {} = {}", divs.back(), freqs.back());
        }
        return std::make_pair(divs, freqs);
    }();
}

NAV::VectorNavSensor::~VectorNavSensor()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::VectorNavSensor::typeStatic()
{
    return "VectorNavSensor";
}

std::string NAV::VectorNavSensor::type() const
{
    return typeStatic();
}

std::string NAV::VectorNavSensor::category()
{
    return "Data Provider";
}

void NAV::VectorNavSensor::guiConfig()
{
    if (ImGui::Combo("Sensor", reinterpret_cast<int*>(&sensorModel), "VN-100 / VN-110\0VN-310\0\0"))
    {
        LOG_DEBUG("{}: Sensor changed to {}", nameId(), sensorModel);
        flow::ApplyChanges();
        deinitializeNode();

        if (sensorModel != VectorNavModel::VN310
            && (asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNGPS
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNGPE
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNINS
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNINE
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNISL
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNISE
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNG2S
                || asyncDataOutputType == vn::protocol::uart::AsciiAsync::VNG2E))
        {
            asyncDataOutputType = vn::protocol::uart::AsciiAsync::VNOFF;
        }

        if (sensorModel == VectorNavModel::VN310)
        {
            communicationProtocolControlRegister.spiCount = vn::protocol::uart::CountMode::COUNTMODE_NONE;
            communicationProtocolControlRegister.spiStatus = vn::protocol::uart::StatusMode::STATUSMODE_OFF;
            communicationProtocolControlRegister.spiChecksum = vn::protocol::uart::ChecksumMode::CHECKSUMMODE_OFF;
        }

        for (auto& binaryOutput : binaryOutputRegister)
        {
            for (const auto& item : binaryGroupCommon)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.commonField)))
                {
                    binaryOutput.commonField &= ~vn::protocol::uart::CommonGroup(item.flagsValue);
                }
            }
            for (const auto& item : binaryGroupTime)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.timeField)))
                {
                    binaryOutput.timeField &= ~vn::protocol::uart::TimeGroup(item.flagsValue);
                }
            }
            for (const auto& item : binaryGroupIMU)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.imuField)))
                {
                    binaryOutput.imuField &= ~vn::protocol::uart::ImuGroup(item.flagsValue);
                }
            }
            for (const auto& item : binaryGroupGNSS)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.gpsField)))
                {
                    binaryOutput.gpsField &= ~vn::protocol::uart::GpsGroup(item.flagsValue);
                }
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.gps2Field)))
                {
                    binaryOutput.gps2Field &= ~vn::protocol::uart::GpsGroup(item.flagsValue);
                }
            }
            for (const auto& item : binaryGroupAttitude)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.attitudeField)))
                {
                    binaryOutput.attitudeField &= ~vn::protocol::uart::AttitudeGroup(item.flagsValue);
                }
            }
            for (const auto& item : binaryGroupINS)
            {
                if (!item.isEnabled(sensorModel, binaryOutput, static_cast<uint32_t>(binaryOutput.insField)))
                {
                    binaryOutput.insField &= ~vn::protocol::uart::InsGroup(item.flagsValue);
                }
            }
        }
    }

    if (ImGui::InputTextWithHint("SensorPort", "/dev/ttyUSB0", &sensorPort))
    {
        LOG_DEBUG("{}: SensorPort changed to {}", nameId(), sensorPort);
        flow::ApplyChanges();
        deinitializeNode();
    }
    ImGui::SameLine();
    gui::widgets::HelpMarker("COM port where the sensor is attached to\n"
                             "- \"COM1\" (Windows format for physical and virtual (USB) serial port)\n"
                             "- \"/dev/ttyS1\" (Linux format for physical serial port)\n"
                             "- \"/dev/ttyUSB0\" (Linux format for virtual (USB) serial port)\n"
                             "- \"/dev/tty.usbserial-FTXXXXXX\" (Mac OS X format for virtual (USB) serial port)\n"
                             "- \"/dev/ttyS0\" (CYGWIN format. Usually the Windows COM port number minus 1. This would connect to COM1)");

    // ###########################################################################################################
    //                                               SYSTEM MODULE
    // ###########################################################################################################

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader(fmt::format("System Module##{}", size_t(id)).c_str()))
    {
        // ------------------------------------------- Serial Baud Rate ----------------------------------------------

        std::array<const char*, 10> items = { "Fastest", "9600", "19200", "38400", "57600", "115200", "128000", "230400", "460800", "921600" };
        if (ImGui::Combo("Baudrate", &selectedBaudrate, items.data(), items.size()))
        {
            LOG_DEBUG("{}: Baudrate changed to {}", nameId(), sensorBaudrate());
            flow::ApplyChanges();
            deinitializeNode();
        }

        if (ImGui::TreeNode(fmt::format("Async Ascii Output##{}", size_t(id)).c_str()))
        {
            std::vector<std::pair<vn::protocol::uart::AsciiAsync, const char*>> asciiAsyncItems{
                { vn::protocol::uart::AsciiAsync::VNOFF, "Asynchronous output turned off" },
                { vn::protocol::uart::AsciiAsync::VNYPR, "Yaw, Pitch, Roll" },
                { vn::protocol::uart::AsciiAsync::VNQTN, "Quaternion" },
                { vn::protocol::uart::AsciiAsync::VNQMR, "Quaternion, Magnetic, Acceleration and Angular Rates" },
                { vn::protocol::uart::AsciiAsync::VNDCM, "Directional Cosine Orientation Matrix" },
                { vn::protocol::uart::AsciiAsync::VNMAG, "Magnetic Measurements" },
                { vn::protocol::uart::AsciiAsync::VNACC, "Acceleration Measurements" },
                { vn::protocol::uart::AsciiAsync::VNGYR, "Angular Rate Measurements" },
                { vn::protocol::uart::AsciiAsync::VNMAR, "Magnetic, Acceleration, and Angular Rate Measurements" },
                { vn::protocol::uart::AsciiAsync::VNYMR, "Yaw, Pitch, Roll, Magnetic, Acceleration, and Angular Rate Measurements" },
                { vn::protocol::uart::AsciiAsync::VNYBA, "Yaw, Pitch, Roll, Body True Acceleration, and Angular Rates" },
                { vn::protocol::uart::AsciiAsync::VNYIA, "Yaw, Pitch, Roll, Inertial True Acceleration, and Angular Rates" },
                { vn::protocol::uart::AsciiAsync::VNIMU, "IMU Measurements" }
            };
            if (sensorModel == VectorNavModel::VN310)
            {
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNGPS, "GNSS LLA");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNGPE, "GNSS ECEF");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNINS, "INS LLA");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNINE, "INS ECEF");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNISL, "INS LLA 2");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNISE, "INS ECEF 2");
            }
            asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNDTV, "Delta theta and delta velocity");
            if (sensorModel == VectorNavModel::VN310)
            {
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNG2S, "GNSS2 LLA");
                asciiAsyncItems.emplace_back(vn::protocol::uart::AsciiAsync::VNG2E, "GNSS2 ECEF");
            }

            if (ImGui::BeginCombo(fmt::format("Async Ascii Output Type##{}", size_t(id)).c_str(), vn::protocol::uart::str(asyncDataOutputType).c_str()))
            {
                for (const auto& asciiAsyncItem : asciiAsyncItems)
                {
                    const bool isSelected = (asyncDataOutputType == asciiAsyncItem.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(asciiAsyncItem.first).c_str(), isSelected))
                    {
                        asyncDataOutputType = asciiAsyncItem.first;
                        LOG_DEBUG("{}: asyncDataOutputType changed to {}", nameId(), vn::protocol::uart::str(asyncDataOutputType));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeAsyncDataOutputType(asyncDataOutputType);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the asyncDataOutputType: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(asciiAsyncItem.second);
                        ImGui::EndTooltip();
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("This register controls the type of data that will be asynchronously outputted by the module. With this "
                                     "register, the user can specify which data register will be automatically outputted when it gets updated "
                                     "with a new reading. The table below lists which registers can be set to asynchronously output, the value "
                                     "to specify which register to output, and the header of the asynchronous data packet. Asynchronous data "
                                     "output can be disabled by setting this register to zero. The asynchronous data output will be sent out "
                                     "automatically at a frequency specified by the Async Data Output Frequency Register.");

            if (ImGui::SliderInt(fmt::format("Async Ascii Output Frequency##{}", size_t(id)).c_str(),
                                 &asyncDataOutputFrequencySelected,
                                 0, possibleAsyncDataOutputFrequency.size() - 1,
                                 fmt::format("{} Hz", possibleAsyncDataOutputFrequency.at(static_cast<size_t>(asyncDataOutputFrequencySelected))).c_str()))
            {
                asyncDataOutputFrequency = static_cast<uint32_t>(possibleAsyncDataOutputFrequency.at(static_cast<size_t>(asyncDataOutputFrequencySelected)));
                LOG_DEBUG("{}: asyncDataOutputType changed to {} Hz", nameId(), asyncDataOutputFrequency);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeAsyncDataOutputFrequency(asyncDataOutputFrequency);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the asyncDataOutputFrequency: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("Asynchronous data output frequency.\nThe ADOF will be changed for the active serial port.");

            if (ImGui::DragInt(fmt::format("Async Ascii Output buffer size##{}", size_t(id)).c_str(), &asciiOutputBufferSize, 1.0F, 0, INT32_MAX / 2))
            {
                asciiOutputBuffer.resize(static_cast<size_t>(asciiOutputBufferSize));
                LOG_DEBUG("{}: asciiOutputBufferSize changed to {}", nameId(), asciiOutputBufferSize);
                flow::ApplyChanges();
            }

            std::string messages;
            for (size_t i = 0; i < asciiOutputBuffer.size(); i++)
            {
                messages.append(asciiOutputBuffer.at(i));
            }
            ImGui::TextUnformatted("Async Ascii Messages:");
            ImGui::BeginChild(fmt::format("##Ascii Mesages {}", size_t(id)).c_str(), ImVec2(0, 300), true);
            ImGui::PushTextWrapPos();
            ImGui::TextUnformatted(messages.c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndChild();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(fmt::format("Synchronization Control##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Contains parameters which allow the timing of the VN-310E to be\n"
                                   "synchronized with external devices.");

            static constexpr std::array<std::pair<vn::protocol::uart::SyncInMode, const char*>, 4> synchronizationControlSyncInModes = {
                { { vn::protocol::uart::SyncInMode::SYNCINMODE_COUNT, "Count number of trigger events on SYNC_IN" },
                  { vn::protocol::uart::SyncInMode::SYNCINMODE_IMU, "Start IMU sampling on trigger of SYNC_IN" },
                  { vn::protocol::uart::SyncInMode::SYNCINMODE_ASYNC, "Output asynchronous message on trigger of SYNC_IN" },
                  { vn::protocol::uart::SyncInMode::SYNCINMODE_ASYNC3, "Output asynchronous or binary messages configured with a rate of 0 to output on trigger of SYNC_IN\n\n"
                                                                       "In ASYNC3 mode messages configured with an output rate = 0 are output each time the appropriate\n"
                                                                       "transistion edge of the SyncIn pin is captured according to the edge settings in the SyncInEdge field.\n"
                                                                       "Messages configured with output rate > 0 are not affected by the SyncIn pulse. This applies to the ASCII\n"
                                                                       "Async message set by the Async Data Output Register, the user configurate binary output messages set\n"
                                                                       "by the Binary Output Registers, as well as the NMEA messages configured by the NMEA Output Registers." } }
            };
            if (ImGui::BeginCombo(fmt::format("SyncIn Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(synchronizationControlRegister.syncInMode).c_str()))
            {
                for (const auto& synchronizationControlSyncInMode : synchronizationControlSyncInModes)
                {
                    const bool isSelected = (synchronizationControlRegister.syncInMode == synchronizationControlSyncInMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(synchronizationControlSyncInMode.first).c_str(), isSelected))
                    {
                        synchronizationControlRegister.syncInMode = synchronizationControlSyncInMode.first;
                        LOG_DEBUG("{}: synchronizationControlRegister.syncInMode changed to {}", nameId(), vn::protocol::uart::str(synchronizationControlRegister.syncInMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeSynchronizationControl(synchronizationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(synchronizationControlSyncInMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncInMode register controls the behavior of the SyncIn event. If the mode is set to COUNT then the "
                                     "internal clock will be used to control the IMU sampling. If SyncInMode is set to IMU then the IMU sampling "
                                     "loop will run on a SyncIn event. The relationship between the SyncIn event and a SyncIn trigger is defined "
                                     "by the SyncInEdge and SyncInSkipFactor parameters. If set to ASYNC then the VN-100 will output "
                                     "asynchronous serial messages upon each trigger event.");

            static constexpr std::array<std::pair<vn::protocol::uart::SyncInEdge, const char*>, 2> synchronizationControlSyncInEdges = {
                { { vn::protocol::uart::SyncInEdge::SYNCINEDGE_RISING, "Trigger on rising edge" },
                  { vn::protocol::uart::SyncInEdge::SYNCINEDGE_FALLING, "Trigger on falling edge" } }
            };
            if (ImGui::BeginCombo(fmt::format("SyncIn Edge##{}", size_t(id)).c_str(), vn::protocol::uart::str(synchronizationControlRegister.syncInEdge).c_str()))
            {
                for (const auto& synchronizationControlSyncInEdge : synchronizationControlSyncInEdges)
                {
                    const bool isSelected = (synchronizationControlRegister.syncInEdge == synchronizationControlSyncInEdge.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(synchronizationControlSyncInEdge.first).c_str(), isSelected))
                    {
                        synchronizationControlRegister.syncInEdge = synchronizationControlSyncInEdge.first;
                        LOG_DEBUG("{}: synchronizationControlRegister.syncInEdge changed to {}", nameId(), vn::protocol::uart::str(synchronizationControlRegister.syncInEdge));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeSynchronizationControl(synchronizationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(synchronizationControlSyncInEdge.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncInEdge register controls the type of edge the signal is set to trigger on.\n"
                                     "The factory default state is to trigger on a rising edge.");

            int syncInSkipFactor = synchronizationControlRegister.syncInSkipFactor;
            if (ImGui::InputInt(fmt::format("SyncIn Skip Factor##{}", size_t(id)).c_str(), &syncInSkipFactor))
            {
                if (syncInSkipFactor < 0)
                {
                    syncInSkipFactor = 0;
                }
                else if (syncInSkipFactor > std::numeric_limits<uint16_t>::max())
                {
                    syncInSkipFactor = std::numeric_limits<uint16_t>::max();
                }
                synchronizationControlRegister.syncInSkipFactor = static_cast<uint16_t>(syncInSkipFactor);
                LOG_DEBUG("{}: synchronizationControlRegister.syncInSkipFactor changed to {}", nameId(), synchronizationControlRegister.syncInSkipFactor);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeSynchronizationControl(synchronizationControlRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncInSkipFactor defines how many times trigger edges defined by SyncInEdge should occur prior to "
                                     "triggering a SyncIn event. The action performed on a SyncIn event is determined by the SyncIn mode. As an "
                                     "example if the SyncInSkipFactor was set to 4 and a 1 kHz signal was attached to the SyncIn pin, then the "
                                     "SyncIn event would only occur at 200 Hz.");

            static constexpr std::array<std::pair<vn::protocol::uart::SyncOutMode, const char*>, 5> synchronizationControlSyncOutModes = {
                { { vn::protocol::uart::SyncOutMode::SYNCOUTMODE_NONE, "None" },
                  { vn::protocol::uart::SyncOutMode::SYNCOUTMODE_ITEMSTART, "Trigger at start of IMU sampling" },
                  { vn::protocol::uart::SyncOutMode::SYNCOUTMODE_IMUREADY, "Trigger when IMU measurements are available" },
                  { vn::protocol::uart::SyncOutMode::SYNCOUTMODE_INS, "Trigger when attitude measurements are available" },
                  { vn::protocol::uart::SyncOutMode::SYNCOUTMODE_GPSPPS, "Trigger on a GPS PPS event (1 Hz) when a 3D fix is valid." } }
            };
            if (ImGui::BeginCombo(fmt::format("SyncOut Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(synchronizationControlRegister.syncOutMode).c_str()))
            {
                for (const auto& synchronizationControlSyncOutMode : synchronizationControlSyncOutModes)
                {
                    const bool isSelected = (synchronizationControlRegister.syncOutMode == synchronizationControlSyncOutMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(synchronizationControlSyncOutMode.first).c_str(), isSelected))
                    {
                        synchronizationControlRegister.syncOutMode = synchronizationControlSyncOutMode.first;
                        LOG_DEBUG("{}: synchronizationControlRegister.syncOutMode changed to {}", nameId(), vn::protocol::uart::str(synchronizationControlRegister.syncOutMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeSynchronizationControl(synchronizationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(synchronizationControlSyncOutMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncOutMode register controls the behavior of the SyncOut pin. If this is set to IMU then the SyncOut "
                                     "will start the pulse when the internal IMU sample loop starts. This mode is used to make a sensor the Master "
                                     "in a multi-sensor network array. If this is set to IMU_READY mode then the pulse will start when IMU "
                                     "measurements become available. If this is set to INS mode then the pulse will start when attitude "
                                     "measurements are made available. Changes to this register take effect immediately.");

            static constexpr std::array<std::pair<vn::protocol::uart::SyncOutPolarity, const char*>, 2> synchronizationControlSyncOutPolarities = {
                { { vn::protocol::uart::SyncOutPolarity::SYNCOUTPOLARITY_NEGATIVE, "Negative Pulse" },
                  { vn::protocol::uart::SyncOutPolarity::SYNCOUTPOLARITY_POSITIVE, "Positive Pulse" } }
            };
            if (ImGui::BeginCombo(fmt::format("SyncOut Polarity##{}", size_t(id)).c_str(), vn::protocol::uart::str(synchronizationControlRegister.syncOutPolarity).c_str()))
            {
                for (const auto& synchronizationControlSyncOutPolarity : synchronizationControlSyncOutPolarities)
                {
                    const bool isSelected = (synchronizationControlRegister.syncOutPolarity == synchronizationControlSyncOutPolarity.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(synchronizationControlSyncOutPolarity.first).c_str(), isSelected))
                    {
                        synchronizationControlRegister.syncOutPolarity = synchronizationControlSyncOutPolarity.first;
                        LOG_DEBUG("{}: synchronizationControlRegister.syncOutPolarity changed to {}", nameId(), vn::protocol::uart::str(synchronizationControlRegister.syncOutPolarity));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeSynchronizationControl(synchronizationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(synchronizationControlSyncOutPolarity.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncOutPolarity register controls the polarity of the output pulse on the SyncOut pin.\n"
                                     "Changes to this register take effect immediately.");

            int syncOutSkipFactor = synchronizationControlRegister.syncOutSkipFactor;
            if (ImGui::InputInt(fmt::format("SyncOut Skip Factor##{}", size_t(id)).c_str(), &syncOutSkipFactor))
            {
                if (syncOutSkipFactor < 0)
                {
                    syncOutSkipFactor = 0;
                }
                else if (syncOutSkipFactor > std::numeric_limits<uint16_t>::max())
                {
                    syncOutSkipFactor = std::numeric_limits<uint16_t>::max();
                }
                synchronizationControlRegister.syncOutSkipFactor = static_cast<uint16_t>(syncOutSkipFactor);
                LOG_DEBUG("{}: synchronizationControlRegister.syncOutSkipFactor changed to {}", nameId(), synchronizationControlRegister.syncOutSkipFactor);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeSynchronizationControl(synchronizationControlRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncOutSkipFactor defines how many times the sync out event should be skipped before actually triggering the SyncOut pin.");

            int syncOutPulseWidth = static_cast<int>(synchronizationControlRegister.syncOutPulseWidth);
            if (ImGui::InputInt(fmt::format("SyncOut Pulse Width##{}", size_t(id)).c_str(), &syncOutPulseWidth))
            {
                if (syncOutPulseWidth < 0)
                {
                    syncOutPulseWidth = 0;
                }
                synchronizationControlRegister.syncOutPulseWidth = static_cast<uint32_t>(syncOutPulseWidth);
                LOG_DEBUG("{}: synchronizationControlRegister.syncOutPulseWidth changed to {}", nameId(), synchronizationControlRegister.syncOutPulseWidth);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeSynchronizationControl(synchronizationControlRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the synchronizationControlRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SyncOutPulseWidth field controls the desired width of the SyncOut pulse.\n"
                                     "The default value is 100,000,000 (100 ms).");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(fmt::format("Communication Protocol Control##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Contains parameters that controls the communication protocol used by the sensor.");

            static constexpr std::array<std::pair<vn::protocol::uart::CountMode, const char*>, 5> communicationProtocolControlSerialCounts = {
                { { vn::protocol::uart::CountMode::COUNTMODE_NONE, "OFF" },
                  { vn::protocol::uart::CountMode::COUNTMODE_SYNCINCOUNT, "SyncIn Counter" },
                  { vn::protocol::uart::CountMode::COUNTMODE_SYNCINTIME, "SyncIn Time" },
                  { vn::protocol::uart::CountMode::COUNTMODE_SYNCOUTCOUNTER, "SyncOut Counter" },
                  { vn::protocol::uart::CountMode::COUNTMODE_GPSPPS, "Gps Pps Time" } }
            };
            if (ImGui::BeginCombo(fmt::format("Serial Count Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.serialCount).c_str()))
            {
                for (const auto& communicationProtocolControlSerialCount : communicationProtocolControlSerialCounts)
                {
                    const bool isSelected = (communicationProtocolControlRegister.serialCount == communicationProtocolControlSerialCount.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSerialCount.first).c_str(), isSelected))
                    {
                        communicationProtocolControlRegister.serialCount = communicationProtocolControlSerialCount.first;
                        LOG_DEBUG("{}: communicationProtocolControlRegister.serialCount changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.serialCount));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(communicationProtocolControlSerialCount.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SerialCount field provides a means of appending a time or counter to the end of all asynchronous "
                                     "communication messages transmitted on the serial interface. The values for each of these counters come "
                                     "directly from the Synchronization Status Register in the System subsystem.\n\n"
                                     "With the SerialCount field set to OFF a typical serial asynchronous message would appear as the following:\n"
                                     "$VNYPR,+010.071,+000.278,-002.026*60\n\n"
                                     "With the SerialCount field set to one of the non-zero values the same asynchronous message would appear "
                                     "instead as:\n"
                                     "$VNYPR,+010.071,+000.278,-002.026,T1162704*2F\n\n"
                                     "When the SerialCount field is enabled the counter will always be appended to the end of the message just "
                                     "prior to the checksum. The counter will be preceded by the T character to distinguish it from the status field.");

            static constexpr std::array<std::pair<vn::protocol::uart::StatusMode, const char*>, 3> communicationProtocolControlSerialStatuses = {
                { { vn::protocol::uart::StatusMode::STATUSMODE_OFF, "OFF" },
                  { vn::protocol::uart::StatusMode::STATUSMODE_VPESTATUS, "VPE Status" },
                  { vn::protocol::uart::StatusMode::STATUSMODE_INSSTATUS, "INS Status" } }
            };
            if (ImGui::BeginCombo(fmt::format("Serial Status##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.serialStatus).c_str()))
            {
                for (const auto& communicationProtocolControlSerialStatus : communicationProtocolControlSerialStatuses)
                {
                    const bool isSelected = (communicationProtocolControlRegister.serialStatus == communicationProtocolControlSerialStatus.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSerialStatus.first).c_str(), isSelected))
                    {
                        communicationProtocolControlRegister.serialStatus = communicationProtocolControlSerialStatus.first;
                        LOG_DEBUG("{}: communicationProtocolControlRegister.serialStatus changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.serialStatus));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(communicationProtocolControlSerialStatus.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The SerialStatus field provides a means of tracking real-time status information pertain to the overall state "
                                     "of the sensor measurements and onboard filtering algorithm. As with the SerialCount, a typical serial "
                                     "asynchronous message would appear as the following:\n"
                                     "$VNYPR,+010.071,+000.278,-002.026*60\n\n"
                                     "With the SerialStatus field set to one of the non-zero values, the same asynchronous message would appear "
                                     "instead as:\n"
                                     "$VNYPR,+010.071,+000.278,-002.026,S0000*1F\n\n"
                                     "When the SerialStatus field is enabled the status will always be appended to the end of the message just "
                                     "prior to the checksum. If both the SerialCount and SerialStatus are enabled then the SerialStatus will be "
                                     "displayed first. The counter will be preceded by the S character to distinguish it from the counter field. The "
                                     "status consists of 4 hexadecimal characters.");

            if (sensorModel == VectorNavModel::VN100_VN110)
            {
                if (ImGui::BeginCombo(fmt::format("SPI Count##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.spiCount).c_str()))
                {
                    for (const auto& communicationProtocolControlSpiCount : communicationProtocolControlSerialCounts)
                    {
                        const bool isSelected = (communicationProtocolControlRegister.spiCount == communicationProtocolControlSpiCount.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSpiCount.first).c_str(), isSelected))
                        {
                            communicationProtocolControlRegister.spiCount = communicationProtocolControlSpiCount.first;
                            LOG_DEBUG("{}: communicationProtocolControlRegister.spiCount changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.spiCount));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(communicationProtocolControlSpiCount.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("The SPICount field provides a means of appending a time or counter to the end of all SPI packets. The "
                                         "values for each of these counters come directly from the Synchronization Status Register.");
            }

            if (sensorModel == VectorNavModel::VN100_VN110)
            {
                if (ImGui::BeginCombo(fmt::format("SPI Status##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.spiStatus).c_str()))
                {
                    for (const auto& communicationProtocolControlSpiStatus : communicationProtocolControlSerialStatuses)
                    {
                        const bool isSelected = (communicationProtocolControlRegister.spiStatus == communicationProtocolControlSpiStatus.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSpiStatus.first).c_str(), isSelected))
                        {
                            communicationProtocolControlRegister.spiStatus = communicationProtocolControlSpiStatus.first;
                            LOG_DEBUG("{}: communicationProtocolControlRegister.spiStatus changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.spiStatus));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(communicationProtocolControlSpiStatus.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("The AsyncStatus field provides a means of tracking real-time status information pertaining to the overall "
                                         "state of the sensor measurements and onboard filtering algorithm. This information is very useful in "
                                         "situations where action must be taken when certain crucial events happen such as the detection of gyro "
                                         "saturation or magnetic interference.");
            }

            static constexpr std::array<std::pair<vn::protocol::uart::ChecksumMode, const char*>, 2> communicationProtocolControlSerialChecksums = {
                { { vn::protocol::uart::ChecksumMode::CHECKSUMMODE_CHECKSUM, "8-Bit Checksum" },
                  { vn::protocol::uart::ChecksumMode::CHECKSUMMODE_CRC, "16-Bit CRC" } }
            };
            if (ImGui::BeginCombo(fmt::format("Serial Checksum##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.serialChecksum).c_str()))
            {
                for (const auto& communicationProtocolControlSerialChecksum : communicationProtocolControlSerialChecksums)
                {
                    const bool isSelected = (communicationProtocolControlRegister.serialChecksum == communicationProtocolControlSerialChecksum.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSerialChecksum.first).c_str(), isSelected))
                    {
                        communicationProtocolControlRegister.serialChecksum = communicationProtocolControlSerialChecksum.first;
                        LOG_DEBUG("{}: communicationProtocolControlRegister.serialChecksum changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.serialChecksum));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(communicationProtocolControlSerialChecksum.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("This field controls the type of checksum used for the serial communications. Normally the VN-310E uses an "
                                     "8-bit checksum identical to the type used for normal GPS NMEA packets. This form of checksum however "
                                     "offers only a limited means of error checking. As an alternative a full 16-bit CRC (CRC16-CCITT with "
                                     "polynomial = 0x07) is also offered. The 2-byte CRC value is printed using 4 hexadecimal digits.");

            if (sensorModel == VectorNavModel::VN100_VN110)
            {
                static constexpr std::array<std::pair<vn::protocol::uart::ChecksumMode, const char*>, 3> communicationProtocolControlSpiChecksums = {
                    { { vn::protocol::uart::ChecksumMode::CHECKSUMMODE_OFF, "OFF" },
                      { vn::protocol::uart::ChecksumMode::CHECKSUMMODE_CHECKSUM, "8-Bit Checksum" },
                      { vn::protocol::uart::ChecksumMode::CHECKSUMMODE_CRC, "16-Bit CRC" } }
                };
                if (ImGui::BeginCombo(fmt::format("SPI Checksum##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.spiChecksum).c_str()))
                {
                    for (const auto& communicationProtocolControlSpiChecksum : communicationProtocolControlSpiChecksums)
                    {
                        const bool isSelected = (communicationProtocolControlRegister.spiChecksum == communicationProtocolControlSpiChecksum.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlSpiChecksum.first).c_str(), isSelected))
                        {
                            communicationProtocolControlRegister.spiChecksum = communicationProtocolControlSpiChecksum.first;
                            LOG_DEBUG("{}: communicationProtocolControlRegister.spiChecksum changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.spiChecksum));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(communicationProtocolControlSpiChecksum.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("This field controls the type of checksum used for the SPI communications. The checksum is appended to "
                                         "the end of the binary data packet. The 16-bit CRC is identical to the one described above for the "
                                         "SerialChecksum.");
            }

            static constexpr std::array<std::pair<vn::protocol::uart::ErrorMode, const char*>, 3> communicationProtocolControlErrorModes = {
                { { vn::protocol::uart::ErrorMode::ERRORMODE_IGNORE, "Ignore Error" },
                  { vn::protocol::uart::ErrorMode::ERRORMODE_SEND, "Send Error" },
                  { vn::protocol::uart::ErrorMode::ERRORMODE_SENDANDOFF, "Send Error and set ADOR register to OFF" } }
            };
            if (ImGui::BeginCombo(fmt::format("Error Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(communicationProtocolControlRegister.errorMode).c_str()))
            {
                for (const auto& communicationProtocolControlErrorMode : communicationProtocolControlErrorModes)
                {
                    const bool isSelected = (communicationProtocolControlRegister.errorMode == communicationProtocolControlErrorMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(communicationProtocolControlErrorMode.first).c_str(), isSelected))
                    {
                        communicationProtocolControlRegister.errorMode = communicationProtocolControlErrorMode.first;
                        LOG_DEBUG("{}: communicationProtocolControlRegister.errorMode changed to {}", nameId(), vn::protocol::uart::str(communicationProtocolControlRegister.errorMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the communicationProtocolControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(communicationProtocolControlErrorMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("This field controls the type of action taken by the VectorNav when an error event occurs. If the send error "
                                     "mode is enabled then a message similar to the one shown below will be sent on the serial bus when an error "
                                     "event occurs.\n\n"
                                     "$VNERR,03*72\n\n"
                                     "Regardless of the state of the ErrorMode, the number of error events is always recorded and is made available "
                                     "in the SysErrors field of the Communication Protocol Status Register in the System subsystem.");

            if (ImGui::TreeNode(fmt::format("Example Async Messages##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("The following table shows example asynchronous messages with the\nAsyncCount and the AsyncStatus values appended to the end.");

                if (ImGui::BeginTable("Example Async Messages Table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX, ImVec2(0.0F, 0.0F)))
                {
                    ImGui::TableSetupColumn("Example Type");
                    ImGui::TableSetupColumn("Message");
                    ImGui::TableHeadersRow();

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Async Message with\nAsyncCount Enabled");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("$VNYPR,+010.071,+000.278,-002.026,T1162704*2F");

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Async Message with\nAsyncStatus Enabled");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("$VNYPR,+010.071,+000.278,-002.026,S0000*1F");

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Async Message with\nAsyncCount and\nAsyncStatus Enabled");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("$VNYPR,+010.071,+000.278,-002.026,T1162704,S0000*50");

                    ImGui::EndTable();
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        for (size_t b = 0; b < binaryOutputRegister.size(); b++)
        {
            if (ImGui::TreeNode(fmt::format("Binary Output {}##{}", b + 1, size_t(id)).c_str()))
            {
                static constexpr std::array<std::pair<vn::protocol::uart::AsyncMode, const char*>, 4> asyncModes = {
                    { { vn::protocol::uart::AsyncMode::ASYNCMODE_NONE, " User message is not automatically sent out either serial port" },
                      { vn::protocol::uart::AsyncMode::ASYNCMODE_PORT1, "Message is sent out serial port 1 at a fixed rate" },
                      { vn::protocol::uart::AsyncMode::ASYNCMODE_PORT2, "Message is sent out serial port 2 at a fixed rate" },
                      { vn::protocol::uart::AsyncMode::ASYNCMODE_BOTH, "Message is sent out both serial ports at a fixed rate" } }
                };
                if (ImGui::BeginCombo(fmt::format("Async Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(binaryOutputRegister.at(b).asyncMode).c_str()))
                {
                    for (const auto& asyncMode : asyncModes)
                    {
                        const bool isSelected = (binaryOutputRegister.at(b).asyncMode == asyncMode.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(asyncMode.first).c_str(), isSelected))
                        {
                            binaryOutputRegister.at(b).asyncMode = asyncMode.first;
                            LOG_DEBUG("{}: binaryOutputRegister.at(b).asyncMode changed to {}", nameId(), vn::protocol::uart::str(binaryOutputRegister.at(b).asyncMode));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    switch (b)
                                    {
                                    case 0:
                                        vs.writeBinaryOutput1(binaryOutputRegister.at(0));
                                        break;
                                    case 1:
                                        vs.writeBinaryOutput2(binaryOutputRegister.at(1));
                                        break;
                                    case 2:
                                        vs.writeBinaryOutput3(binaryOutputRegister.at(2));
                                        break;
                                    default:
                                        break;
                                    }
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the binaryOutputRegister {}: {}", nameId(), b + 1, e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(asyncMode.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Selects whether the output message should be sent "
                                         "out on the serial port(s) at a fixed rate.");

                const char* frequencyText = binaryOutputSelectedFrequency.at(b) < dividerFrequency.first.size()
                                                ? dividerFrequency.second.at(binaryOutputSelectedFrequency.at(b)).c_str()
                                                : "Unknown";
                if (ImGui::SliderInt(fmt::format("Frequency##{} {}", size_t(id), b).c_str(),
                                     reinterpret_cast<int*>(&binaryOutputSelectedFrequency.at(b)),
                                     0, static_cast<int>(dividerFrequency.second.size()) - 1,
                                     frequencyText))
                {
                    binaryOutputRegister.at(b).rateDivisor = dividerFrequency.first.at(binaryOutputSelectedFrequency.at(b));
                    LOG_DEBUG("{}: Frequency of Binary Group {} changed to {}", nameId(), b + 1, dividerFrequency.second.at(binaryOutputSelectedFrequency.at(b)));
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            switch (b)
                            {
                            case 0:
                                vs.writeBinaryOutput1(binaryOutputRegister.at(b));
                                break;
                            case 1:
                                vs.writeBinaryOutput2(binaryOutputRegister.at(b));
                                break;
                            case 2:
                                vs.writeBinaryOutput3(binaryOutputRegister.at(b));
                                break;
                            default:
                                break;
                            }
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the binaryOutputRegister {}: {}", nameId(), b + 1, e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::BeginTable(fmt::format("##VectorNavSensorConfig ({})", size_t(id)).c_str(), 7,
                                      ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupColumn("Common", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("IMU", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("GNSS1", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Attitude", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("INS", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("GNSS2", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();

                    auto CheckboxFlags = [&, this](int index, const char* label, int* flags, int flags_value, bool enabled, void (*toggleFields)(vn::sensors::BinaryOutputRegister & bor, uint32_t & binaryField)) {
                        ImGui::TableSetColumnIndex(index);

                        if (!enabled)
                        {
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5F);
                        }

                        if (ImGui::CheckboxFlags(label, flags, flags_value))
                        {
                            LOG_DEBUG("{}: Field '{}' of Binary Group {} is now {}", nameId(), std::string(label).substr(0, std::string(label).find('#')), b + 1, (*flags & flags_value) ? "checked" : "unchecked");
                            if (toggleFields)
                            {
                                toggleFields(binaryOutputRegister.at(b), *reinterpret_cast<uint32_t*>(flags));
                            }
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    if (b == 0)
                                    {
                                        vs.writeBinaryOutput1(binaryOutputRegister.at(b));
                                        auto connectedLinks = nm::FindConnectedLinksToOutputPin(outputPins.at(b + 2).id);
                                        for (auto& connectedLink : connectedLinks)
                                        {
                                            nm::RefreshLink(connectedLink->id);
                                        }
                                    }
                                    else if (b == 1)
                                    {
                                        vs.writeBinaryOutput2(binaryOutputRegister.at(b));
                                        auto connectedLinks = nm::FindConnectedLinksToOutputPin(outputPins.at(b + 2).id);
                                        for (auto& connectedLink : connectedLinks)
                                        {
                                            nm::RefreshLink(connectedLink->id);
                                        }
                                    }
                                    else if (b == 2)
                                    {
                                        vs.writeBinaryOutput3(binaryOutputRegister.at(b));
                                        auto connectedLinks = nm::FindConnectedLinksToOutputPin(outputPins.at(b + 2).id);
                                        for (auto& connectedLink : connectedLinks)
                                        {
                                            nm::RefreshLink(connectedLink->id);
                                        }
                                    }
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the binaryOutputRegister {}: {}", nameId(), b + 1, e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }

                        if (!enabled)
                        {
                            ImGui::PopItemFlag();
                            ImGui::PopStyleVar();
                        }
                    };

                    for (size_t i = 0; i < 16; i++)
                    {
                        if (i < std::max({ binaryGroupCommon.size(), binaryGroupTime.size(), binaryGroupIMU.size(),
                                           binaryGroupGNSS.size(), binaryGroupAttitude.size(), binaryGroupINS.size() }))
                        {
                            ImGui::TableNextRow();
                        }
                        if (i < binaryGroupCommon.size())
                        {
                            const auto& binaryGroupItem = binaryGroupCommon.at(i);
                            CheckboxFlags(0, fmt::format("{}##Common {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).commonField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).commonField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupTime.size())
                        {
                            const auto& binaryGroupItem = binaryGroupTime.at(i);
                            CheckboxFlags(1, fmt::format("{}##Time {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).timeField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).timeField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupIMU.size())
                        {
                            const auto& binaryGroupItem = binaryGroupIMU.at(i);
                            CheckboxFlags(2, fmt::format("{}##IMU {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).imuField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).imuField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupGNSS.size())
                        {
                            const auto& binaryGroupItem = binaryGroupGNSS.at(i);
                            CheckboxFlags(3, fmt::format("{}##GNSS1 {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).gpsField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).gpsField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupAttitude.size())
                        {
                            const auto& binaryGroupItem = binaryGroupAttitude.at(i);
                            CheckboxFlags(4, fmt::format("{}##Attitude {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).attitudeField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).attitudeField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupINS.size())
                        {
                            const auto& binaryGroupItem = binaryGroupINS.at(i);
                            CheckboxFlags(5, fmt::format("{}##INS {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).insField),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).insField)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                        if (i < binaryGroupGNSS.size())
                        {
                            const auto& binaryGroupItem = binaryGroupGNSS.at(i);
                            CheckboxFlags(6, fmt::format("{}##GNSS2 {} {}", binaryGroupItem.name, size_t(id), b).c_str(),
                                          reinterpret_cast<int*>(&binaryOutputRegister.at(b).gps2Field),
                                          binaryGroupItem.flagsValue,
                                          binaryGroupItem.isEnabled(sensorModel, binaryOutputRegister.at(b), static_cast<uint32_t>(binaryOutputRegister.at(b).gps2Field)),
                                          binaryGroupItem.toggleFields);
                            if (ImGui::IsItemHovered() && binaryGroupItem.tooltip != nullptr)
                            {
                                ImGui::BeginTooltip();
                                binaryGroupItem.tooltip();
                                ImGui::EndTooltip();
                            }
                        }
                    }

                    ImGui::EndTable();
                }

                ImGui::TreePop();
            }
        }

        // TODO: Add Gui Config for NMEA output - User manual VN-310 - 8.2.14 (p 103)
    }

    // ###########################################################################################################
    //                                               IMU SUBSYSTEM
    // ###########################################################################################################

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader(fmt::format("IMU Subsystem##{}", size_t(id)).c_str()))
    {
        if (ImGui::TreeNode(fmt::format("Reference Frame Rotation##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Allows the measurements of the VN-310E to be rotated into a different reference frame.");
            ImGui::SameLine();
            gui::widgets::HelpMarker("This register contains a transformation matrix that allows for the transformation of measured acceleration, "
                                     "magnetic, and angular rates from the body frame of the VN-310E to any other arbitrary frame of reference. "
                                     "The use of this register allows for the sensor to be placed in any arbitrary orientation with respect to the "
                                     "user's desired body coordinate frame. This register can also be used to correct for any orientation errors due "
                                     "to mounting the VN-310E on the user's vehicle or platform.\n\n"
                                     "(X Y Z)_U = C * (X Y Z)_B\n\n"
                                     "The variables (X Y Z)_B are a measured parameter such as acceleration in the body reference frame with "
                                     "respect to the VectorNav. The variables (X Y Z)_U are a measured parameter such as acceleration in the user's "
                                     "frame of reference. The reference frame rotation register Thus needs to be loaded with the transformation "
                                     "matrix that will transform measurements from the body reference frame of the VectorNav to the desired user "
                                     "frame of reference.");
            if (sensorModel == VectorNavModel::VN310)
            {
                ImGui::SameLine();
                gui::widgets::HelpMarker("The reference frame rotation is performed on all vector measurements prior to entering the INS "
                                         "filter. As such, changing this register while the attitude filter is running will lead to unexpected "
                                         "behavior in the INS output. To prevent this, the register is cached on startup and changes will "
                                         "not take effect during runtime. After setting the reference frame rotation register to its new value, "
                                         "send a write settings command and then reset the VN-310E. This will allow the INS filter to "
                                         "startup with the newly set reference frame rotation.",
                                         "(!)");
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The matrix C in the Reference Frame Rotation Register must be an orthonormal, right-handed"
                                     " matrix. The sensor will output an error if the tolerance is not within 1e-5. The sensor will also"
                                     " report an error if any of the parameters are greater than 1 or less than -1.",
                                     "(!)");

            ImGui::TextUnformatted("Rotation Angles [deg]");
            ImGui::SameLine();
            gui::widgets::HelpMarker("The rotation happens in ZYX Order");
            ImGui::Indent();

            // TODO: Angles to define this
            Eigen::Matrix3d dcm;
            dcm << referenceFrameRotationMatrix.e00, referenceFrameRotationMatrix.e01, referenceFrameRotationMatrix.e02,
                referenceFrameRotationMatrix.e10, referenceFrameRotationMatrix.e11, referenceFrameRotationMatrix.e12,
                referenceFrameRotationMatrix.e20, referenceFrameRotationMatrix.e21, referenceFrameRotationMatrix.e22;
            Eigen::Quaterniond q_bp = Eigen::Quaterniond(dcm);

            Eigen::Vector3d eulerRot = trafo::rad2deg3(trafo::quat2eulerZYX(q_bp.conjugate()));
            std::array<float, 3> imuRot = { static_cast<float>(eulerRot.x()), static_cast<float>(eulerRot.y()), static_cast<float>(eulerRot.z()) };
            if (ImGui::InputFloat3(fmt::format("##rotationAngles{}", size_t(id)).c_str(), imuRot.data()))
            {
                // (-180:180] x (-90:90] x (-180:180]
                if (imuRot.at(0) < -179.9999F)
                {
                    imuRot.at(0) = -179.9999F;
                }
                if (imuRot.at(0) > 180)
                {
                    imuRot.at(0) = 180;
                }
                if (imuRot.at(1) < -89.9999F)
                {
                    imuRot.at(1) = -89.9999F;
                }
                if (imuRot.at(1) > 90)
                {
                    imuRot.at(1) = 90;
                }
                if (imuRot.at(2) < -179.9999F)
                {
                    imuRot.at(2) = -179.9999F;
                }
                if (imuRot.at(2) > 180)
                {
                    imuRot.at(2) = 180;
                }
                auto dcmf = trafo::quat_bp(trafo::deg2rad(imuRot.at(0)), trafo::deg2rad(imuRot.at(1)), trafo::deg2rad(imuRot.at(2))).toRotationMatrix().cast<float>();
                referenceFrameRotationMatrix = vn::math::mat3f(dcmf(0, 0), dcmf(0, 1), dcmf(0, 2),
                                                               dcmf(1, 0), dcmf(1, 1), dcmf(1, 2),
                                                               dcmf(2, 0), dcmf(2, 1), dcmf(2, 2));
                LOG_DEBUG("{}: referenceFrameRotationMatrix changed to {}", nameId(), referenceFrameRotationMatrix);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceFrameRotation(referenceFrameRotationMatrix);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceFrameRotationMatrix: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            ImGui::Unindent();
            ImGui::TextUnformatted("Rotation Matrix C");
            ImGui::Indent();

            std::array<float, 3> row = { referenceFrameRotationMatrix.e00, referenceFrameRotationMatrix.e01, referenceFrameRotationMatrix.e02 };
            if (ImGui::InputFloat3(fmt::format("##referenceFrameRotationMatrix row 0 {}", size_t(id)).c_str(), row.data(), "%.2f"))
            {
                referenceFrameRotationMatrix.e00 = row.at(0);
                referenceFrameRotationMatrix.e01 = row.at(1);
                referenceFrameRotationMatrix.e02 = row.at(2);
                LOG_DEBUG("{}: referenceFrameRotationMatrix changed to {}", nameId(), referenceFrameRotationMatrix);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceFrameRotation(referenceFrameRotationMatrix);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceFrameRotationMatrix: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            row = { referenceFrameRotationMatrix.e10, referenceFrameRotationMatrix.e11, referenceFrameRotationMatrix.e12 };
            if (ImGui::InputFloat3(fmt::format("##referenceFrameRotationMatrix row 1 {}", size_t(id)).c_str(), row.data(), "%.2f"))
            {
                referenceFrameRotationMatrix.e10 = row.at(0);
                referenceFrameRotationMatrix.e11 = row.at(1);
                referenceFrameRotationMatrix.e12 = row.at(2);
                LOG_DEBUG("{}: referenceFrameRotationMatrix changed to {}", nameId(), referenceFrameRotationMatrix);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceFrameRotation(referenceFrameRotationMatrix);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceFrameRotationMatrix: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            row = { referenceFrameRotationMatrix.e20, referenceFrameRotationMatrix.e21, referenceFrameRotationMatrix.e22 };
            if (ImGui::InputFloat3(fmt::format("##referenceFrameRotationMatrix row 2 {}", size_t(id)).c_str(), row.data(), "%.2f"))
            {
                referenceFrameRotationMatrix.e20 = row.at(0);
                referenceFrameRotationMatrix.e21 = row.at(1);
                referenceFrameRotationMatrix.e22 = row.at(2);
                LOG_DEBUG("{}: referenceFrameRotationMatrix changed to {}", nameId(), referenceFrameRotationMatrix);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceFrameRotation(referenceFrameRotationMatrix);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceFrameRotationMatrix: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            ImGui::Unindent();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode(fmt::format("IMU Filtering Configuration##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("This register allows the user to configure the FIR filtering what is applied to the IMU measurements. The\n"
                                   "filter is a uniformly-weighted moving window (boxcar) filter of configurable size. The filtering does not affect\n"
                                   "the values used by the internal filter, but only the output values.");

            int magWindowSize = imuFilteringConfigurationRegister.magWindowSize;
            if (ImGui::InputInt(fmt::format("Mag Window Size##{}", size_t(id)).c_str(), &magWindowSize))
            {
                if (magWindowSize < 0)
                {
                    magWindowSize = 0;
                }
                else if (magWindowSize > std::numeric_limits<uint16_t>::max())
                {
                    magWindowSize = std::numeric_limits<uint16_t>::max();
                }
                imuFilteringConfigurationRegister.magWindowSize = static_cast<uint16_t>(magWindowSize);
                LOG_DEBUG("{}: imuFilteringConfigurationRegister.magWindowSize changed to {}", nameId(), imuFilteringConfigurationRegister.magWindowSize);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The WindowSize parameters for each sensor define the number of samples at the IMU rate (default 800Hz) "
                                     "which will be averaged for each output measurement.");

            int accelWindowSize = imuFilteringConfigurationRegister.accelWindowSize;
            if (ImGui::InputInt(fmt::format("Accel Window Size##{}", size_t(id)).c_str(), &accelWindowSize))
            {
                if (accelWindowSize < 0)
                {
                    accelWindowSize = 0;
                }
                else if (accelWindowSize > std::numeric_limits<uint16_t>::max())
                {
                    accelWindowSize = std::numeric_limits<uint16_t>::max();
                }
                imuFilteringConfigurationRegister.accelWindowSize = static_cast<uint16_t>(accelWindowSize);
                LOG_DEBUG("{}: imuFilteringConfigurationRegister.accelWindowSize changed to {}", nameId(), imuFilteringConfigurationRegister.accelWindowSize);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The WindowSize parameters for each sensor define the number of samples at the IMU rate (default 800Hz) "
                                     "which will be averaged for each output measurement.");

            int gyroWindowSize = imuFilteringConfigurationRegister.gyroWindowSize;
            if (ImGui::InputInt(fmt::format("Gyro Window Size##{}", size_t(id)).c_str(), &gyroWindowSize))
            {
                if (gyroWindowSize < 0)
                {
                    gyroWindowSize = 0;
                }
                else if (gyroWindowSize > std::numeric_limits<uint16_t>::max())
                {
                    gyroWindowSize = std::numeric_limits<uint16_t>::max();
                }
                imuFilteringConfigurationRegister.gyroWindowSize = static_cast<uint16_t>(gyroWindowSize);
                LOG_DEBUG("{}: imuFilteringConfigurationRegister.gyroWindowSize changed to {}", nameId(), imuFilteringConfigurationRegister.gyroWindowSize);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The WindowSize parameters for each sensor define the number of samples at the IMU rate (default 800Hz) "
                                     "which will be averaged for each output measurement.");

            int tempWindowSize = imuFilteringConfigurationRegister.tempWindowSize;
            if (ImGui::InputInt(fmt::format("Temp Window Size##{}", size_t(id)).c_str(), &tempWindowSize))
            {
                if (tempWindowSize < 0)
                {
                    tempWindowSize = 0;
                }
                else if (tempWindowSize > std::numeric_limits<uint16_t>::max())
                {
                    tempWindowSize = std::numeric_limits<uint16_t>::max();
                }
                imuFilteringConfigurationRegister.tempWindowSize = static_cast<uint16_t>(tempWindowSize);
                LOG_DEBUG("{}: imuFilteringConfigurationRegister.tempWindowSize changed to {}", nameId(), imuFilteringConfigurationRegister.tempWindowSize);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The WindowSize parameters for each sensor define the number of samples at the IMU rate (default 800Hz) "
                                     "which will be averaged for each output measurement.");

            int presWindowSize = imuFilteringConfigurationRegister.presWindowSize;
            if (ImGui::InputInt(fmt::format("Pres Window Size##{}", size_t(id)).c_str(), &presWindowSize))
            {
                if (presWindowSize < 0)
                {
                    presWindowSize = 0;
                }
                else if (presWindowSize > std::numeric_limits<uint16_t>::max())
                {
                    presWindowSize = std::numeric_limits<uint16_t>::max();
                }
                imuFilteringConfigurationRegister.presWindowSize = static_cast<uint16_t>(presWindowSize);
                LOG_DEBUG("{}: imuFilteringConfigurationRegister.presWindowSize changed to {}", nameId(), imuFilteringConfigurationRegister.presWindowSize);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The WindowSize parameters for each sensor define the number of samples at the IMU rate (default 800Hz) "
                                     "which will be averaged for each output measurement.");

            static constexpr std::array<std::pair<vn::protocol::uart::FilterMode, const char*>, 4> imuFilteringConfigurationFilterModes = {
                { { vn::protocol::uart::FilterMode::FILTERMODE_NOFILTERING, "No Filtering" },
                  { vn::protocol::uart::FilterMode::FILTERMODE_ONLYRAW, "Filtering performed only on raw uncompensated IMU measurements." },
                  { vn::protocol::uart::FilterMode::FILTERMODE_ONLYCOMPENSATED, "Filtering performed only on compensated IMU measurements." },
                  { vn::protocol::uart::FilterMode::FILTERMODE_BOTH, "Filtering performed on both uncompensated and compensated IMU measurements." } }
            };
            if (ImGui::BeginCombo(fmt::format("Mag Filter Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(imuFilteringConfigurationRegister.magFilterMode).c_str()))
            {
                for (const auto& imuFilteringConfigurationFilterMode : imuFilteringConfigurationFilterModes)
                {
                    const bool isSelected = (imuFilteringConfigurationRegister.magFilterMode == imuFilteringConfigurationFilterMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(imuFilteringConfigurationFilterMode.first).c_str(), isSelected))
                    {
                        imuFilteringConfigurationRegister.magFilterMode = imuFilteringConfigurationFilterMode.first;
                        LOG_DEBUG("{}: imuFilteringConfigurationRegister.magFilterMode changed to {}", nameId(), vn::protocol::uart::str(imuFilteringConfigurationRegister.magFilterMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(imuFilteringConfigurationFilterMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The FilterMode parameters for each sensor select which output quantities the filtering should be applied to. "
                                     "Filtering can be applied to either the uncompensated IMU measurements, compensated (HSI and biases "
                                     "compensated by onboard filters, if applicable), or both.");

            if (ImGui::BeginCombo(fmt::format("Accel Filter Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(imuFilteringConfigurationRegister.accelFilterMode).c_str()))
            {
                for (const auto& imuFilteringConfigurationFilterMode : imuFilteringConfigurationFilterModes)
                {
                    const bool isSelected = (imuFilteringConfigurationRegister.accelFilterMode == imuFilteringConfigurationFilterMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(imuFilteringConfigurationFilterMode.first).c_str(), isSelected))
                    {
                        imuFilteringConfigurationRegister.accelFilterMode = imuFilteringConfigurationFilterMode.first;
                        LOG_DEBUG("{}: imuFilteringConfigurationRegister.accelFilterMode changed to {}", nameId(), vn::protocol::uart::str(imuFilteringConfigurationRegister.accelFilterMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(imuFilteringConfigurationFilterMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The FilterMode parameters for each sensor select which output quantities the filtering should be applied to. "
                                     "Filtering can be applied to either the uncompensated IMU measurements, compensated (HSI and biases "
                                     "compensated by onboard filters, if applicable), or both.");

            if (ImGui::BeginCombo(fmt::format("Gyro Filter Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(imuFilteringConfigurationRegister.gyroFilterMode).c_str()))
            {
                for (const auto& imuFilteringConfigurationFilterMode : imuFilteringConfigurationFilterModes)
                {
                    const bool isSelected = (imuFilteringConfigurationRegister.gyroFilterMode == imuFilteringConfigurationFilterMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(imuFilteringConfigurationFilterMode.first).c_str(), isSelected))
                    {
                        imuFilteringConfigurationRegister.gyroFilterMode = imuFilteringConfigurationFilterMode.first;
                        LOG_DEBUG("{}: imuFilteringConfigurationRegister.gyroFilterMode changed to {}", nameId(), vn::protocol::uart::str(imuFilteringConfigurationRegister.gyroFilterMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(imuFilteringConfigurationFilterMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The FilterMode parameters for each sensor select which output quantities the filtering should be applied to. "
                                     "Filtering can be applied to either the uncompensated IMU measurements, compensated (HSI and biases "
                                     "compensated by onboard filters, if applicable), or both.");

            if (ImGui::BeginCombo(fmt::format("Temp Filter Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(imuFilteringConfigurationRegister.tempFilterMode).c_str()))
            {
                for (const auto& imuFilteringConfigurationFilterMode : imuFilteringConfigurationFilterModes)
                {
                    const bool isSelected = (imuFilteringConfigurationRegister.tempFilterMode == imuFilteringConfigurationFilterMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(imuFilteringConfigurationFilterMode.first).c_str(), isSelected))
                    {
                        imuFilteringConfigurationRegister.tempFilterMode = imuFilteringConfigurationFilterMode.first;
                        LOG_DEBUG("{}: imuFilteringConfigurationRegister.tempFilterMode changed to {}", nameId(), vn::protocol::uart::str(imuFilteringConfigurationRegister.tempFilterMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(imuFilteringConfigurationFilterMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The FilterMode parameters for each sensor select which output quantities the filtering should be applied to. "
                                     "Filtering can be applied to either the uncompensated IMU measurements, compensated (HSI and biases "
                                     "compensated by onboard filters, if applicable), or both.");

            if (ImGui::BeginCombo(fmt::format("Pres Filter Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(imuFilteringConfigurationRegister.presFilterMode).c_str()))
            {
                for (const auto& imuFilteringConfigurationFilterMode : imuFilteringConfigurationFilterModes)
                {
                    const bool isSelected = (imuFilteringConfigurationRegister.presFilterMode == imuFilteringConfigurationFilterMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(imuFilteringConfigurationFilterMode.first).c_str(), isSelected))
                    {
                        imuFilteringConfigurationRegister.presFilterMode = imuFilteringConfigurationFilterMode.first;
                        LOG_DEBUG("{}: imuFilteringConfigurationRegister.presFilterMode changed to {}", nameId(), vn::protocol::uart::str(imuFilteringConfigurationRegister.presFilterMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the imuFilteringConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(imuFilteringConfigurationFilterMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The FilterMode parameters for each sensor select which output quantities the filtering should be applied to. "
                                     "Filtering can be applied to either the uncompensated IMU measurements, compensated (HSI and biases "
                                     "compensated by onboard filters, if applicable), or both.");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(fmt::format("Delta Theta and Delta Velocity Configuration##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("The Delta Theta and Delta Velocity Configuration register allows configuration of the onboard coning and\n"
                                   "sculling used to generate integrated motion values from the angular rate and acceleration IMU quantities.\n"
                                   "The fully-coupled coning and sculling integrals are computed at the IMU sample rate (nominal 400 Hz).");

            static constexpr std::array<std::pair<vn::protocol::uart::IntegrationFrame, const char*>, 2> deltaThetaAndDeltaVelocityConfigurationIntegrationFrames = {
                { { vn::protocol::uart::IntegrationFrame::INTEGRATIONFRAME_BODY, "Body frame" },
                  { vn::protocol::uart::IntegrationFrame::INTEGRATIONFRAME_NED, "NED frame" } }
            };
            if (ImGui::BeginCombo(fmt::format("Integration Frame##{}", size_t(id)).c_str(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame).c_str()))
            {
                for (const auto& deltaThetaAndDeltaVelocityConfigurationIntegrationFrame : deltaThetaAndDeltaVelocityConfigurationIntegrationFrames)
                {
                    const bool isSelected = (deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame == deltaThetaAndDeltaVelocityConfigurationIntegrationFrame.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationIntegrationFrame.first).c_str(), isSelected))
                    {
                        deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame = deltaThetaAndDeltaVelocityConfigurationIntegrationFrame.first;
                        LOG_DEBUG("{}: deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame changed to {}", nameId(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.integrationFrame));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeDeltaThetaAndDeltaVelocityConfiguration(deltaThetaAndDeltaVelocityConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the deltaThetaAndDeltaVelocityConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(deltaThetaAndDeltaVelocityConfigurationIntegrationFrame.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The IntegrationFrame register setting selects the reference frame used for coning and sculling. Note that "
                                     "using any frame other than the body frame will rely on the onboard Kalman filter's attitude estimate. The "
                                     "factory default state is to integrate in the sensor body frame.");

            static constexpr std::array<std::pair<vn::protocol::uart::CompensationMode, const char*>, 2> deltaThetaAndDeltaVelocityConfigurationGyroCompensationModes = {
                { { vn::protocol::uart::CompensationMode::COMPENSATIONMODE_NONE, "None" },
                  { vn::protocol::uart::CompensationMode::COMPENSATIONMODE_BIAS, "Bias" } }
            };
            if (ImGui::BeginCombo(fmt::format("Gyro Compensation##{}", size_t(id)).c_str(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation).c_str()))
            {
                for (const auto& deltaThetaAndDeltaVelocityConfigurationGyroCompensationMode : deltaThetaAndDeltaVelocityConfigurationGyroCompensationModes)
                {
                    const bool isSelected = (deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation == deltaThetaAndDeltaVelocityConfigurationGyroCompensationMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationGyroCompensationMode.first).c_str(), isSelected))
                    {
                        deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation = deltaThetaAndDeltaVelocityConfigurationGyroCompensationMode.first;
                        LOG_DEBUG("{}: deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation changed to {}", nameId(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.gyroCompensation));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeDeltaThetaAndDeltaVelocityConfiguration(deltaThetaAndDeltaVelocityConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the deltaThetaAndDeltaVelocityConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(deltaThetaAndDeltaVelocityConfigurationGyroCompensationMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The GyroCompensation register setting selects the compensation to be applied to the angular rate "
                                     "measurements before integration. If bias compensation is selected, the onboard Kalman filter’s real-time "
                                     "estimate of the gyro biases will be used to compensate the IMU measurements before integration. The "
                                     "factory default state is to integrate the uncompensated angular rates from the IMU.");

            static constexpr std::array<std::pair<vn::protocol::uart::AccCompensationMode, const char*>, 2> deltaThetaAndDeltaVelocityConfigurationAccelCompensationModes = {
                { { vn::protocol::uart::AccCompensationMode::ACCCOMPENSATIONMODE_NONE, "None" },
                  { vn::protocol::uart::AccCompensationMode::ACCCOMPENSATIONMODE_BIAS, "Bias" } }
            };
            if (ImGui::BeginCombo(fmt::format("Accel Compensation##{}", size_t(id)).c_str(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation).c_str()))
            {
                for (const auto& deltaThetaAndDeltaVelocityConfigurationAccelCompensationMode : deltaThetaAndDeltaVelocityConfigurationAccelCompensationModes)
                {
                    const bool isSelected = (deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation == deltaThetaAndDeltaVelocityConfigurationAccelCompensationMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationAccelCompensationMode.first).c_str(), isSelected))
                    {
                        deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation = deltaThetaAndDeltaVelocityConfigurationAccelCompensationMode.first;
                        LOG_DEBUG("{}: deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation changed to {}", nameId(), vn::protocol::uart::str(deltaThetaAndDeltaVelocityConfigurationRegister.accelCompensation));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeDeltaThetaAndDeltaVelocityConfiguration(deltaThetaAndDeltaVelocityConfigurationRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the deltaThetaAndDeltaVelocityConfigurationRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(deltaThetaAndDeltaVelocityConfigurationAccelCompensationMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The AccelCompensation register setting selects the compensation to be applied to the acceleration "
                                     "measurements before integration. If bias compensation is selected, the onboard Kalman filter’s real-time "
                                     "estimate of the accel biases will be used to compensate the IMU measurements before integration. The "
                                     "factory default state is to integrate the uncompensated acceleration from the IMU.");

            ImGui::TreePop();
        }
    }

    // ###########################################################################################################
    //                                              GNSS SUBSYSTEM
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN310)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::CollapsingHeader(fmt::format("GNSS Subsystem##{}", size_t(id)).c_str()))
        {
            if (ImGui::TreeNode(fmt::format("GNSS Configuration##{}", size_t(id)).c_str()))
            {
                static constexpr std::array<std::pair<vn::protocol::uart::GpsMode, const char*>, 3> gpsConfigurationModes = {
                    { { vn::protocol::uart::GpsMode::GPSMODE_ONBOARDGPS, "Use onboard GNSS" },
                      { vn::protocol::uart::GpsMode::GPSMODE_EXTERNALGPS, "Use external GNSS" },
                      { vn::protocol::uart::GpsMode::GPSMODE_EXTERNALVN200GPS, "Use external VectorNav sensor as the GNSS" } }
                };
                if (ImGui::BeginCombo(fmt::format("Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(gpsConfigurationRegister.mode).c_str()))
                {
                    for (const auto& gpsConfigurationMode : gpsConfigurationModes)
                    {
                        const bool isSelected = (gpsConfigurationRegister.mode == gpsConfigurationMode.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(gpsConfigurationMode.first).c_str(), isSelected))
                        {
                            gpsConfigurationRegister.mode = gpsConfigurationMode.first;
                            LOG_DEBUG("{}: gpsConfigurationRegister.mode changed to {}", nameId(), vn::protocol::uart::str(gpsConfigurationRegister.mode));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeGpsConfiguration(gpsConfigurationRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the gpsConfigurationRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(gpsConfigurationMode.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                static constexpr std::array<std::pair<vn::protocol::uart::PpsSource, const char*>, 4> gpsConfigurationPpsSources = {
                    { { vn::protocol::uart::PpsSource::PPSSOURCE_GPSPPSRISING, "GNSS PPS signal is present on the GNSS_PPS pin (pin 24) and should trigger on a rising edge." },
                      { vn::protocol::uart::PpsSource::PPSSOURCE_GPSPPSFALLING, "GNSS PPS signal is present on the GNSS_PPS pin (pin 24) and should trigger on a falling edge" },
                      { vn::protocol::uart::PpsSource::PPSSOURCE_SYNCINRISING, "GNSS PPS signal is present on the SyncIn pin (pin 22) and should trigger on a rising edge" },
                      { vn::protocol::uart::PpsSource::PPSSOURCE_SYNCINFALLING, "GNSS PPS signal is present on the SyncIn pin (pin 22) and should trigger on a falling edge" } }
                };
                if (ImGui::BeginCombo(fmt::format("PPS Source##{}", size_t(id)).c_str(), vn::protocol::uart::str(gpsConfigurationRegister.ppsSource).c_str()))
                {
                    for (const auto& gpsConfigurationPpsSource : gpsConfigurationPpsSources)
                    {
                        const bool isSelected = (gpsConfigurationRegister.ppsSource == gpsConfigurationPpsSource.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(gpsConfigurationPpsSource.first).c_str(), isSelected))
                        {
                            gpsConfigurationRegister.ppsSource = gpsConfigurationPpsSource.first;
                            LOG_DEBUG("{}: gpsConfigurationRegister.ppsSource changed to {}", nameId(), vn::protocol::uart::str(gpsConfigurationRegister.ppsSource));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeGpsConfiguration(gpsConfigurationRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the gpsConfigurationRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(gpsConfigurationPpsSource.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                static constexpr std::array<vn::protocol::uart::GpsRate, 1> gpsConfigurationRates = {
                    { /* vn::protocol::uart::GpsRate::GPSRATE_1HZ, */
                      vn::protocol::uart::GpsRate::GPSRATE_5HZ }
                };
                if (ImGui::BeginCombo(fmt::format("Rate##{}", size_t(id)).c_str(), vn::protocol::uart::str(gpsConfigurationRegister.rate).c_str()))
                {
                    for (const auto& gpsConfigurationRate : gpsConfigurationRates)
                    {
                        const bool isSelected = (gpsConfigurationRegister.rate == gpsConfigurationRate);
                        if (ImGui::Selectable(vn::protocol::uart::str(gpsConfigurationRate).c_str(), isSelected))
                        {
                            gpsConfigurationRegister.rate = gpsConfigurationRate;
                            LOG_DEBUG("{}: gpsConfigurationRegister.rate changed to {}", nameId(), vn::protocol::uart::str(gpsConfigurationRegister.rate));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeGpsConfiguration(gpsConfigurationRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the gpsConfigurationRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("GNSS navigation rate. Value must be set to 5.");

                static constexpr std::array<std::pair<vn::protocol::uart::AntPower, const char*>, 3> gpsConfigurationAntPowers = {
                    { { vn::protocol::uart::AntPower::ANTPOWER_OFFRESV, "Disable antenna power supply." },
                      { vn::protocol::uart::AntPower::ANTPOWER_INTERNAL, "Use internal antenna power supply (3V, 50mA combined)." },
                      { vn::protocol::uart::AntPower::ANTPOWER_EXTERNAL, "Use external antenna power supply (VANT pin, up to 5V and 100mA combined)" } }
                };
                if (ImGui::BeginCombo(fmt::format("Ant Power##{}", size_t(id)).c_str(), vn::protocol::uart::str(gpsConfigurationRegister.antPow).c_str()))
                {
                    for (const auto& gpsConfigurationAntPower : gpsConfigurationAntPowers)
                    {
                        const bool isSelected = (gpsConfigurationRegister.antPow == gpsConfigurationAntPower.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(gpsConfigurationAntPower.first).c_str(), isSelected))
                        {
                            gpsConfigurationRegister.antPow = gpsConfigurationAntPower.first;
                            LOG_DEBUG("{}: gpsConfigurationRegister.antPow changed to {}", nameId(), vn::protocol::uart::str(gpsConfigurationRegister.antPow));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeGpsConfiguration(gpsConfigurationRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the gpsConfigurationRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(gpsConfigurationAntPower.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("GNSS navigation rate. Value must be set to 5.");

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("GNSS Antenna A Offset##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("The position of the GNSS antenna A relative to the sensor in the vehicle coordinate frame\n"
                                       "also referred to as the GNSS antenna lever arm.");

                if (ImGui::InputFloat3(fmt::format("##gpsAntennaOffset {}", size_t(id)).c_str(), gpsAntennaOffset.c, "%.6f"))
                {
                    LOG_DEBUG("{}: gpsAntennaOffset changed to {}", nameId(), gpsAntennaOffset);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeGpsAntennaOffset(gpsAntennaOffset);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the gpsAntennaOffset: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("GNSS Compass Baseline##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Configures the position offset and measurement uncertainty of the second GNSS\n"
                                       "antenna relative to the first GNSS antenna in the vehicle reference frame.");

                if (ImGui::InputFloat3(fmt::format("Position [m]##{}", size_t(id)).c_str(), gpsCompassBaselineRegister.position.c, "%.6f"))
                {
                    LOG_DEBUG("{}: gpsCompassBaselineRegister.position changed to {}", nameId(), gpsCompassBaselineRegister.position);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeGpsCompassBaseline(gpsCompassBaselineRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the gpsCompassBaselineRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("HEADING ACCURACY\n\n"
                                         "The accuracy of the estimated heading is dependent upon the accuracy of the measured baseline "
                                         "between the two GNSS antennas. The factory default baseline is {1.0m, 0.0m, 0.0m}. If any other "
                                         "baseline is used, it is extremely important that the user acurately measures this baseline to ensure "
                                         "accurate heading estimates.\n"
                                         "The heading accuracy is linearly proportional to the measurement accuracy of the position of "
                                         "GNSS antenna B with respect to GNSS antenna A, and inversely proportional to the baseline "
                                         "length.\n\n"
                                         "Heading Error [deg] ~= 0.57 * (Baseline Error [cm]) / (Baseline Length [m])\n\n"
                                         "On a 1 meter baseline, a 1 cm measurement error equates to heading error of 0.6 degrees.",
                                         "(!)");

                if (ImGui::InputFloat3(fmt::format("Uncertainty [m]##{}", size_t(id)).c_str(), gpsCompassBaselineRegister.uncertainty.c, "%.3f"))
                {
                    LOG_DEBUG("{}: gpsCompassBaselineRegister.uncertainty changed to {}", nameId(), gpsCompassBaselineRegister.uncertainty);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeGpsCompassBaseline(gpsCompassBaselineRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the gpsCompassBaselineRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("MEASUREMENT UNCERTAINTY\n\n"
                                         "For the VN-310E to function properly it is very important that the user supplies a reasonable "
                                         "measurement uncertainty that is greater than the actual uncertainty in the baseline measurement. "
                                         "The VN-310E uses the uncertainty supplied by the user to validate measurements that it receives "
                                         "from the GNSS receivers. If the user inputs an uncertainty that is lower than the actual error in "
                                         "the baseline measurement between the two antennas, the VN-310E will no longer be able to derive "
                                         "heading estimates from the GNSS.\n\n"
                                         "It is recommended that you set the uncertainty equal to twice what you expect the worst case "
                                         "error to be in your baseline measurements. In many applications it is easier to measure more "
                                         "accurately in one direction than another. It is recommended that you set each of the X, Y, & Z "
                                         "uncertainties seperately to reflect this, as opposed to using a single large value.",
                                         "(!)");

                ImGui::TreePop();
            }
        }
    }

    // ###########################################################################################################
    //                                            ATTITUDE SUBSYSTEM
    // ###########################################################################################################

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader(fmt::format("Attitude Subsystem##{}", size_t(id)).c_str()))
    {
        if (ImGui::TreeNode(fmt::format("VPE Basic Control##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Provides control over various features relating to the onboard attitude filtering algorithm.");

            static constexpr std::array<vn::protocol::uart::VpeEnable, 2> vpeBasicControlEnables = {
                { vn::protocol::uart::VpeEnable::VPEENABLE_DISABLE,
                  vn::protocol::uart::VpeEnable::VPEENABLE_ENABLE }
            };
            if (ImGui::BeginCombo(fmt::format("Enable##{}", size_t(id)).c_str(), vn::protocol::uart::str(vpeBasicControlRegister.enable).c_str()))
            {
                for (const auto& vpeBasicControlEnable : vpeBasicControlEnables)
                {
                    const bool isSelected = (vpeBasicControlRegister.enable == vpeBasicControlEnable);
                    if (ImGui::Selectable(vn::protocol::uart::str(vpeBasicControlEnable).c_str(), isSelected))
                    {
                        vpeBasicControlRegister.enable = vpeBasicControlEnable;
                        LOG_DEBUG("{}: vpeBasicControlRegister.enable changed to {}", nameId(), vn::protocol::uart::str(vpeBasicControlRegister.enable));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeVpeBasicControl(vpeBasicControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the vpeBasicControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static constexpr std::array<vn::protocol::uart::HeadingMode, 3> vpeBasicControlHeadingModes = {
                { vn::protocol::uart::HeadingMode::HEADINGMODE_ABSOLUTE,
                  vn::protocol::uart::HeadingMode::HEADINGMODE_RELATIVE,
                  vn::protocol::uart::HeadingMode::HEADINGMODE_INDOOR }
            };
            if (ImGui::BeginCombo(fmt::format("Heading Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(vpeBasicControlRegister.headingMode).c_str()))
            {
                for (const auto& vpeBasicControlHeadingMode : vpeBasicControlHeadingModes)
                {
                    const bool isSelected = (vpeBasicControlRegister.headingMode == vpeBasicControlHeadingMode);
                    if (ImGui::Selectable(vn::protocol::uart::str(vpeBasicControlHeadingMode).c_str(), isSelected))
                    {
                        vpeBasicControlRegister.headingMode = vpeBasicControlHeadingMode;
                        LOG_DEBUG("{}: vpeBasicControlRegister.headingMode changed to {}", nameId(), vn::protocol::uart::str(vpeBasicControlRegister.headingMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeVpeBasicControl(vpeBasicControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the vpeBasicControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static constexpr std::array<vn::protocol::uart::VpeMode, 2> vpeBasicControlModes = {
                { vn::protocol::uart::VpeMode::VPEMODE_OFF,
                  vn::protocol::uart::VpeMode::VPEMODE_MODE1 }
            };
            if (ImGui::BeginCombo(fmt::format("Filtering Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(vpeBasicControlRegister.filteringMode).c_str()))
            {
                for (const auto& vpeBasicControlMode : vpeBasicControlModes)
                {
                    const bool isSelected = (vpeBasicControlRegister.filteringMode == vpeBasicControlMode);
                    if (ImGui::Selectable(vn::protocol::uart::str(vpeBasicControlMode).c_str(), isSelected))
                    {
                        vpeBasicControlRegister.filteringMode = vpeBasicControlMode;
                        LOG_DEBUG("{}: vpeBasicControlRegister.filteringMode changed to {}", nameId(), vn::protocol::uart::str(vpeBasicControlRegister.filteringMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeVpeBasicControl(vpeBasicControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the vpeBasicControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo(fmt::format("Tuning Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(vpeBasicControlRegister.tuningMode).c_str()))
            {
                for (const auto& vpeBasicControlMode : vpeBasicControlModes)
                {
                    const bool isSelected = (vpeBasicControlRegister.tuningMode == vpeBasicControlMode);
                    if (ImGui::Selectable(vn::protocol::uart::str(vpeBasicControlMode).c_str(), isSelected))
                    {
                        vpeBasicControlRegister.tuningMode = vpeBasicControlMode;
                        LOG_DEBUG("{}: vpeBasicControlRegister.tuningMode changed to {}", nameId(), vn::protocol::uart::str(vpeBasicControlRegister.tuningMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeVpeBasicControl(vpeBasicControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the vpeBasicControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::TreePop();
        }

        if (sensorModel == VectorNavModel::VN100_VN110)
        {
            if (ImGui::TreeNode(fmt::format("VPE Magnetometer Basic Tuning##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Provides basic control of the adaptive filtering and tuning for the magnetometer.");

                if (ImGui::DragFloat3(fmt::format("Base Tuning Level##{}", size_t(id)).c_str(), vpeMagnetometerBasicTuningRegister.baseTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeMagnetometerBasicTuningRegister.baseTuning changed to {}", nameId(), vpeMagnetometerBasicTuningRegister.baseTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeMagnetometerBasicTuning(vpeMagnetometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeMagnetometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("This sets the level of confidence placed in the magnetometer when no disturbances are present. "
                                         "A larger number provides better heading accuracy, but with more sensitivity to magnetic interference.");

                if (ImGui::DragFloat3(fmt::format("Adaptive Tuning Level##{}", size_t(id)).c_str(), vpeMagnetometerBasicTuningRegister.adaptiveTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeMagnetometerBasicTuningRegister.adaptiveTuning changed to {}", nameId(), vpeMagnetometerBasicTuningRegister.adaptiveTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeMagnetometerBasicTuning(vpeMagnetometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeMagnetometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::DragFloat3(fmt::format("Adaptive Filtering Level##{}", size_t(id)).c_str(), vpeMagnetometerBasicTuningRegister.adaptiveFiltering.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeMagnetometerBasicTuningRegister.adaptiveFiltering changed to {}", nameId(), vpeMagnetometerBasicTuningRegister.adaptiveFiltering);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeMagnetometerBasicTuning(vpeMagnetometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeMagnetometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("VPE Accelerometer Basic Tuning##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Provides basic control of the adaptive filtering and tuning for the accelerometer.");

                if (ImGui::DragFloat3(fmt::format("Base Tuning Level##{}", size_t(id)).c_str(), vpeAccelerometerBasicTuningRegister.baseTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeAccelerometerBasicTuningRegister.baseTuning changed to {}", nameId(), vpeAccelerometerBasicTuningRegister.baseTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeAccelerometerBasicTuning(vpeAccelerometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeAccelerometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("This sets the level of confidence placed in the accelerometer when no disturbances are present. "
                                         "A larger number provides better pitch/roll heading accuracy, but with more sensitivity to acceleration interference.");

                if (ImGui::DragFloat3(fmt::format("Adaptive Tuning Level##{}", size_t(id)).c_str(), vpeAccelerometerBasicTuningRegister.adaptiveTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeAccelerometerBasicTuningRegister.adaptiveTuning changed to {}", nameId(), vpeAccelerometerBasicTuningRegister.adaptiveTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeAccelerometerBasicTuning(vpeAccelerometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeAccelerometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::DragFloat3(fmt::format("Adaptive Filtering Level##{}", size_t(id)).c_str(), vpeAccelerometerBasicTuningRegister.adaptiveFiltering.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeAccelerometerBasicTuningRegister.adaptiveFiltering changed to {}", nameId(), vpeAccelerometerBasicTuningRegister.adaptiveFiltering);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeAccelerometerBasicTuning(vpeAccelerometerBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeAccelerometerBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("VPE Gyro Basic Tuning##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Provides basic control of the adaptive filtering and tuning for the gyroscope.");

                if (ImGui::DragFloat3(fmt::format("Base Tuning Level##{}", size_t(id)).c_str(), vpeGyroBasicTuningRegister.baseTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeGyroBasicTuningRegister.baseTuning changed to {}", nameId(), vpeGyroBasicTuningRegister.baseTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeGyroBasicTuning(vpeGyroBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeGyroBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("This sets the level of confidence placed in the gyro axes.");

                if (ImGui::DragFloat3(fmt::format("Adaptive Tuning Level##{}", size_t(id)).c_str(), vpeGyroBasicTuningRegister.adaptiveTuning.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeGyroBasicTuningRegister.adaptiveTuning changed to {}", nameId(), vpeGyroBasicTuningRegister.adaptiveTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeGyroBasicTuning(vpeGyroBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeGyroBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::DragFloat3(fmt::format("Variance - Angular Walk##{}", size_t(id)).c_str(), vpeGyroBasicTuningRegister.angularWalkVariance.c, 0.1F, 0.0F, 10.0F, "%.1f"))
                {
                    LOG_DEBUG("{}: vpeGyroBasicTuningRegister.angularWalkVariance changed to {}", nameId(), vpeGyroBasicTuningRegister.angularWalkVariance);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVpeGyroBasicTuning(vpeGyroBasicTuningRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the vpeGyroBasicTuningRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("Filter Startup Gyro Bias##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("The filter gyro bias estimate used at startup [rad/s].");

                if (ImGui::InputFloat3(fmt::format("## FilterStartupGyroBias {}", size_t(id)).c_str(), filterStartupGyroBias.c, "%.1f"))
                {
                    LOG_DEBUG("{}: filterStartupGyroBias changed to {}", nameId(), filterStartupGyroBias);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeFilterStartupGyroBias(filterStartupGyroBias);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the filterStartupGyroBias: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    // ###########################################################################################################
    //                                               INS SUBSYSTEM
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN310)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::CollapsingHeader(fmt::format("INS Subsystem##{}", size_t(id)).c_str()))
        {
            if (ImGui::TreeNode(fmt::format("INS Basic Configuration##{}", size_t(id)).c_str()))
            {
                static constexpr std::array<std::pair<vn::protocol::uart::Scenario, const char*>, 3> insBasicConfigurationRegisterVn300Scenarios = {
                    { { vn::protocol::uart::Scenario::SCENARIO_INSWITHPRESSURE, "General purpose INS with barometric pressure sensor" },
                      { vn::protocol::uart::Scenario::SCENARIO_INSWITHOUTPRESSURE, "General purpose INS without barometric pressure sensor" },
                      { vn::protocol::uart::Scenario::SCENARIO_GPSMOVINGBASELINEDYNAMIC, "GNSS moving baseline for dynamic applications" } }
                };
                if (ImGui::BeginCombo(fmt::format("Scenario##{}", size_t(id)).c_str(), vn::protocol::uart::str(insBasicConfigurationRegisterVn300.scenario).c_str()))
                {
                    for (const auto& insBasicConfigurationRegisterVn300Scenario : insBasicConfigurationRegisterVn300Scenarios)
                    {
                        const bool isSelected = (insBasicConfigurationRegisterVn300.scenario == insBasicConfigurationRegisterVn300Scenario.first);
                        if (ImGui::Selectable(vn::protocol::uart::str(insBasicConfigurationRegisterVn300Scenario.first).c_str(), isSelected))
                        {
                            insBasicConfigurationRegisterVn300.scenario = insBasicConfigurationRegisterVn300Scenario.first;
                            LOG_DEBUG("{}: insBasicConfigurationRegisterVn300.scenario changed to {}", nameId(), vn::protocol::uart::str(insBasicConfigurationRegisterVn300.scenario));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeInsBasicConfigurationVn300(insBasicConfigurationRegisterVn300);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the insBasicConfigurationRegisterVn300: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(insBasicConfigurationRegisterVn300Scenario.second);
                            ImGui::EndTooltip();
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Checkbox(fmt::format("Ahrs Aiding##{}", size_t(id)).c_str(), &insBasicConfigurationRegisterVn300.ahrsAiding))
                {
                    LOG_DEBUG("{}: insBasicConfigurationRegisterVn300.ahrsAiding changed to {}", nameId(), insBasicConfigurationRegisterVn300.ahrsAiding);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeInsBasicConfigurationVn300(insBasicConfigurationRegisterVn300);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the insBasicConfigurationRegisterVn300: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Enables AHRS attitude aiding. AHRS aiding provides "
                                         "the ability to switch to using the magnetometer to "
                                         "stabilize heading during times when the device is "
                                         "stationary and the GNSS compass is not available. "
                                         "AHRS aiding also helps to eliminate large updates in "
                                         "the attitude solution during times when heading is "
                                         "weakly observable, such as at startup.");

                if (ImGui::Checkbox(fmt::format("Estimate Baseline##{}", size_t(id)).c_str(), &insBasicConfigurationRegisterVn300.estBaseline))
                {
                    LOG_DEBUG("{}: insBasicConfigurationRegisterVn300.estBaseline changed to {}", nameId(), insBasicConfigurationRegisterVn300.estBaseline);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeInsBasicConfigurationVn300(insBasicConfigurationRegisterVn300);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the insBasicConfigurationRegisterVn300: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Enables GNSS compass baseline estimation by INS");

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(fmt::format("Startup Filter Bias Estimate##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Sets the initial estimate for the filter bias states");

                if (ImGui::InputFloat3(fmt::format("Gyro Bias [rad/s]##{}", size_t(id)).c_str(), startupFilterBiasEstimateRegister.gyroBias.c, "%.1f"))
                {
                    LOG_DEBUG("{}: startupFilterBiasEstimateRegister.gyroBias changed to {}", nameId(), startupFilterBiasEstimateRegister.gyroBias);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeStartupFilterBiasEstimate(startupFilterBiasEstimateRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the startupFilterBiasEstimateRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::InputFloat3(fmt::format("Accel Bias [m/s^2]##{}", size_t(id)).c_str(), startupFilterBiasEstimateRegister.accelBias.c, "%.1f"))
                {
                    LOG_DEBUG("{}: startupFilterBiasEstimateRegister.accelBias changed to {}", nameId(), startupFilterBiasEstimateRegister.accelBias);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeStartupFilterBiasEstimate(startupFilterBiasEstimateRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the startupFilterBiasEstimateRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                if (ImGui::InputFloat(fmt::format("Pressure Bias [m]##{}", size_t(id)).c_str(), &startupFilterBiasEstimateRegister.pressureBias))
                {
                    LOG_DEBUG("{}: startupFilterBiasEstimateRegister.pressureBias changed to {}", nameId(), startupFilterBiasEstimateRegister.pressureBias);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeStartupFilterBiasEstimate(startupFilterBiasEstimateRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the startupFilterBiasEstimateRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    // ###########################################################################################################
    //                                    HARD/SOFT IRON ESTIMATOR SUBSYSTEM
    // ###########################################################################################################

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader(fmt::format("Hard/Soft Iron Estimator Subsystem##{}", size_t(id)).c_str()))
    {
        if (ImGui::TreeNode(fmt::format("Magnetometer Calibration Control##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Controls the magnetometer real-time calibration algorithm.");
            if (sensorModel == VectorNavModel::VN310)
            {
                ImGui::SameLine();
                gui::widgets::HelpMarker("On the PRODUCT the magnetometer is only used to provide a coarse heading estimate at startup "
                                         "and is completely tuned out of the INS filter during normal operation. A Hard/Soft Iron calibration "
                                         "may be performed to try and improve the startup magnetic heading, but is not required for nominal "
                                         "operaiton.",
                                         "(!)");
            }

            static constexpr std::array<std::pair<vn::protocol::uart::HsiMode, const char*>, 3> magnetometerCalibrationControlHsiModes = {
                { { vn::protocol::uart::HsiMode::HSIMODE_OFF, "Real-time hard/soft iron calibration algorithm is turned off" },
                  { vn::protocol::uart::HsiMode::HSIMODE_RUN, "Runs the real-time hard/soft iron calibration. The algorithm will continue using its existing solution.\n"
                                                              "The algorithm can be started and stopped at any time by switching between the HSI_OFF and HSI_RUN state." },
                  { vn::protocol::uart::HsiMode::HSIMODE_RESET, "Resets the real-time hard/soft iron solution" } }
            };
            if (ImGui::BeginCombo(fmt::format("HSI Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(magnetometerCalibrationControlRegister.hsiMode).c_str()))
            {
                for (const auto& magnetometerCalibrationControlHsiMode : magnetometerCalibrationControlHsiModes)
                {
                    const bool isSelected = (magnetometerCalibrationControlRegister.hsiMode == magnetometerCalibrationControlHsiMode.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(magnetometerCalibrationControlHsiMode.first).c_str(), isSelected))
                    {
                        magnetometerCalibrationControlRegister.hsiMode = magnetometerCalibrationControlHsiMode.first;
                        LOG_DEBUG("{}: magnetometerCalibrationControlRegister.hsiMode changed to {}", nameId(), vn::protocol::uart::str(magnetometerCalibrationControlRegister.hsiMode));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeMagnetometerCalibrationControl(magnetometerCalibrationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the magnetometerCalibrationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(magnetometerCalibrationControlHsiMode.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("Controls the mode of operation for the onboard real-time magnetometer hard/soft iron compensation algorithm.");

            static constexpr std::array<std::pair<vn::protocol::uart::HsiOutput, const char*>, 3> magnetometerCalibrationControlHsiOutputs = {
                { { vn::protocol::uart::HsiOutput::HSIOUTPUT_NOONBOARD, "Onboard HSI is not applied to the magnetic measurements" },
                  { vn::protocol::uart::HsiOutput::HSIOUTPUT_USEONBOARD, "Onboard HSI is applied to the magnetic measurements" } }
            };
            if (ImGui::BeginCombo(fmt::format("HSI Output##{}", size_t(id)).c_str(), vn::protocol::uart::str(magnetometerCalibrationControlRegister.hsiOutput).c_str()))
            {
                for (const auto& magnetometerCalibrationControlHsiOutput : magnetometerCalibrationControlHsiOutputs)
                {
                    const bool isSelected = (magnetometerCalibrationControlRegister.hsiOutput == magnetometerCalibrationControlHsiOutput.first);
                    if (ImGui::Selectable(vn::protocol::uart::str(magnetometerCalibrationControlHsiOutput.first).c_str(), isSelected))
                    {
                        magnetometerCalibrationControlRegister.hsiOutput = magnetometerCalibrationControlHsiOutput.first;
                        LOG_DEBUG("{}: magnetometerCalibrationControlRegister.hsiOutput changed to {}", nameId(), vn::protocol::uart::str(magnetometerCalibrationControlRegister.hsiOutput));
                        flow::ApplyChanges();
                        if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                        {
                            try
                            {
                                vs.writeMagnetometerCalibrationControl(magnetometerCalibrationControlRegister);
                            }
                            catch (const std::exception& e)
                            {
                                LOG_ERROR("{}: Could not configure the magnetometerCalibrationControlRegister: {}", nameId(), e.what());
                                deinitializeNode();
                            }
                        }
                        else
                        {
                            deinitializeNode();
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(magnetometerCalibrationControlHsiOutput.second);
                        ImGui::EndTooltip();
                    }

                    if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("Controls the type of measurements that are provided as "
                                     "outputs from the magnetometer sensor and also "
                                     "subsequently used in the attitude filter.");

            int convergeRate = magnetometerCalibrationControlRegister.convergeRate;
            if (ImGui::SliderInt(fmt::format("Converge Rate##{}", size_t(id)).c_str(), &convergeRate, 0, 5))
            {
                magnetometerCalibrationControlRegister.convergeRate = static_cast<uint8_t>(convergeRate);
                LOG_DEBUG("{}: magnetometerCalibrationControlRegister.convergeRate changed to {}", nameId(), magnetometerCalibrationControlRegister.convergeRate);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeMagnetometerCalibrationControl(magnetometerCalibrationControlRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the magnetometerCalibrationControlRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("Controls how quickly the hard/soft iron solution is allowed "
                                     "to converge onto a new solution. The slower the "
                                     "convergence the more accurate the estimate of the "
                                     "hard/soft iron solution. A quicker convergence will provide "
                                     "a less accurate estimate of the hard/soft iron parameters, "
                                     "but for applications where the hard/soft iron changes "
                                     "rapidly may provide a more accurate attitude estimate.\n\n"
                                     "Range: 1 to 5\n"
                                     "1 = Solution converges slowly over approximately 60-90 seconds.\n"
                                     "5 = Solution converges rapidly over approximately 15-20 seconds.");

            ImGui::TreePop();
        }
    }

    // ###########################################################################################################
    //                                      WORLD MAGNETIC & GRAVITY MODULE
    // ###########################################################################################################

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader(fmt::format("World Magnetic & Gravity Module##{}", size_t(id)).c_str()))
    {
        if (ImGui::TreeNode(fmt::format("Magnetic and Gravity Reference Vectors##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Magnetic and gravity reference vectors");
            ImGui::SameLine();
            gui::widgets::HelpMarker("This register contains the reference vectors for the magnetic and gravitational fields as used by the "
                                     "onboard filter. The values map to either the user-set values or the results of calculations of the onboard "
                                     "reference models (see the Reference Vector Configuration Register in the IMU subsystem). When the "
                                     "reference values come from the onboard model(s), those values are read-only. When the reference models "
                                     "are disabled, the values reflect the user reference vectors and will be writable. For example, if the onboard "
                                     "World Magnetic Model is enabled and the onboard Gravitational Model is disabled, only the gravity "
                                     "reference values will be modified on a register write. Note that the user reference vectors will not be "
                                     "overwritten by the onboard models, but will retain their previous values for when the onboard models are "
                                     "disabled.");

            if (ImGui::InputFloat3(fmt::format("Magnetic Reference [Gauss]##{}", size_t(id)).c_str(), magneticAndGravityReferenceVectorsRegister.magRef.c, "%.3f"))
            {
                LOG_DEBUG("{}: magneticAndGravityReferenceVectorsRegister.magRef changed to {}", nameId(), magneticAndGravityReferenceVectorsRegister.magRef);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeMagneticAndGravityReferenceVectors(magneticAndGravityReferenceVectorsRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the magneticAndGravityReferenceVectorsRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            if (ImGui::InputFloat3(fmt::format("Gravity Reference [m/s^2]##{}", size_t(id)).c_str(), magneticAndGravityReferenceVectorsRegister.accRef.c, "%.6f"))
            {
                LOG_DEBUG("{}: magneticAndGravityReferenceVectorsRegister.accRef changed to {}", nameId(), magneticAndGravityReferenceVectorsRegister.accRef);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeMagneticAndGravityReferenceVectors(magneticAndGravityReferenceVectorsRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the magneticAndGravityReferenceVectorsRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(fmt::format("Reference Vector Configuration##{}", size_t(id)).c_str()))
        {
            ImGui::TextUnformatted("Control register for both the onboard world magnetic and gravity model corrections");
            ImGui::SameLine();
            gui::widgets::HelpMarker("This register allows configuration of the onboard spherical harmonic models used to calculate the local "
                                     "magnetic and gravitational reference values. Having accurate magnetic reference values improves the "
                                     "accuracy of heading when using the magnetometer and accounts for magnetic declination. Having accurate "
                                     "gravitational reference values improves accuracy by allowing the INS filter to more accurately estimate the "
                                     "accelerometer biases. The VectorNav currently includes the EGM96 gravitational model and the WMM2010 "
                                     "magnetic model. The models are upgradable to allow updating to future models when available.\n\n"
                                     "The magnetic and gravity models can be individually enabled or disabled using the UseMagModel and "
                                     "UseGravityModel parameters, respectively. When disabled, the corresponding values set by the user in the "
                                     "Reference Vector Register in the IMU subsystem will be used instead of values calculated by the onboard "
                                     "model.\n\n"
                                     "The VectorNav starts up with the user configured reference vector values. Shortly after startup (and if the "
                                     "models are enabled), the location and time set in this register will be used to update the reference vectors. "
                                     "When a 3D GNSS fix is available, the location and time reported by the GNSS will be used to update the model. "
                                     "If GNSS is lost, the reference vectors will hold their last valid values. The model values will be recalculated "
                                     "whenever the current position has changed by the RecaclThreshold or the date has changed by more than "
                                     "approximately 8 hours, whichever comes first.");

            if (ImGui::Checkbox(fmt::format("Use Mag Model##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.useMagModel))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.useMagModel changed to {}", nameId(), referenceVectorConfigurationRegister.useMagModel);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            if (ImGui::Checkbox(fmt::format("Use Gravity Model##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.useGravityModel))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.useGravityModel changed to {}", nameId(), referenceVectorConfigurationRegister.useGravityModel);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }

            auto recalcThreshold = static_cast<int>(referenceVectorConfigurationRegister.recalcThreshold);
            if (ImGui::InputInt(fmt::format("Recalc Threshold [m]##{}", size_t(id)).c_str(), &recalcThreshold))
            {
                referenceVectorConfigurationRegister.recalcThreshold = static_cast<uint32_t>(recalcThreshold);
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.recalcThreshold changed to {}", nameId(), referenceVectorConfigurationRegister.recalcThreshold);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("Maximum distance traveled before magnetic and gravity models are recalculated for the new position.");

            if (ImGui::InputFloat(fmt::format("Year##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.year))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.year changed to {}", nameId(), referenceVectorConfigurationRegister.year);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The reference date expressed as a decimal year. Used for both the magnetic and gravity models.");

            if (ImGui::InputDouble(fmt::format("Latitude [deg]##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.position[0]))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.position.latitude changed to {}", nameId(), referenceVectorConfigurationRegister.position[0]);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The reference latitude position in degrees.");

            if (ImGui::InputDouble(fmt::format("Longitude [deg]##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.position[1]))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.position.longitude changed to {}", nameId(), referenceVectorConfigurationRegister.position[1]);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The reference longitude position in degrees.");

            if (ImGui::InputDouble(fmt::format("Altitude [m]##{}", size_t(id)).c_str(), &referenceVectorConfigurationRegister.position[2]))
            {
                LOG_DEBUG("{}: referenceVectorConfigurationRegister.position.altitude changed to {}", nameId(), referenceVectorConfigurationRegister.position[2]);
                flow::ApplyChanges();
                if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                {
                    try
                    {
                        vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("{}: Could not configure the referenceVectorConfigurationRegister: {}", nameId(), e.what());
                        deinitializeNode();
                    }
                }
                else
                {
                    deinitializeNode();
                }
            }
            ImGui::SameLine();
            gui::widgets::HelpMarker("The reference altitude above the reference ellipsoid in meters.");

            ImGui::TreePop();
        }
    }

    // ###########################################################################################################
    //                                              VELOCITY AIDING
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN100_VN110)
    {
        ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::CollapsingHeader(fmt::format("Velocity Aiding##{}", size_t(id)).c_str()))
        {
            if (ImGui::TreeNode(fmt::format("Velocity Compensation Control##{}", size_t(id)).c_str()))
            {
                ImGui::TextUnformatted("Provides control over the velocity compensation feature for the attitude filter.");

                static constexpr std::array<vn::protocol::uart::VelocityCompensationMode, 2> velocityCompensationControlModes = {
                    { vn::protocol::uart::VelocityCompensationMode::VELOCITYCOMPENSATIONMODE_DISABLED,
                      vn::protocol::uart::VelocityCompensationMode::VELOCITYCOMPENSATIONMODE_BODYMEASUREMENT }
                };
                if (ImGui::BeginCombo(fmt::format("Mode##{}", size_t(id)).c_str(), vn::protocol::uart::str(velocityCompensationControlRegister.mode).c_str()))
                {
                    for (const auto& velocityCompensationControlMode : velocityCompensationControlModes)
                    {
                        const bool isSelected = (velocityCompensationControlRegister.mode == velocityCompensationControlMode);
                        if (ImGui::Selectable(vn::protocol::uart::str(velocityCompensationControlMode).c_str(), isSelected))
                        {
                            velocityCompensationControlRegister.mode = velocityCompensationControlMode;
                            LOG_DEBUG("{}: velocityCompensationControlRegister.mode changed to {}", nameId(), vn::protocol::uart::str(velocityCompensationControlRegister.mode));
                            flow::ApplyChanges();
                            if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                            {
                                try
                                {
                                    vs.writeVelocityCompensationControl(velocityCompensationControlRegister);
                                }
                                catch (const std::exception& e)
                                {
                                    LOG_ERROR("{}: Could not configure the velocityCompensationControlRegister: {}", nameId(), e.what());
                                    deinitializeNode();
                                }
                            }
                            else
                            {
                                deinitializeNode();
                            }
                        }

                        if (isSelected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Selects the type of velocity compensation performed by the VPE");

                if (ImGui::InputFloat(fmt::format("Velocity Tuning##{}", size_t(id)).c_str(), &velocityCompensationControlRegister.velocityTuning))
                {
                    LOG_DEBUG("{}: velocityCompensationControlRegister.velocityTuning changed to {}", nameId(), velocityCompensationControlRegister.velocityTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVelocityCompensationControl(velocityCompensationControlRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the velocityCompensationControlRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Tuning parameter for the velocity measurement");

                if (ImGui::InputFloat(fmt::format("Rate Tuning##{}", size_t(id)).c_str(), &velocityCompensationControlRegister.rateTuning))
                {
                    LOG_DEBUG("{}: velocityCompensationControlRegister.rateTuning changed to {}", nameId(), velocityCompensationControlRegister.rateTuning);
                    flow::ApplyChanges();
                    if (isInitialized() && vs.isConnected() && vs.verifySensorConnectivity())
                    {
                        try
                        {
                            vs.writeVelocityCompensationControl(velocityCompensationControlRegister);
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("{}: Could not configure the velocityCompensationControlRegister: {}", nameId(), e.what());
                            deinitializeNode();
                        }
                    }
                    else
                    {
                        deinitializeNode();
                    }
                }
                ImGui::SameLine();
                gui::widgets::HelpMarker("Tuning parameter for the angular rate measurement");

                ImGui::TreePop();
            }
        }
    }
}

[[nodiscard]] json NAV::VectorNavSensor::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["UartSensor"] = UartSensor::save();
    j["sensorModel"] = sensorModel;

    // ###########################################################################################################
    //                                               SYSTEM MODULE
    // ###########################################################################################################

    j["asyncDataOutputType"] = asyncDataOutputType;
    j["asyncDataOutputFrequency"] = asyncDataOutputFrequency;
    j["asciiOutputBufferSize"] = asciiOutputBufferSize;
    j["synchronizationControlRegister"] = synchronizationControlRegister;
    j["communicationProtocolControlRegister"] = communicationProtocolControlRegister;
    for (size_t b = 0; b < 3; b++)
    {
        j[fmt::format("binaryOutputRegister{}", b + 1)] = binaryOutputRegister.at(b);
        j[fmt::format("binaryOutputRegister{}-Frequency", b + 1)] = dividerFrequency.second.at(static_cast<size_t>(binaryOutputSelectedFrequency.at(b)));
    }

    // ###########################################################################################################
    //                                               IMU SUBSYSTEM
    // ###########################################################################################################

    j["referenceFrameRotationMatrix"] = referenceFrameRotationMatrix;
    j["imuFilteringConfigurationRegister"] = imuFilteringConfigurationRegister;
    j["deltaThetaAndDeltaVelocityConfigurationRegister"] = deltaThetaAndDeltaVelocityConfigurationRegister;

    // ###########################################################################################################
    //                                              GNSS SUBSYSTEM
    // ###########################################################################################################

    j["gpsConfigurationRegister"] = gpsConfigurationRegister;
    j["gpsAntennaOffset"] = gpsAntennaOffset;
    j["gpsCompassBaselineRegister"] = gpsCompassBaselineRegister;

    // ###########################################################################################################
    //                                            ATTITUDE SUBSYSTEM
    // ###########################################################################################################

    j["vpeBasicControlRegister"] = vpeBasicControlRegister;
    j["vpeMagnetometerBasicTuningRegister"] = vpeMagnetometerBasicTuningRegister;
    j["vpeAccelerometerBasicTuningRegister"] = vpeAccelerometerBasicTuningRegister;
    j["vpeGyroBasicTuningRegister"] = vpeGyroBasicTuningRegister;
    j["filterStartupGyroBias"] = filterStartupGyroBias;

    // ###########################################################################################################
    //                                               INS SUBSYSTEM
    // ###########################################################################################################

    j["insBasicConfigurationRegisterVn300"] = insBasicConfigurationRegisterVn300;
    j["startupFilterBiasEstimateRegister"] = startupFilterBiasEstimateRegister;

    // ###########################################################################################################
    //                                    HARD/SOFT IRON ESTIMATOR SUBSYSTEM
    // ###########################################################################################################

    j["magnetometerCalibrationControlRegister"] = magnetometerCalibrationControlRegister;

    // ###########################################################################################################
    //                                      WORLD MAGNETIC & GRAVITY MODULE
    // ###########################################################################################################

    j["magneticAndGravityReferenceVectorsRegister"] = magneticAndGravityReferenceVectorsRegister;
    j["referenceVectorConfigurationRegister"] = referenceVectorConfigurationRegister;

    // ###########################################################################################################
    //                                              VELOCITY AIDING
    // ###########################################################################################################

    j["velocityCompensationControlRegister"] = velocityCompensationControlRegister;

    return j;
}

void NAV::VectorNavSensor::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("UartSensor"))
    {
        UartSensor::restore(j.at("UartSensor"));
    }
    if (j.contains("sensorModel"))
    {
        j.at("sensorModel").get_to(sensorModel);
    }

    // ###########################################################################################################
    //                                               SYSTEM MODULE
    // ###########################################################################################################

    if (j.contains("asyncDataOutputType"))
    {
        j.at("asyncDataOutputType").get_to(asyncDataOutputType);
    }
    if (j.contains("asyncDataOutputFrequency"))
    {
        j.at("asyncDataOutputFrequency").get_to(asyncDataOutputFrequency);
        asyncDataOutputFrequencySelected = static_cast<int>(std::find(possibleAsyncDataOutputFrequency.begin(), possibleAsyncDataOutputFrequency.end(), static_cast<int>(asyncDataOutputFrequency))
                                                            - possibleAsyncDataOutputFrequency.begin());
    }
    if (j.contains("asciiOutputBufferSize"))
    {
        j.at("asciiOutputBufferSize").get_to(asciiOutputBufferSize);
        asciiOutputBuffer.resize(static_cast<size_t>(asciiOutputBufferSize));
    }
    if (j.contains("synchronizationControlRegister"))
    {
        j.at("synchronizationControlRegister").get_to(synchronizationControlRegister);
    }
    if (j.contains("communicationProtocolControlRegister"))
    {
        j.at("communicationProtocolControlRegister").get_to(communicationProtocolControlRegister);
    }
    for (size_t b = 0; b < 3; b++)
    {
        if (j.contains(fmt::format("binaryOutputRegister{}", b + 1)))
        {
            j.at(fmt::format("binaryOutputRegister{}", b + 1)).get_to(binaryOutputRegister.at(b));
        }
        if (j.contains(fmt::format("binaryOutputRegister{}-Frequency", b + 1)))
        {
            std::string frequency;
            j.at(fmt::format("binaryOutputRegister{}-Frequency", b + 1)).get_to(frequency);
            for (size_t i = 0; i < dividerFrequency.second.size(); i++)
            {
                if (dividerFrequency.second.at(i) == frequency)
                {
                    binaryOutputSelectedFrequency.at(b) = i;
                    break;
                }
            }
        }
    }

    // ###########################################################################################################
    //                                               IMU SUBSYSTEM
    // ###########################################################################################################

    if (j.contains("referenceFrameRotationMatrix"))
    {
        j.at("referenceFrameRotationMatrix").get_to(referenceFrameRotationMatrix);
    }
    if (j.contains("imuFilteringConfigurationRegister"))
    {
        j.at("imuFilteringConfigurationRegister").get_to(imuFilteringConfigurationRegister);
    }
    if (j.contains("deltaThetaAndDeltaVelocityConfigurationRegister"))
    {
        j.at("deltaThetaAndDeltaVelocityConfigurationRegister").get_to(deltaThetaAndDeltaVelocityConfigurationRegister);
    }

    // ###########################################################################################################
    //                                              GNSS SUBSYSTEM
    // ###########################################################################################################

    if (j.contains("gpsConfigurationRegister"))
    {
        j.at("gpsConfigurationRegister").get_to(gpsConfigurationRegister);
    }
    if (j.contains("gpsAntennaOffset"))
    {
        j.at("gpsAntennaOffset").get_to(gpsAntennaOffset);
    }
    if (j.contains("gpsCompassBaselineRegister"))
    {
        j.at("gpsCompassBaselineRegister").get_to(gpsCompassBaselineRegister);
    }

    // ###########################################################################################################
    //                                            ATTITUDE SUBSYSTEM
    // ###########################################################################################################

    if (j.contains("vpeBasicControlRegister"))
    {
        j.at("vpeBasicControlRegister").get_to(vpeBasicControlRegister);
    }
    if (j.contains("vpeMagnetometerBasicTuningRegister"))
    {
        j.at("vpeMagnetometerBasicTuningRegister").get_to(vpeMagnetometerBasicTuningRegister);
    }
    if (j.contains("vpeAccelerometerBasicTuningRegister"))
    {
        j.at("vpeAccelerometerBasicTuningRegister").get_to(vpeAccelerometerBasicTuningRegister);
    }
    if (j.contains("vpeGyroBasicTuningRegister"))
    {
        j.at("vpeGyroBasicTuningRegister").get_to(vpeGyroBasicTuningRegister);
    }
    if (j.contains("filterStartupGyroBias"))
    {
        j.at("filterStartupGyroBias").get_to(filterStartupGyroBias);
    }

    // ###########################################################################################################
    //                                               INS SUBSYSTEM
    // ###########################################################################################################

    if (j.contains("insBasicConfigurationRegisterVn300"))
    {
        j.at("insBasicConfigurationRegisterVn300").get_to(insBasicConfigurationRegisterVn300);
    }
    if (j.contains("startupFilterBiasEstimateRegister"))
    {
        j.at("startupFilterBiasEstimateRegister").get_to(startupFilterBiasEstimateRegister);
    }

    // ###########################################################################################################
    //                                    HARD/SOFT IRON ESTIMATOR SUBSYSTEM
    // ###########################################################################################################

    if (j.contains("magnetometerCalibrationControlRegister"))
    {
        j.at("magnetometerCalibrationControlRegister").get_to(magnetometerCalibrationControlRegister);
    }

    // ###########################################################################################################
    //                                      WORLD MAGNETIC & GRAVITY MODULE
    // ###########################################################################################################

    if (j.contains("magneticAndGravityReferenceVectorsRegister"))
    {
        j.at("magneticAndGravityReferenceVectorsRegister").get_to(magneticAndGravityReferenceVectorsRegister);
    }
    if (j.contains("referenceVectorConfigurationRegister"))
    {
        j.at("referenceVectorConfigurationRegister").get_to(referenceVectorConfigurationRegister);
    }

    // ###########################################################################################################
    //                                              VELOCITY AIDING
    // ###########################################################################################################

    if (j.contains("velocityCompensationControlRegister"))
    {
        j.at("velocityCompensationControlRegister").get_to(velocityCompensationControlRegister);
    }
}

bool NAV::VectorNavSensor::resetNode()
{
    return true;
}

bool NAV::VectorNavSensor::initialize()
{
    LOG_TRACE("{}: called", nameId());

    // ###########################################################################################################
    //                                                Connecting
    // ###########################################################################################################

    // Choose baudrate
    Baudrate targetBaudrate = sensorBaudrate() == BAUDRATE_FASTEST
                                  ? static_cast<Baudrate>(vn::sensors::VnSensor::supportedBaudrates()[vn::sensors::VnSensor::supportedBaudrates().size() - 1])
                                  : sensorBaudrate();

    Baudrate connectedBaudrate{};
    // Search for the VectorNav Sensor
    if (int32_t foundBaudrate = 0;
        vn::sensors::Searcher::search(sensorPort, &foundBaudrate))
    {
        // Sensor was found at specified port with the baudrate 'foundBaudrate'
        connectedBaudrate = static_cast<Baudrate>(foundBaudrate);
    }
    else if (std::vector<std::pair<std::string, uint32_t>> foundSensors = vn::sensors::Searcher::search();
             !foundSensors.empty())
    {
        if (foundSensors.size() == 1)
        {
            sensorPort = foundSensors.at(0).first;
            connectedBaudrate = static_cast<Baudrate>(foundSensors.at(0).second);
        }
        else
        {
            sensorPort = "";
            // Some VectorNav sensors where found, try to identify the wanted one by it's name
            for (auto [port, baudrate] : foundSensors)
            {
                vs.connect(port, baudrate);
                std::string modelNumber = vs.readModelNumber();
                vs.disconnect();

                LOG_DEBUG("{}: Found VectorNav Sensor {} on port {} with baudrate {}", nameId(), modelNumber, port, baudrate);

                // Regex search may be better, but simple find is used here
                if (modelNumber.find(name) != std::string::npos)
                {
                    sensorPort = port;
                    connectedBaudrate = static_cast<Baudrate>(baudrate);
                    break;
                }
            }
            // Sensor could not be identified
            if (sensorPort.empty())
            {
                // This point is also reached if a sensor is connected with USB but external power is off
                LOG_ERROR("{}: Could not connect", nameId());
                return false;
            }
        }
    }
    else
    {
        LOG_ERROR("{}: Could not connect. Is the sensor connected and do you have read permissions?", nameId());
        return false;
    }

    try
    {
        // Connect to the sensor (vs.verifySensorConnectivity does not have to be called as sensor is already tested)
        vs.connect(sensorPort, connectedBaudrate);
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("{}: Failed to connect to sensor on port {} with baudrate {} with error: {}", nameId(),
                  sensorPort, connectedBaudrate, e.what());
        return false;
    }

    if (!vs.verifySensorConnectivity())
    {
        LOG_ERROR("{}: Connected to sensor on port {} with baudrate {} but sensor does not answer", nameId(),
                  sensorPort, connectedBaudrate);
        return false;
    }
    // Query the sensor's model number
    LOG_DEBUG("{}: {} connected on port {} with baudrate {}", nameId(), vs.readModelNumber(), sensorPort, connectedBaudrate);

    // ###########################################################################################################
    //                                               SYSTEM MODULE
    // ###########################################################################################################

    // Change Connection Baudrate
    if (targetBaudrate != connectedBaudrate)
    {
        auto suppBaud = vn::sensors::VnSensor::supportedBaudrates();
        if (std::find(suppBaud.begin(), suppBaud.end(), targetBaudrate) != suppBaud.end())
        {
            vs.changeBaudRate(targetBaudrate);
            LOG_DEBUG("{}: Baudrate changed to {}", nameId(), static_cast<size_t>(targetBaudrate));
        }
        else
        {
            LOG_ERROR("{}: Does not support baudrate {}", nameId(), static_cast<size_t>(targetBaudrate));
            return false;
        }
    }
    if (vs.readSerialBaudRate() != targetBaudrate)
    {
        LOG_ERROR("{}: Changing the baudrate from {} to {} was not successfull", nameId(), vs.readSerialBaudRate(), targetBaudrate);
        deinitializeNode();
        return false;
    }

    vs.writeSynchronizationControl(synchronizationControlRegister);
    vs.writeCommunicationProtocolControl(communicationProtocolControlRegister);

    // vs.writeSynchronizationStatus(vn::sensors::SynchronizationStatusRegister); // User manual VN-310 - 8.3.1 (p 105) / VN-100 - 5.3.1 (p 76)

    // ###########################################################################################################
    //                                               IMU SUBSYSTEM
    // ###########################################################################################################

    // vs.writeMagnetometerCompensation(vn::sensors::MagnetometerCompensationRegister()); // User manual VN-310 - 9.2.1 (p 111) / VN-100 - 6.2.1 (p 82)
    // vs.writeAccelerationCompensation(vn::sensors::AccelerationCompensationRegister()); // User manual VN-310 - 9.2.2 (p 112) / VN-100 - 6.2.2 (p 83)
    // vs.writeGyroCompensation(vn::sensors::GyroCompensationRegister());                 // User manual VN-310 - 9.2.3 (p 113) / VN-100 - 6.2.3 (p 84)

    vs.writeReferenceFrameRotation(referenceFrameRotationMatrix);
    vs.writeImuFilteringConfiguration(imuFilteringConfigurationRegister);
    vs.writeDeltaThetaAndDeltaVelocityConfiguration(deltaThetaAndDeltaVelocityConfigurationRegister);

    // ###########################################################################################################
    //                                               GNSS SUBSYSTEM
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN310)
    {
        vs.writeGpsConfiguration(gpsConfigurationRegister);
        vs.writeGpsAntennaOffset(gpsAntennaOffset);
        vs.writeGpsCompassBaseline(gpsCompassBaselineRegister);
    }

    // ###########################################################################################################
    //                                             ATTITUDE SUBSYSTEM
    // ###########################################################################################################
    if (sensorModel == VectorNavModel::VN100_VN110)
    {
        // vs.tare(); // User manual VN-100 - 7.1.1 (p 92)
        // vs.magneticDisturbancePresent(bool); // User manual VN-100 - 7.1.2 (p 92)
        // vs.accelerationDisturbancePresent(true); // User manual VN-100 - 7.1.3 (p 92f)
    }
    // vs.setGyroBias(); // User manual VN-310 - 11.1.1 (p 148) / VN-100 - 7.1.4 (p 93)

    // TODO: Implement in vnproglib: vs.setInitialHeading() - User manual VN-310 - 11.1.2 (p 148)

    vs.writeVpeBasicControl(vpeBasicControlRegister);

    if (sensorModel == VectorNavModel::VN100_VN110)
    {
        vs.writeVpeMagnetometerBasicTuning(vpeMagnetometerBasicTuningRegister);
        vs.writeVpeAccelerometerBasicTuning(vpeAccelerometerBasicTuningRegister);
        vs.writeFilterStartupGyroBias(filterStartupGyroBias);
        vs.writeVpeGyroBasicTuning(vpeGyroBasicTuningRegister);
    }

    // ###########################################################################################################
    //                                               INS SUBSYSTEM
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN310)
    {
        vs.writeInsBasicConfigurationVn300(insBasicConfigurationRegisterVn300);
        vs.writeStartupFilterBiasEstimate(startupFilterBiasEstimateRegister);
    }

    // ###########################################################################################################
    //                                     HARD/SOFT IRON ESTIMATOR SUBSYSTEM
    // ###########################################################################################################

    vs.writeMagnetometerCalibrationControl(magnetometerCalibrationControlRegister);

    // ###########################################################################################################
    //                                      WORLD MAGNETIC & GRAVITY MODULE
    // ###########################################################################################################

    vs.writeMagneticAndGravityReferenceVectors(magneticAndGravityReferenceVectorsRegister);
    vs.writeReferenceVectorConfiguration(referenceVectorConfigurationRegister);

    // ###########################################################################################################
    //                                              Velocity Aiding
    // ###########################################################################################################

    if (sensorModel == VectorNavModel::VN100_VN110)
    {
        vs.writeVelocityCompensationControl(velocityCompensationControlRegister);

        // vs.writeVelocityCompensationMeasurement(vn::math::vec3f); // User manual VN-100 - 10.3.1 (p 124)
    }

    // ###########################################################################################################
    //                                                  Outputs
    // ###########################################################################################################

    vs.writeAsyncDataOutputType(asyncDataOutputType);
    if (asyncDataOutputType != vn::protocol::uart::AsciiAsync::VNOFF)
    {
        vs.writeAsyncDataOutputFrequency(asyncDataOutputFrequency);
    }

    size_t binaryOutputRegisterCounter = 1; // To give a proper error message
    try
    {
        vs.writeBinaryOutput1(binaryOutputRegister.at(0));
        binaryOutputRegisterCounter++;
        vs.writeBinaryOutput2(binaryOutputRegister.at(1));
        binaryOutputRegisterCounter++;
        vs.writeBinaryOutput3(binaryOutputRegister.at(2));
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("{}: Could not configure binary output register {}: {}", nameId(), binaryOutputRegisterCounter, e.what());
        deinitializeNode();
        return false;
    }

    // TODO: Implement in vnproglib: vs.writeNmeaOutput1(...) - User manual VN-310 - 8.2.14 (p 103)
    // TODO: Implement in vnproglib: vs.writeNmeaOutput2(...) - User manual VN-310 - 8.2.15 (p 104)

    vs.registerAsyncPacketReceivedHandler(this, asciiOrBinaryAsyncMessageReceived);

    LOG_DEBUG("{} successfully initialized", nameId());

    return true;
}

void NAV::VectorNavSensor::deinitialize()
{
    LOG_TRACE("{}: called", nameId());

    if (!isInitialized())
    {
        return;
    }

    if (vs.isConnected())
    {
        try
        {
            vs.unregisterAsyncPacketReceivedHandler();
        }
        catch (...)
        {
            LOG_TRACE("{}: Could not unregisterAsyncPacketReceivedHandler", nameId());
        }
        try
        {
            vs.reset(true);
        }
        catch (...)
        {
            LOG_TRACE("{}: Could not reset", nameId());
        }
        try
        {
            vs.disconnect();
        }
        catch (...)
        {
            LOG_TRACE("{}: Could not disconnect", nameId());
        }
    }
}

void NAV::VectorNavSensor::asciiOrBinaryAsyncMessageReceived(void* userData, vn::protocol::uart::Packet& p, [[maybe_unused]] size_t index)
{
    auto* vnSensor = static_cast<VectorNavSensor*>(userData);

    LOG_DATA("{}: Received message", vnSensor->nameId());

    if (p.type() == vn::protocol::uart::Packet::TYPE_BINARY)
    {
        for (size_t b = 0; b < 3; b++)
        {
            // Make sure that the binary packet is from the type we expect
            if (p.isCompatible(vnSensor->binaryOutputRegister.at(b).commonField,
                               vnSensor->binaryOutputRegister.at(b).timeField,
                               vnSensor->binaryOutputRegister.at(b).imuField,
                               vnSensor->binaryOutputRegister.at(b).gpsField,
                               vnSensor->binaryOutputRegister.at(b).attitudeField,
                               vnSensor->binaryOutputRegister.at(b).insField,
                               vnSensor->binaryOutputRegister.at(b).gps2Field))
            {
                auto obs = std::make_shared<VectorNavBinaryOutput>(vnSensor->imuPos);

                // Group 1 (Common)
                if (vnSensor->binaryOutputRegister.at(b).commonField != vn::protocol::uart::CommonGroup::COMMONGROUP_NONE)
                {
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMESTARTUP)
                    {
                        if (!obs->timeOutputs)
                        {
                            obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                            obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                        }
                        obs->timeOutputs->timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTARTUP;
                        obs->timeOutputs->timeStartup = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPS)
                    {
                        if (!obs->timeOutputs)
                        {
                            obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                            obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                        }
                        obs->timeOutputs->timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS;
                        obs->timeOutputs->timeStartup = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMESYNCIN)
                    {
                        if (!obs->timeOutputs)
                        {
                            obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                            obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                        }
                        obs->timeOutputs->timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESYNCIN;
                        obs->timeOutputs->timeSyncIn = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_YAWPITCHROLL)
                    {
                        if (!obs->attitudeOutputs)
                        {
                            obs->attitudeOutputs = std::make_shared<NAV::sensors::vectornav::AttitudeOutputs>();
                            obs->attitudeOutputs->attitudeField |= vnSensor->binaryOutputRegister.at(b).attitudeField;
                        }
                        obs->attitudeOutputs->attitudeField |= vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_YAWPITCHROLL;
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->ypr = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_QUATERNION)
                    {
                        if (!obs->attitudeOutputs)
                        {
                            obs->attitudeOutputs = std::make_shared<NAV::sensors::vectornav::AttitudeOutputs>();
                            obs->attitudeOutputs->attitudeField |= vnSensor->binaryOutputRegister.at(b).attitudeField;
                        }
                        obs->attitudeOutputs->attitudeField |= vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_QUATERNION;
                        auto vec = p.extractVec4f();
                        obs->attitudeOutputs->qtn = { vec.w, vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_ANGULARRATE)
                    {
                        if (!obs->imuOutputs)
                        {
                            obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                            obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                        }
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_ANGULARRATE;
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->angularRate = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_POSITION)
                    {
                        if (!obs->insOutputs)
                        {
                            obs->insOutputs = std::make_shared<NAV::sensors::vectornav::InsOutputs>();
                            obs->insOutputs->insField |= vnSensor->binaryOutputRegister.at(b).insField;
                        }
                        obs->insOutputs->insField |= vn::protocol::uart::InsGroup::INSGROUP_POSLLA;
                        auto vec = p.extractVec3d();
                        obs->insOutputs->posLla = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_VELOCITY)
                    {
                        if (!obs->insOutputs)
                        {
                            obs->insOutputs = std::make_shared<NAV::sensors::vectornav::InsOutputs>();
                            obs->insOutputs->insField |= vnSensor->binaryOutputRegister.at(b).insField;
                        }
                        obs->insOutputs->insField |= vn::protocol::uart::InsGroup::INSGROUP_VELNED;
                        auto vec = p.extractVec3f();
                        obs->insOutputs->velNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_ACCEL)
                    {
                        if (!obs->imuOutputs)
                        {
                            obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                            obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                        }
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_ACCEL;
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->accel = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_IMU)
                    {
                        if (!obs->imuOutputs)
                        {
                            obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                            obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                        }
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPACCEL;
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->uncompAccel = { vec.x, vec.y, vec.z };
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPGYRO;
                        vec = p.extractVec3f();
                        obs->imuOutputs->uncompGyro = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_MAGPRES)
                    {
                        if (!obs->imuOutputs)
                        {
                            obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                            obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                        }
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_MAG;
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->mag = { vec.x, vec.y, vec.z };
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_TEMP;
                        obs->imuOutputs->temp = p.extractFloat();
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_PRES;
                        obs->imuOutputs->pres = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_DELTATHETA)
                    {
                        if (!obs->imuOutputs)
                        {
                            obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                            obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                        }
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_DELTATHETA;
                        obs->imuOutputs->deltaTime = p.extractFloat();
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->deltaTheta = { vec.x, vec.y, vec.z };
                        obs->imuOutputs->imuField |= vn::protocol::uart::ImuGroup::IMUGROUP_DELTAVEL;
                        vec = p.extractVec3f();
                        obs->imuOutputs->deltaV = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_INSSTATUS)
                    {
                        if (!obs->insOutputs)
                        {
                            obs->insOutputs = std::make_shared<NAV::sensors::vectornav::InsOutputs>();
                            obs->insOutputs->insField |= vnSensor->binaryOutputRegister.at(b).insField;
                        }
                        obs->insOutputs->insField |= vn::protocol::uart::InsGroup::INSGROUP_INSSTATUS;
                        obs->insOutputs->insStatus = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMESYNCIN)
                    {
                        if (!obs->timeOutputs)
                        {
                            obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                            obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                        }
                        obs->timeOutputs->timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_SYNCINCNT;
                        obs->timeOutputs->syncInCnt = p.extractUint32();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).commonField & vn::protocol::uart::CommonGroup::COMMONGROUP_TIMEGPSPPS)
                    {
                        if (!obs->timeOutputs)
                        {
                            obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                            obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                        }
                        obs->timeOutputs->timeField |= vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPSPPS;
                        obs->timeOutputs->timePPS = p.extractUint64();
                    }
                }
                // Group 2 (Time)
                if (vnSensor->binaryOutputRegister.at(b).timeField != vn::protocol::uart::TimeGroup::TIMEGROUP_NONE)
                {
                    if (!obs->timeOutputs)
                    {
                        obs->timeOutputs = std::make_shared<NAV::sensors::vectornav::TimeOutputs>();
                        obs->timeOutputs->timeField |= vnSensor->binaryOutputRegister.at(b).timeField;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTARTUP)
                    {
                        obs->timeOutputs->timeStartup = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS)
                    {
                        obs->timeOutputs->timeGps = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSTOW)
                    {
                        obs->timeOutputs->gpsTow = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSWEEK)
                    {
                        obs->timeOutputs->gpsWeek = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESYNCIN)
                    {
                        obs->timeOutputs->timeSyncIn = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPSPPS)
                    {
                        obs->timeOutputs->timePPS = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEUTC)
                    {
                        obs->timeOutputs->timeUtc.year = p.extractInt8();
                        obs->timeOutputs->timeUtc.month = p.extractUint8();
                        obs->timeOutputs->timeUtc.day = p.extractUint8();
                        obs->timeOutputs->timeUtc.hour = p.extractUint8();
                        obs->timeOutputs->timeUtc.min = p.extractUint8();
                        obs->timeOutputs->timeUtc.sec = p.extractUint8();
                        obs->timeOutputs->timeUtc.ms = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_SYNCINCNT)
                    {
                        obs->timeOutputs->syncInCnt = p.extractUint32();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_SYNCOUTCNT)
                    {
                        obs->timeOutputs->syncOutCnt = p.extractUint32();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS)
                    {
                        obs->timeOutputs->timeStatus = p.extractUint8();
                    }
                }
                // Group 3 (IMU)
                if (vnSensor->binaryOutputRegister.at(b).imuField != vn::protocol::uart::ImuGroup::IMUGROUP_NONE)
                {
                    if (!obs->imuOutputs)
                    {
                        obs->imuOutputs = std::make_shared<NAV::sensors::vectornav::ImuOutputs>();
                        obs->imuOutputs->imuField |= vnSensor->binaryOutputRegister.at(b).imuField;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_IMUSTATUS)
                    {
                        obs->imuOutputs->imuStatus = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPMAG)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->uncompMag = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPACCEL)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->uncompAccel = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_UNCOMPGYRO)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->uncompGyro = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_TEMP)
                    {
                        obs->imuOutputs->temp = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_PRES)
                    {
                        obs->imuOutputs->pres = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_DELTATHETA)
                    {
                        obs->imuOutputs->deltaTime = p.extractFloat();
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->deltaTheta = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_DELTAVEL)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->deltaV = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_MAG)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->mag = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_ACCEL)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->accel = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).imuField & vn::protocol::uart::ImuGroup::IMUGROUP_ANGULARRATE)
                    {
                        auto vec = p.extractVec3f();
                        obs->imuOutputs->angularRate = { vec.x, vec.y, vec.z };
                    }
                }
                // Group 4 (GNSS1)
                if (vnSensor->binaryOutputRegister.at(b).gpsField != vn::protocol::uart::GpsGroup::GPSGROUP_NONE)
                {
                    if (!obs->gnss1Outputs)
                    {
                        obs->gnss1Outputs = std::make_shared<NAV::sensors::vectornav::GnssOutputs>();
                        obs->gnss1Outputs->gnssField |= vnSensor->binaryOutputRegister.at(b).gpsField;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_UTC)
                    {
                        obs->gnss1Outputs->timeUtc.year = p.extractInt8();
                        obs->gnss1Outputs->timeUtc.month = p.extractUint8();
                        obs->gnss1Outputs->timeUtc.day = p.extractUint8();
                        obs->gnss1Outputs->timeUtc.hour = p.extractUint8();
                        obs->gnss1Outputs->timeUtc.min = p.extractUint8();
                        obs->gnss1Outputs->timeUtc.sec = p.extractUint8();
                        obs->gnss1Outputs->timeUtc.ms = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                    {
                        obs->gnss1Outputs->tow = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK)
                    {
                        obs->gnss1Outputs->week = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_NUMSATS)
                    {
                        obs->gnss1Outputs->numSats = p.extractUint8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_FIX)
                    {
                        obs->gnss1Outputs->fix = p.extractUint8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_POSLLA)
                    {
                        auto vec = p.extractVec3d();
                        obs->gnss1Outputs->posLla = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_POSECEF)
                    {
                        auto vec = p.extractVec3d();
                        obs->gnss1Outputs->posEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_VELNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss1Outputs->velNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_VELECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss1Outputs->velEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_POSU)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss1Outputs->posU = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_VELU)
                    {
                        obs->gnss1Outputs->velU = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEU)
                    {
                        obs->gnss1Outputs->timeU = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO)
                    {
                        obs->gnss1Outputs->timeInfo.status = p.extractUint8();
                        obs->gnss1Outputs->timeInfo.leapSeconds = p.extractInt8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_DOP)
                    {
                        obs->gnss1Outputs->dop.gDop = p.extractFloat();
                        obs->gnss1Outputs->dop.pDop = p.extractFloat();
                        obs->gnss1Outputs->dop.tDop = p.extractFloat();
                        obs->gnss1Outputs->dop.vDop = p.extractFloat();
                        obs->gnss1Outputs->dop.hDop = p.extractFloat();
                        obs->gnss1Outputs->dop.nDop = p.extractFloat();
                        obs->gnss1Outputs->dop.eDop = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_SATINFO)
                    {
                        obs->gnss1Outputs->satInfo.numSats = p.extractUint8();
                        p.extractUint8(); // Reserved for future use
                        for (size_t i = 0; i < obs->gnss1Outputs->satInfo.numSats; i++)
                        {
                            auto sys = p.extractInt8();
                            auto svId = p.extractUint8();
                            auto flags = p.extractUint8();
                            auto cno = p.extractUint8();
                            auto qi = p.extractUint8();
                            auto el = p.extractInt8();
                            auto az = p.extractInt16();
                            obs->gnss1Outputs->satInfo.satellites.emplace_back(sys, svId, flags, cno, qi, el, az);
                        }
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gpsField & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS)
                    {
                        obs->gnss1Outputs->raw.tow = p.extractDouble();
                        obs->gnss1Outputs->raw.week = p.extractUint16();
                        obs->gnss1Outputs->raw.numSats = p.extractUint8();
                        p.extractUint8(); // Reserved for future use
                        for (size_t i = 0; i < obs->gnss1Outputs->raw.numSats; i++)
                        {
                            auto sys = p.extractUint8();
                            auto svId = p.extractUint8();
                            auto freq = p.extractUint8();
                            auto chan = p.extractUint8();
                            auto slot = p.extractInt8();
                            auto cno = p.extractUint8();
                            auto flags = p.extractUint16();
                            auto pr = p.extractDouble();
                            auto cp = p.extractDouble();
                            auto dp = p.extractFloat();
                            obs->gnss1Outputs->raw.satellites.emplace_back(sys, svId, freq, chan, slot, cno, flags, pr, cp, dp);
                        }
                    }
                }
                // Group 5 (Attitude)
                if (vnSensor->binaryOutputRegister.at(b).attitudeField != vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_NONE)
                {
                    if (!obs->attitudeOutputs)
                    {
                        obs->attitudeOutputs = std::make_shared<NAV::sensors::vectornav::AttitudeOutputs>();
                        obs->attitudeOutputs->attitudeField |= vnSensor->binaryOutputRegister.at(b).attitudeField;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_VPESTATUS)
                    {
                        obs->attitudeOutputs->vpeStatus = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_YAWPITCHROLL)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->ypr = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_QUATERNION)
                    {
                        auto vec = p.extractVec4f();
                        obs->attitudeOutputs->qtn = { vec.w, vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_DCM)
                    {
                        auto col0 = p.extractVec3f();
                        auto col1 = p.extractVec3f();
                        auto col2 = p.extractVec3f();
                        obs->attitudeOutputs->dcm << col0.x, col1.x, col2.x,
                            col0.y, col1.y, col2.y,
                            col0.z, col1.z, col2.z;
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_MAGNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->magNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_ACCELNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->accelNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_LINEARACCELBODY)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->linearAccelBody = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_LINEARACCELNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->linearAccelNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).attitudeField & vn::protocol::uart::AttitudeGroup::ATTITUDEGROUP_YPRU)
                    {
                        auto vec = p.extractVec3f();
                        obs->attitudeOutputs->yprU = { vec.x, vec.y, vec.z };
                    }
                }
                // Group 6 (INS)
                if (vnSensor->binaryOutputRegister.at(b).insField != vn::protocol::uart::InsGroup::INSGROUP_NONE)
                {
                    if (!obs->insOutputs)
                    {
                        obs->insOutputs = std::make_shared<NAV::sensors::vectornav::InsOutputs>();
                        obs->insOutputs->insField |= vnSensor->binaryOutputRegister.at(b).insField;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_INSSTATUS)
                    {
                        obs->insOutputs->insStatus = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_POSLLA)
                    {
                        auto vec = p.extractVec3d();
                        obs->insOutputs->posLla = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_POSECEF)
                    {
                        auto vec = p.extractVec3d();
                        obs->insOutputs->posEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_VELBODY)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->velBody = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_VELNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->velNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_VELECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->velEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_MAGECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->magEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_ACCELECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->accelEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_LINEARACCELECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->insOutputs->linearAccelEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_POSU)
                    {
                        obs->insOutputs->posU = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).insField & vn::protocol::uart::InsGroup::INSGROUP_VELU)
                    {
                        obs->insOutputs->velU = p.extractFloat();
                    }
                }
                // Group 7 (GNSS2)
                if (vnSensor->binaryOutputRegister.at(b).gps2Field != vn::protocol::uart::GpsGroup::GPSGROUP_NONE)
                {
                    if (!obs->gnss2Outputs)
                    {
                        obs->gnss2Outputs = std::make_shared<NAV::sensors::vectornav::GnssOutputs>();
                        obs->gnss2Outputs->gnssField |= vnSensor->binaryOutputRegister.at(b).gps2Field;
                    }

                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_UTC)
                    {
                        obs->gnss2Outputs->timeUtc.year = p.extractInt8();
                        obs->gnss2Outputs->timeUtc.month = p.extractUint8();
                        obs->gnss2Outputs->timeUtc.day = p.extractUint8();
                        obs->gnss2Outputs->timeUtc.hour = p.extractUint8();
                        obs->gnss2Outputs->timeUtc.min = p.extractUint8();
                        obs->gnss2Outputs->timeUtc.sec = p.extractUint8();
                        obs->gnss2Outputs->timeUtc.ms = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                    {
                        obs->gnss2Outputs->tow = p.extractUint64();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK)
                    {
                        obs->gnss2Outputs->week = p.extractUint16();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_NUMSATS)
                    {
                        obs->gnss2Outputs->numSats = p.extractUint8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_FIX)
                    {
                        obs->gnss2Outputs->fix = p.extractUint8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_POSLLA)
                    {
                        auto vec = p.extractVec3d();
                        obs->gnss2Outputs->posLla = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_POSECEF)
                    {
                        auto vec = p.extractVec3d();
                        obs->gnss2Outputs->posEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_VELNED)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss2Outputs->velNed = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_VELECEF)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss2Outputs->velEcef = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_POSU)
                    {
                        auto vec = p.extractVec3f();
                        obs->gnss2Outputs->posU = { vec.x, vec.y, vec.z };
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_VELU)
                    {
                        obs->gnss2Outputs->velU = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEU)
                    {
                        obs->gnss2Outputs->timeU = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO)
                    {
                        obs->gnss2Outputs->timeInfo.status = p.extractUint8();
                        obs->gnss2Outputs->timeInfo.leapSeconds = p.extractInt8();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_DOP)
                    {
                        obs->gnss2Outputs->dop.gDop = p.extractFloat();
                        obs->gnss2Outputs->dop.pDop = p.extractFloat();
                        obs->gnss2Outputs->dop.tDop = p.extractFloat();
                        obs->gnss2Outputs->dop.vDop = p.extractFloat();
                        obs->gnss2Outputs->dop.hDop = p.extractFloat();
                        obs->gnss2Outputs->dop.nDop = p.extractFloat();
                        obs->gnss2Outputs->dop.eDop = p.extractFloat();
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_SATINFO)
                    {
                        obs->gnss2Outputs->satInfo.numSats = p.extractUint8();
                        p.extractUint8(); // Reserved for future use
                        for (size_t i = 0; i < obs->gnss2Outputs->satInfo.numSats; i++)
                        {
                            auto sys = p.extractInt8();
                            auto svId = p.extractUint8();
                            auto flags = p.extractUint8();
                            auto cno = p.extractUint8();
                            auto qi = p.extractUint8();
                            auto el = p.extractInt8();
                            auto az = p.extractInt16();
                            obs->gnss2Outputs->satInfo.satellites.emplace_back(sys, svId, flags, cno, qi, el, az);
                        }
                    }
                    if (vnSensor->binaryOutputRegister.at(b).gps2Field & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS)
                    {
                        obs->gnss2Outputs->raw.tow = p.extractDouble();
                        obs->gnss2Outputs->raw.week = p.extractUint16();
                        obs->gnss2Outputs->raw.numSats = p.extractUint8();
                        p.extractUint8(); // Reserved for future use
                        for (size_t i = 0; i < obs->gnss2Outputs->raw.numSats; i++)
                        {
                            auto sys = p.extractUint8();
                            auto svId = p.extractUint8();
                            auto freq = p.extractUint8();
                            auto chan = p.extractUint8();
                            auto slot = p.extractInt8();
                            auto cno = p.extractUint8();
                            auto flags = p.extractUint16();
                            auto pr = p.extractDouble();
                            auto cp = p.extractDouble();
                            auto dp = p.extractFloat();
                            obs->gnss2Outputs->raw.satellites.emplace_back(sys, svId, freq, chan, slot, cno, flags, pr, cp, dp);
                        }
                    }
                }

                if (p.getCurExtractLoc() != p.getPacketLength() - 2) // 2 Bytes CRC should be left
                {
                    LOG_DEBUG("{}: Only {} of {} bytes were extracted from the Binary Output {}", vnSensor->nameId(), p.getCurExtractLoc(), p.getPacketLength(), b + 1);
                }

                // --------------------------------------------- Fetch InsTime -----------------------------------------------
                // Group 2 (Time)
                if (obs->timeOutputs)
                {
                    if (obs->timeOutputs->timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMESTATUS)
                    {
                        if (obs->timeOutputs->timeStatus.dateOk())
                        {
                            if (obs->timeOutputs->timeStatus.timeOk()
                                && (obs->timeOutputs->timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSTOW)
                                && (obs->timeOutputs->timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_GPSWEEK))
                            {
                                obs->insTime.emplace(InsTime_GPSweekTow(0, obs->timeOutputs->gpsWeek, obs->timeOutputs->gpsTow * 1e-9L));
                                util::time::SetCurrentTime(obs->insTime.value());
                            }
                            else if (obs->timeOutputs->timeField & vn::protocol::uart::TimeGroup::TIMEGROUP_TIMEGPS)
                            {
                                auto secondsSinceEpoche = static_cast<long double>(obs->timeOutputs->timeGps) * 1e-9L;
                                auto week = static_cast<uint16_t>(secondsSinceEpoche / static_cast<long double>(InsTimeUtil::SECONDS_PER_DAY * InsTimeUtil::DAYS_PER_WEEK));
                                auto tow = secondsSinceEpoche - week * InsTimeUtil::SECONDS_PER_DAY * InsTimeUtil::DAYS_PER_WEEK;

                                obs->insTime.emplace(InsTime_GPSweekTow(0, week, tow));
                                util::time::SetCurrentTime(obs->insTime.value());
                            }
                        }
                    }
                }
                // TODO: Calculate time from GNSS. The value only changes with the GNSS update rate. Therefore all IMU messages in between have the same time
                // Group 4 (GNSS1)
                // if ((!obs->insTime.has_value() || obs->insTime->empty()) && obs->gnss1Outputs)
                // {
                //     if (obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO)
                //     {
                //         if (obs->gnss1Outputs->timeInfo.status.dateOk() && obs->gnss1Outputs->timeInfo.status.timeOk()
                //             && (((obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                //                  && (obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK))
                //                 || obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS))
                //         {
                //             if ((obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                //                 && (obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK))
                //             {
                //                 obs->insTime.emplace(InsTime_GPSweekTow(0, obs->gnss1Outputs->week, obs->gnss1Outputs->tow * 1e-9L));
                //             }
                //             else // obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS
                //             {
                //                 obs->insTime.emplace(InsTime_GPSweekTow(0, obs->gnss1Outputs->raw.week, obs->gnss1Outputs->raw.tow * 1e-9L));
                //             }
                //         }
                //         else if (obs->gnss1Outputs->timeInfo.status.utcTimeValid()
                //                  && obs->gnss1Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_UTC)
                //         {
                //             obs->insTime.emplace(InsTime_YMDHMS(2000 + obs->gnss1Outputs->timeUtc.year,
                //                                                 obs->gnss1Outputs->timeUtc.month,
                //                                                 obs->gnss1Outputs->timeUtc.day,
                //                                                 obs->gnss1Outputs->timeUtc.hour,
                //                                                 obs->gnss1Outputs->timeUtc.min,
                //                                                 obs->gnss1Outputs->timeUtc.sec + static_cast<long double>(obs->gnss1Outputs->timeUtc.ms) * 1e-3L));
                //         }
                //     }
                // }
                // // Group 7 (GNSS2)
                // if ((!obs->insTime.has_value() || obs->insTime->empty()) && obs->gnss2Outputs)
                // {
                //     if (obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TIMEINFO)
                //     {
                //         if (obs->gnss2Outputs->timeInfo.status.dateOk() && obs->gnss2Outputs->timeInfo.status.timeOk()
                //             && (((obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                //                  && (obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK))
                //                 || obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS))
                //         {
                //             if ((obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_TOW)
                //                 && (obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_WEEK))
                //             {
                //                 obs->insTime.emplace(InsTime_GPSweekTow(0, obs->gnss2Outputs->week, obs->gnss2Outputs->tow * 1e-9L));
                //             }
                //             else // obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_RAWMEAS
                //             {
                //                 obs->insTime.emplace(InsTime_GPSweekTow(0, obs->gnss2Outputs->raw.week, obs->gnss2Outputs->raw.tow * 1e-9L));
                //             }
                //         }
                //         else if (obs->gnss2Outputs->timeInfo.status.utcTimeValid()
                //                  && obs->gnss2Outputs->gnssField & vn::protocol::uart::GpsGroup::GPSGROUP_UTC)
                //         {
                //             obs->insTime.emplace(InsTime_YMDHMS(2000 + obs->gnss2Outputs->timeUtc.year,
                //                                                 obs->gnss2Outputs->timeUtc.month,
                //                                                 obs->gnss2Outputs->timeUtc.day,
                //                                                 obs->gnss2Outputs->timeUtc.hour,
                //                                                 obs->gnss2Outputs->timeUtc.min,
                //                                                 obs->gnss2Outputs->timeUtc.sec + static_cast<long double>(obs->gnss2Outputs->timeUtc.ms) * 1e-3L));
                //         }
                //     }
                // }

                if (!obs->insTime.has_value() || obs->insTime->empty())
                {
                    if (InsTime currentTime = util::time::GetCurrentInsTime();
                        !currentTime.empty())
                    {
                        obs->insTime = currentTime;
                    }
                }

                if (obs->insTime.has_value() && !obs->insTime->empty())
                {
                    if (!vnSensor->lastMessageTime.at(b).empty())
                    {
                        // FIXME: This seems like a bug in clang-tidy. Check if it is working in future versions of clang-tidy
                        // NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
                        if (obs->insTime.value() - vnSensor->lastMessageTime.at(b) >= std::chrono::milliseconds(static_cast<int>(1.5 / IMU_DEFAULT_FREQUENCY * vnSensor->binaryOutputRegister.at(b).rateDivisor)))
                        {
                            LOG_WARN("{}: Potentially lost a message. Previous message was at {} and current message at {} which is a time difference of {} seconds.", vnSensor->nameId(),
                                     vnSensor->lastMessageTime.at(b), obs->insTime.value(), (obs->insTime.value() - vnSensor->lastMessageTime.at(b)).count());
                        }
                    }
                    vnSensor->lastMessageTime.at(b) = obs->insTime.value();
                }

                // Calls all the callbacks
                vnSensor->invokeCallbacks(b + 1, obs);
            }
        }
    }
    else if (p.type() == vn::protocol::uart::Packet::TYPE_ASCII)
    {
        LOG_DATA("{} received an ASCII Async message: {}", vnSensor->nameId(), p.datastr());
        vnSensor->asciiOutputBuffer.push_back(p.datastr());

        auto obs = std::make_shared<StringObs>(p.datastr());

        if (InsTime currentTime = util::time::GetCurrentInsTime();
            !currentTime.empty())
        {
            obs->insTime = currentTime;
        }
        // Calls all the callbacks
        vnSensor->invokeCallbacks(VectorNavSensor::OutputPortIndex_AsciiOutput, obs);
    }
}