#include "NodeRegistry.hpp"
#include "Nodes/NodeManager.hpp"

#include "Nodes/DataLogger/IMU/VectorNavDataLogger.hpp"
#include "Nodes/DataLogger/GNSS/UbloxDataLogger.hpp"
#include "Nodes/DataLogger/GNSS/EmlidDataLogger.hpp"

#include "Nodes/DataProvider/IMU/FileReader/VectorNavFile.hpp"
#include "Nodes/DataProvider/IMU/Sensors/VectorNavSensor.hpp"
#include "Nodes/DataProvider/GNSS/Sensors/UbloxSensor.hpp"
#include "Nodes/DataProvider/GNSS/Sensors/EmlidSensor.hpp"
#include "Nodes/DataProvider/GNSS/FileReader/RtklibPosFile.hpp"
#include "Nodes/DataProvider/GNSS/FileReader/UbloxFile.hpp"
#include "Nodes/DataProvider/GNSS/FileReader/EmlidFile.hpp"

#include "Nodes/GnuPlot/GNSS/RtklibPosGnuPlot.hpp"
#include "Nodes/GnuPlot/IMU/VectorNavGnuPlot.hpp"

#include "Nodes/Integrator/ImuIntegrator.hpp"

#include "Nodes/Synchronizer/TimeSynchronizer/TimeSynchronizer.hpp"
#include "Nodes/Synchronizer/UsbSyncSignal/GNSS/UbloxSyncSignal.hpp"

void NAV::NodeRegistry::registerNodeTypes(NAV::NodeManager& nodeManager)
{
    nodeManager.registerNodeType<NAV::VectorNavDataLogger>();
    nodeManager.registerNodeType<NAV::UbloxDataLogger>();
    nodeManager.registerNodeType<NAV::EmlidDataLogger>();

    nodeManager.registerNodeType<NAV::VectorNavFile>();
    nodeManager.registerNodeType<NAV::RtklibPosFile>();
    nodeManager.registerNodeType<NAV::UbloxFile>();
    nodeManager.registerNodeType<NAV::EmlidFile>();
#ifndef DISABLE_SENSORS
    nodeManager.registerNodeType<NAV::VectorNavSensor>();
    nodeManager.registerNodeType<NAV::UbloxSensor>();
    nodeManager.registerNodeType<NAV::EmlidSensor>();
#endif

    nodeManager.registerNodeType<NAV::RtklibPosGnuPlot>();
    nodeManager.registerNodeType<NAV::VectorNavGnuPlot>();

    nodeManager.registerNodeType<NAV::ImuIntegrator>();

    nodeManager.registerNodeType<NAV::TimeSynchronizer>();
#ifndef DISABLE_SENSORS
    nodeManager.registerNodeType<NAV::UbloxSyncSignal>();
#endif
}

#include "NodeData/InsObs.hpp"

#include "NodeData/IMU/ImuObs.hpp"
#include "NodeData/IMU/VectorNavObs.hpp"

#include "NodeData/GNSS/GnssObs.hpp"
#include "NodeData/GNSS/RtklibPosObs.hpp"
#include "NodeData/GNSS/UbloxObs.hpp"
#include "NodeData/GNSS/EmlidObs.hpp"

void NAV::NodeRegistry::registerNodeDataTypes(NAV::NodeManager& nodeManager)
{
    nodeManager.registerNodeDataType<NAV::InsObs>();

    nodeManager.registerNodeDataType<NAV::ImuObs>();
    nodeManager.registerNodeDataType<NAV::VectorNavObs>();

    nodeManager.registerNodeDataType<NAV::GnssObs>();
    nodeManager.registerNodeDataType<NAV::RtklibPosObs>();
    nodeManager.registerNodeDataType<NAV::UbloxObs>();
    nodeManager.registerNodeDataType<NAV::EmlidObs>();
}