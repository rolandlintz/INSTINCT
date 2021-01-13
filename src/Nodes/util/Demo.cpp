#include "Demo.hpp"

#include "util/Logger.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/InsObs.hpp"

#include <chrono>

NAV::Demo::Demo()
{
    name = typeStatic();

    LOG_TRACE("{}: called", name);

    color = ImColor(255, 128, 128);
    hasConfig = true;

    nm::CreateOutputPin(this, "", Pin::Type::Delegate, typeStatic(), this);
    nm::CreateOutputPin(this, "Sensor\nData", Pin::Type::Flow, NAV::NodeData::type());
    nm::CreateOutputPin(this, "FileReader\n Data", Pin::Type::Flow, NAV::InsObs::type(), &Demo::pollData);
    nm::CreateOutputPin(this, "Bool", Pin::Type::Bool, "", &valueBool);
    nm::CreateOutputPin(this, "Int", Pin::Type::Int, "", &valueInt);
    nm::CreateOutputPin(this, "Float", Pin::Type::Float, "", &valueFloat);
    nm::CreateOutputPin(this, "Double", Pin::Type::Float, "", &valueDouble);
    nm::CreateOutputPin(this, "String", Pin::Type::String, "", &valueString);
    nm::CreateOutputPin(this, "Object", Pin::Type::Object, "Demo::DemoData", &valueObject);
    nm::CreateOutputPin(this, "Matrix", Pin::Type::Matrix, "", &valueMatrix);
    nm::CreateOutputPin(this, "Function", Pin::Type::Function, "Demo::DemoData (*)(int, bool)", &Demo::callbackFunction);

    nm::CreateInputPin(this, "Demo Node", Pin::Type::Delegate, typeStatic());
    nm::CreateInputPin(this, "Sensor\nData", Pin::Type::Flow, NAV::NodeData::type(), &Demo::receiveSensorData);
    nm::CreateInputPin(this, "FileReader\n Data", Pin::Type::Flow, NAV::InsObs::type(), &Demo::receiveFileReaderData);
    nm::CreateInputPin(this, "Bool", Pin::Type::Bool);
    nm::CreateInputPin(this, "Int", Pin::Type::Int);
    nm::CreateInputPin(this, "Float", Pin::Type::Float);
    nm::CreateInputPin(this, "Double", Pin::Type::Float);
    nm::CreateInputPin(this, "String", Pin::Type::String);
    nm::CreateInputPin(this, "Object", Pin::Type::Object, "Demo::DemoData1");
    nm::CreateInputPin(this, "Matrix", Pin::Type::Matrix);
    nm::CreateInputPin(this, "Function", Pin::Type::Function, "Demo::DemoData1 (*)(int, bool)");
}

NAV::Demo::~Demo()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::Demo::typeStatic()
{
    return "Demo";
}

std::string NAV::Demo::type() const
{
    return typeStatic();
}

std::string NAV::Demo::category()
{
    return "Demo";
}

void NAV::Demo::guiConfig()
{
}

[[nodiscard]] json NAV::Demo::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    // j["dynamicStateInit"] = dynamicStateInit;

    return j;
}

void NAV::Demo::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("dynamicStateInit"))
    {
        // j.at("dynamicStateInit").get_to(dynamicStateInit);
    }
}

bool NAV::Demo::initialize()
{
    deinitialize();

    LOG_TRACE("{}: called", nameId());

    if (!Node::initialize())
    {
        return false;
    }

    iPollData = 0;

    valueMatrix = Eigen::MatrixXd::Identity(3, 3);

    return isInitialized = true;
}

void NAV::Demo::deinitialize()
{
    LOG_TRACE("{}: called", nameId());

    Node::deinitialize();
}

void NAV::Demo::resetNode()
{
    // Here you could reset a FileReader
}

bool NAV::Demo::onCreateLink([[maybe_unused]] Pin* startPin, [[maybe_unused]] Pin* endPin)
{
    LOG_TRACE("{}: called for {} ==> {}", nameId(), size_t(startPin->id), size_t(endPin->id));

    return true;
}

void NAV::Demo::onDeleteLink([[maybe_unused]] Pin* startPin, [[maybe_unused]] Pin* endPin)
{
    LOG_TRACE("{}: called for {} ==> {}", nameId(), size_t(startPin->id), size_t(endPin->id));
}

void NAV::Demo::receiveSensorData(const std::shared_ptr<NodeData>& /*nodeData*/, ax::NodeEditor::LinkId /*linkId*/)
{
    LOG_INFO("{}: received Sensor Data", nameId());
}

void NAV::Demo::receiveFileReaderData(const std::shared_ptr<NodeData>& nodeData, ax::NodeEditor::LinkId /*linkId*/)
{
    auto obs = std::static_pointer_cast<InsObs>(nodeData);
    LOG_INFO("{}: received FileReader Data: {}", nameId(), obs->insTime->GetStringOfDate());
}

std::shared_ptr<NAV::NodeData> NAV::Demo::pollData(bool peek)
{
    if (iPollData++ > nPollData)
    {
        return nullptr;
    }

    auto obs = std::make_shared<InsObs>();

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto* t = std::localtime(&now);

    obs->insTime = InsTime(static_cast<uint16_t>(t->tm_year),
                           static_cast<uint16_t>(t->tm_mon),
                           static_cast<uint16_t>(t->tm_mday),
                           static_cast<uint16_t>(t->tm_hour),
                           static_cast<uint16_t>(t->tm_min),
                           static_cast<long double>(t->tm_sec));

    // Calls all the callbacks
    if (!peek)
    {
        invokeCallbacks(OutputPortIndex_InsObs, obs);
    }

    return obs;
}

NAV::Demo::DemoData NAV::Demo::callbackFunction(int integer, bool boolean)
{
    LOG_INFO("{}: called with integer={}, boolean={}", nameId(), integer, boolean);
    DemoData data;
    data.boolean = boolean;
    data.integer = integer;

    return data;
}