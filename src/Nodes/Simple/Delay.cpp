#include "Delay.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/InsObs.hpp"

NAV::Delay::Delay()
    : Node(fmt::format("z^-{}", _delayLength))
{
    LOG_TRACE("{}: called", name);

    _hasConfig = true;
    _guiConfigDefaultWindowSize = { 305, 70 };
    kind = Kind::Simple;

    nm::CreateInputPin(this, "", Pin::Type::Flow, { InsObs::type() }, &Delay::delayObs);
    nm::CreateOutputPin(this, "", Pin::Type::Flow, { InsObs::type() });
}

NAV::Delay::~Delay()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::Delay::typeStatic()
{
    return "Delay";
}

std::string NAV::Delay::type() const
{
    return typeStatic();
}

std::string NAV::Delay::category()
{
    return "Simple";
}

void NAV::Delay::guiConfig()
{
    if (ImGui::InputInt(fmt::format("Delay length##{}", size_t(id)).c_str(), &_delayLength))
    {
        if (_delayLength < 1)
        {
            _delayLength = 1;
        }
        LOG_DEBUG("{}: delayLength changed to {}", nameId(), _delayLength);
        if (name.starts_with("z^-"))
        {
            name = fmt::format("z^-{}", _delayLength);
        }

        while (_buffer.size() > static_cast<size_t>(_delayLength))
        {
            _buffer.pop_front();
        }
    }
}

[[nodiscard]] json NAV::Delay::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["delayLength"] = _delayLength;

    return j;
}

void NAV::Delay::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("delayLength"))
    {
        j.at("delayLength").get_to(_delayLength);
    }
}

bool NAV::Delay::initialize()
{
    LOG_TRACE("{}: called", nameId());

    _buffer.clear();

    return true;
}

void NAV::Delay::deinitialize()
{
    LOG_TRACE("{}: called", nameId());
}

bool NAV::Delay::onCreateLink(OutputPin& startPin, InputPin& endPin)
{
    LOG_TRACE("{}: called for {} ==> {}", nameId(), size_t(startPin.id), size_t(endPin.id));

    if (endPin.parentNode->id != id)
    {
        return true; // Link on Output Port
    }

    // New Link on the Input port, but the previously connected dataIdentifier is different from the new one.
    // Then remove all links.
    if (outputPins.at(OUTPUT_PORT_INDEX_FLOW).dataIdentifier != startPin.dataIdentifier)
    {
        outputPins.at(OUTPUT_PORT_INDEX_FLOW).deleteLinks();
    }

    // Update the dataIdentifier of the output pin to the same as input pin
    outputPins.at(OUTPUT_PORT_INDEX_FLOW).dataIdentifier = startPin.dataIdentifier;

    // Refresh all links connected to the output pin
    for (auto& link : outputPins.at(OUTPUT_PORT_INDEX_FLOW).links)
    {
        if (auto* connectedPin = link.getConnectedPin())
        {
            outputPins.at(OUTPUT_PORT_INDEX_FLOW).recreateLink(*connectedPin);
        }
    }

    return true;
}

void NAV::Delay::delayObs(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::PinId /* pinId */)
{
    if (_buffer.size() == static_cast<size_t>(_delayLength))
    {
        auto oldest = _buffer.front();
        _buffer.pop_front();
        _buffer.push_back(nodeData);

        if (auto obs = std::static_pointer_cast<const InsObs>(nodeData))
        {
            LOG_DATA("{}: Delay pushing out message: {}", nameId(), obs->insTime->toGPSweekTow());
        }
        else
        {
            LOG_DATA("{}: Delay pushing out message", nameId());
        }

        if (!(NAV::Node::callbacksEnabled))
        {
            NAV::Node::callbacksEnabled = true;
        }

        invokeCallbacks(OUTPUT_PORT_INDEX_FLOW, oldest);
    }
    else
    {
        _buffer.push_back(nodeData);
    }
}