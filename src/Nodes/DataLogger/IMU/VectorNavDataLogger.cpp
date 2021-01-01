#include "VectorNavDataLogger.hpp"

#include "util/Logger.hpp"

#include "imgui_stdlib.h"
#include "ImGuiFileDialog.h"

#include <iomanip> // std::setprecision
#include <functional>

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;

NAV::VectorNavDataLogger::VectorNavDataLogger()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);

    fileType = FileType::ASCII;

    color = ImColor(255, 128, 128);
    hasConfig = true;

    nm::CreateOutputPin(this, "", Pin::Type::Delegate, "VectorNavDataLogger", this);

    nm::CreateInputPin(this, "writeObservation", Pin::Type::Flow, NAV::VectorNavObs::type(), &VectorNavDataLogger::writeObservation);
}

NAV::VectorNavDataLogger::~VectorNavDataLogger()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::VectorNavDataLogger::typeStatic()
{
    return "VectorNavDataLogger";
}

std::string NAV::VectorNavDataLogger::type() const
{
    return typeStatic();
}

std::string NAV::VectorNavDataLogger::category()
{
    return "Data Logger";
}

void NAV::VectorNavDataLogger::config()
{
    // Filepath
    ImGui::InputText("Filepath", &path);
    ImGui::SameLine();
    std::string saveFileDialogKey = fmt::format("Save VectorNav File ({})", id.AsPointer());
    if (ImGui::Button("Save"))
    {
        igfd::ImGuiFileDialog::Instance()->OpenDialog(saveFileDialogKey, "Save VectorNav File", ".csv", "");
        igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".csv", ImVec4(0.0F, 1.0F, 0.0F, 0.9F));
    }

    if (igfd::ImGuiFileDialog::Instance()->FileDialog(saveFileDialogKey))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk)
        {
            path = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
            LOG_DEBUG("Selected file: {}", path);
            initialize();
        }

        igfd::ImGuiFileDialog::Instance()->CloseDialog(saveFileDialogKey);
    }
}

[[nodiscard]] json NAV::VectorNavDataLogger::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["FileWriter"] = FileWriter::save();

    return j;
}

void NAV::VectorNavDataLogger::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("FileWriter"))
    {
        FileWriter::restore(j.at("FileWriter"));
    }
}

bool NAV::VectorNavDataLogger::initialize()
{
    LOG_TRACE("{}: called", nameId());

    if (!Node::initialize()
        || !FileWriter::initialize())
    {
        return false;
    }

    filestream << "GpsCycle,GpsWeek,GpsToW,TimeStartup,TimeSyncIn,SyncInCnt,"
               << "UnCompMagX,UnCompMagY,UnCompMagZ,UnCompAccX,UnCompAccY,UnCompAccZ,UnCompGyroX,UnCompGyroY,UnCompGyroZ,"
               << "Temperature,Pressure,DeltaTime,DeltaThetaX,DeltaThetaY,DeltaThetaZ,DeltaVelX,DeltaVelY,DeltaVelZ,"
               << "MagX,MagY,MagZ,AccX,AccY,AccZ,GyroX,GyroY,GyroZ,AhrsStatus,Yaw,Pitch,Roll,Quat[0],Quat[1],Quat[2],Quat[3],"
               << "MagN,MagE,MagD,AccN,AccE,AccD,LinAccX,LinAccY,LinAccZ,LinAccN,LinAccE,LinAccD,"
               << "YawU,PitchU,RollU,YawRate,PitchRate,RollRate" << std::endl;

    return isInitialized = true;
}

void NAV::VectorNavDataLogger::deinitialize()
{
    LOG_TRACE("{}: called", nameId());

    FileWriter::deinitialize();
    Node::deinitialize();
}

void NAV::VectorNavDataLogger::writeObservation(std::shared_ptr<NodeData> nodeData)
{
    auto obs = std::static_pointer_cast<VectorNavObs>(nodeData);

    constexpr int gpsCyclePrecision = 3;
    constexpr int gpsTimePrecision = 12;
    constexpr int valuePrecision = 9;

    if (obs->insTime.has_value())
    {
        filestream << std::fixed << std::setprecision(gpsCyclePrecision) << obs->insTime.value().toGPSweekTow().gpsCycle;
    }
    filestream << ',';
    if (obs->insTime.has_value())
    {
        filestream << std::defaultfloat << std::setprecision(gpsTimePrecision) << obs->insTime.value().toGPSweekTow().gpsWeek;
    }
    filestream << ',';
    if (obs->insTime.has_value())
    {
        filestream << std::defaultfloat << std::setprecision(gpsTimePrecision) << obs->insTime.value().toGPSweekTow().tow;
    }
    filestream << ',';
    if (obs->timeSinceStartup.has_value())
    {
        filestream << obs->timeSinceStartup.value();
    }
    filestream << ',';
    if (obs->timeSinceSyncIn.has_value())
    {
        filestream << obs->timeSinceSyncIn.value();
    }
    filestream << ',';
    if (obs->syncInCnt.has_value())
    {
        filestream << obs->syncInCnt.value();
    }
    filestream << ',';
    if (obs->magUncompXYZ.has_value())
    {
        filestream << std::setprecision(valuePrecision) << obs->magUncompXYZ.value().x();
    }
    filestream << ',';
    if (obs->magUncompXYZ.has_value())
    {
        filestream << obs->magUncompXYZ.value().y();
    }
    filestream << ',';
    if (obs->magUncompXYZ.has_value())
    {
        filestream << obs->magUncompXYZ.value().z();
    }
    filestream << ',';
    if (obs->accelUncompXYZ.has_value())
    {
        filestream << obs->accelUncompXYZ.value().x();
    }
    filestream << ',';
    if (obs->accelUncompXYZ.has_value())
    {
        filestream << obs->accelUncompXYZ.value().y();
    }
    filestream << ',';
    if (obs->accelUncompXYZ.has_value())
    {
        filestream << obs->accelUncompXYZ.value().z();
    }
    filestream << ',';
    if (obs->gyroUncompXYZ.has_value())
    {
        filestream << obs->gyroUncompXYZ.value().x();
    }
    filestream << ',';
    if (obs->gyroUncompXYZ.has_value())
    {
        filestream << obs->gyroUncompXYZ.value().y();
    }
    filestream << ',';
    if (obs->gyroUncompXYZ.has_value())
    {
        filestream << obs->gyroUncompXYZ.value().z();
    }
    filestream << ',';
    if (obs->temperature.has_value())
    {
        filestream << obs->temperature.value();
    }
    filestream << ',';
    if (obs->pressure.has_value())
    {
        filestream << obs->pressure.value();
    }
    filestream << ',';
    if (obs->dtime.has_value())
    {
        filestream << obs->dtime.value();
    }
    filestream << ',';
    if (obs->dtheta.has_value())
    {
        filestream << obs->dtheta.value().x();
    }
    filestream << ',';
    if (obs->dtheta.has_value())
    {
        filestream << obs->dtheta.value().y();
    }
    filestream << ',';
    if (obs->dtheta.has_value())
    {
        filestream << obs->dtheta.value().z();
    }
    filestream << ',';
    if (obs->dvel.has_value())
    {
        filestream << obs->dvel.value().x();
    }
    filestream << ',';
    if (obs->dvel.has_value())
    {
        filestream << obs->dvel.value().y();
    }
    filestream << ',';
    if (obs->dvel.has_value())
    {
        filestream << obs->dvel.value().z();
    }
    filestream << ',';
    if (obs->magCompXYZ.has_value())
    {
        filestream << obs->magCompXYZ.value().x();
    }
    filestream << ',';
    if (obs->magCompXYZ.has_value())
    {
        filestream << obs->magCompXYZ.value().y();
    }
    filestream << ',';
    if (obs->magCompXYZ.has_value())
    {
        filestream << obs->magCompXYZ.value().z();
    }
    filestream << ',';
    if (obs->accelCompXYZ.has_value())
    {
        filestream << obs->accelCompXYZ.value().x();
    }
    filestream << ',';
    if (obs->accelCompXYZ.has_value())
    {
        filestream << obs->accelCompXYZ.value().y();
    }
    filestream << ',';
    if (obs->accelCompXYZ.has_value())
    {
        filestream << obs->accelCompXYZ.value().z();
    }
    filestream << ',';
    if (obs->gyroCompXYZ.has_value())
    {
        filestream << obs->gyroCompXYZ.value().x();
    }
    filestream << ',';
    if (obs->gyroCompXYZ.has_value())
    {
        filestream << obs->gyroCompXYZ.value().y();
    }
    filestream << ',';
    if (obs->gyroCompXYZ.has_value())
    {
        filestream << obs->gyroCompXYZ.value().z();
    }
    filestream << ',';
    if (obs->vpeStatus.has_value())
    {
        filestream << obs->vpeStatus.value().status;
    }
    filestream << ',';
    if (obs->yawPitchRoll.has_value())
    {
        filestream << obs->yawPitchRoll.value().x();
    }
    filestream << ',';
    if (obs->yawPitchRoll.has_value())
    {
        filestream << obs->yawPitchRoll.value().y();
    }
    filestream << ',';
    if (obs->yawPitchRoll.has_value())
    {
        filestream << obs->yawPitchRoll.value().z();
    }
    filestream << ',';
    if (obs->quaternion.has_value())
    {
        filestream << obs->quaternion.value().w();
    }
    filestream << ',';
    if (obs->quaternion.has_value())
    {
        filestream << obs->quaternion.value().x();
    }
    filestream << ',';
    if (obs->quaternion.has_value())
    {
        filestream << obs->quaternion.value().y();
    }
    filestream << ',';
    if (obs->quaternion.has_value())
    {
        filestream << obs->quaternion.value().z();
    }
    filestream << ',';
    if (obs->magCompNED.has_value())
    {
        filestream << obs->magCompNED.value().x();
    }
    filestream << ',';
    if (obs->magCompNED.has_value())
    {
        filestream << obs->magCompNED.value().y();
    }
    filestream << ',';
    if (obs->magCompNED.has_value())
    {
        filestream << obs->magCompNED.value().z();
    }
    filestream << ',';
    if (obs->accelCompNED.has_value())
    {
        filestream << obs->accelCompNED.value().x();
    }
    filestream << ',';
    if (obs->accelCompNED.has_value())
    {
        filestream << obs->accelCompNED.value().y();
    }
    filestream << ',';
    if (obs->accelCompNED.has_value())
    {
        filestream << obs->accelCompNED.value().z();
    }
    filestream << ',';
    if (obs->linearAccelXYZ.has_value())
    {
        filestream << obs->linearAccelXYZ.value().x();
    }
    filestream << ',';
    if (obs->linearAccelXYZ.has_value())
    {
        filestream << obs->linearAccelXYZ.value().y();
    }
    filestream << ',';
    if (obs->linearAccelXYZ.has_value())
    {
        filestream << obs->linearAccelXYZ.value().z();
    }
    filestream << ',';
    if (obs->linearAccelNED.has_value())
    {
        filestream << obs->linearAccelNED.value().x();
    }
    filestream << ',';
    if (obs->linearAccelNED.has_value())
    {
        filestream << obs->linearAccelNED.value().y();
    }
    filestream << ',';
    if (obs->linearAccelNED.has_value())
    {
        filestream << obs->linearAccelNED.value().z();
    }
    filestream << ',';
    if (obs->yawPitchRollUncertainty.has_value())
    {
        filestream << obs->yawPitchRollUncertainty.value().x();
    }
    filestream << ',';
    if (obs->yawPitchRollUncertainty.has_value())
    {
        filestream << obs->yawPitchRollUncertainty.value().y();
    }
    filestream << ',';
    if (obs->yawPitchRollUncertainty.has_value())
    {
        filestream << obs->yawPitchRollUncertainty.value().z();
    }
    filestream << ',';
    if (obs->gyroCompNED.has_value())
    {
        filestream << obs->gyroCompNED.value().x();
    }
    filestream << ',';
    if (obs->gyroCompNED.has_value())
    {
        filestream << obs->gyroCompNED.value().y();
    }
    filestream << ',';
    if (obs->gyroCompNED.has_value())
    {
        filestream << obs->gyroCompNED.value().z();
    }
    filestream << '\n';
}