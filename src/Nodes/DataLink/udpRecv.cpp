// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "udpRecv.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "internal/gui/widgets/HelpMarker.hpp"
#include "internal/gui/NodeEditorApplication.hpp"

#include "util/Time/TimeBase.hpp"

#include "util/Logger.hpp"

NAV::UdpRecv::UdpRecv()
    : Node(typeStatic()), _socket(_io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), static_cast<unsigned short>(_port)))
{
    LOG_TRACE("{}: called", name);

    _hasConfig = true;
    _guiConfigDefaultWindowSize = { 202, 66 };

    nm::CreateOutputPin(this, "PosVelAtt", Pin::Type::Flow, { NAV::PosVelAtt::type() });
}

NAV::UdpRecv::~UdpRecv()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::UdpRecv::typeStatic()
{
    return "UdpRecv";
}

std::string NAV::UdpRecv::type() const
{
    return typeStatic();
}

std::string NAV::UdpRecv::category()
{
    return "Data Link";
}

void NAV::UdpRecv::guiConfig()
{
    ImGui::SetNextItemWidth(150 * gui::NodeEditorApplication::windowFontRatio());
    if (ImGui::InputInt(fmt::format("Port##{}", size_t(id)).c_str(), &_port))
    {
        flow::ApplyChanges();
    }
}

bool NAV::UdpRecv::resetNode()
{
    return true;
}

json NAV::UdpRecv::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;
    j["port"] = _port;

    return j;
}

void NAV::UdpRecv::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());
    if (j.contains("port"))
    {
        j.at("port").get_to(_port);
    }
}

bool NAV::UdpRecv::initialize()
{
    LOG_TRACE("{}: called", nameId());
    _running = true;
    _flagsenderstopped = 0.0;

    pollData();

    if (_isStartup)
    {
        _recvThread = std::thread([=, this]() {
            _io_context.run();
        });
    }
    else
    {
        _recvThread = std::thread([=, this]() {
            _io_context.restart();
            _io_context.run();
        });
    }

    _isStartup = false;

    return true;
}

void NAV::UdpRecv::deinitialize()
{
    _running = false;
    _io_context.stop();
    _recvThread.join();

    LOG_TRACE("{}: called", nameId());
}

void NAV::UdpRecv::pollData()
{
    _socket.async_receive_from(
        boost::asio::buffer(_data, _maxBytes), _sender_endpoint,
        [this](boost::system::error_code errorRcvd, std::size_t bytesRcvd) {
            if ((!errorRcvd) && (bytesRcvd > 0))
            {
                auto obs = std::make_shared<PosVelAtt>();

                // Position in LLA coordinates
                Eigen::Vector3d posLLA{ _data.at(0), _data.at(1), _data.at(2) };

                // Velocity in local frame
                Eigen::Vector3d vel_n{ _data.at(3), _data.at(4), _data.at(5) };

                // Attitude
                Eigen::Quaterniond n_Quat_b{};
                n_Quat_b.x() = _data.at(6);
                n_Quat_b.y() = _data.at(7);
                n_Quat_b.z() = _data.at(8);
                n_Quat_b.w() = _data.at(9);

                obs->setPosition_lla(posLLA);
                obs->setVelocity_n(vel_n);
                obs->setAttitude_n_Quat_b(n_Quat_b);

                InsTime currentTime = util::time::GetCurrentInsTime();
                if (!currentTime.empty())
                {
                    obs->insTime = currentTime;
                }

                // if (obs->timeSinceStartup.has_value())
                // {
                //     if (_lastMessageTime)
                //     {
                //         // FIXME: This seems like a bug in clang-tidy. Check if it is working in future versions of clang-tidy
                //         // NOLINTNEXTLINE(hicpp-use-nullptr, modernize-use-nullptr)
                //         if (obs->timeSinceStartup.value() - _lastMessageTime >= static_cast<uint64_t>(1.5 / _dataRate * 1e9))
                //         {
                //             LOG_WARN("{}: Potentially lost a message. Previous message was at {} and current message at {} which is a time difference of {} seconds.", nameId(),
                //                      _lastMessageTime, obs->timeSinceStartup.value(), static_cast<double>(obs->timeSinceStartup.value() - _lastMessageTime) * 1e-9);
                //         }
                //     }
                //     _lastMessageTime = obs->timeSinceStartup.value();
                // }

                this->invokeCallbacks(OUTPUT_PORT_INDEX_NODE_DATA, obs);
            }
            else
            {
                LOG_ERROR("Error receiving the UDP network stream.");
            }

            if (_running)
            {
                pollData();
            }
        });
}