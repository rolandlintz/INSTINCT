#include "SkydelImuStream.hpp"

#include "util/Debug.hpp"
#include "util/Logger.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/IMU/ImuObs.hpp"

#include "util/Time/TimeBase.hpp"

// Includes for IMU Stream
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::udp;
//using namespace boost::algorithm;
//using boost::algorithm;

NAV::SkydelImuStream::SkydelImuStream()
    : m_senderEndpoint(udp::v4(), m_port), m_socket(ioservice, m_senderEndpoint)
{
    name = typeStatic();

    // LOG_TRACE("{}: called", name);

    color = ImColor(255, 128, 128);
    hasConfig = true;
    // breakStream = false;

    //m_port = 4444;
    //m_senderEndpoint = boost::asio::ip::udp::endpoint(udp::v4(), m_port);
    // m_socket = boost::asio::ip::udp::socket(ioservice, m_senderEndpoint);

    nm::CreateOutputPin(this, "ImuObs", Pin::Type::Flow, NAV::ImuObs::type());
    counter = 0;
}

NAV::SkydelImuStream::~SkydelImuStream()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::SkydelImuStream::typeStatic()
{
    return "SkydelImuStream";
}

std::string NAV::SkydelImuStream::type() const
{
    return typeStatic();
}

std::string NAV::SkydelImuStream::category()
{
    return "Data Provider";
}

void NAV::SkydelImuStream::guiConfig()
{
    /*  // Checkbox Beispiel von TT
    static bool chk1 = false;
    if (ImGui::Checkbox("Test##1", &chk1))
    {
        LOG_DEBUG("Test Button 1");
    }
    static bool chk2 = false;
    if (ImGui::Checkbox("Test##2", &chk2))
    {
        LOG_DEBUG("Test Button 2");
    }
    */
}

[[nodiscard]] json NAV::SkydelImuStream::save() const
{
    //LOG_TRACE("{}: called", nameId());

    json j;

    j["Frequency"] = 1;

    return j;
}

void NAV::SkydelImuStream::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());

    if (j.contains("Frequency"))
    {
        // j.at("Frequency").get_to(outputFrequency);
    }
}

bool NAV::SkydelImuStream::resetNode()
{
    return true;
}

void NAV::SkydelImuStream::do_receive()
{
    m_socket.async_receive_from(
        boost::asio::buffer(m_data, max_length), m_senderEndpoint,
        [this](boost::system::error_code ed, std::size_t sdfe) {
            // std::cout << ed << sdfe;

            counter++;

            if ((!ed) & (sdfe > 0)) {}

            std::vector<std::string> v;
            boost::algorithm::split(v, m_data, boost::is_any_of(","));
            //std::cout << std::stod(v.at(0)) *10.0 << '\n';
            //std::cout << m_data;
            // LOG_TRACE("{}: reading 4444", m_data);

            // LOG_TRACE("mm: Test");
            //if (InsTime currentTime = util::time::GetCurrentTime();
            //   !currentTime.empty())
            //{
            //    obs->in sTime = currentTime;
            //}

            if (counter > 5)
            {
                counter = 0;

                if (v.size() == 7)
                {
                    auto obs = std::make_shared<ImuObs>(this->imuPos);
                    obs->timeSinceStartup = std::stod(v.at(0)) * 1e6;

                    // if (startTime.empty())
                    // {
                    //     startTime = util::time::GetCurrentTime();
                    //     timeSinceStartupStart = obs->timeSinceStartup.value();
                    // }

                    // obs->insTime = startTime + std::chrono::milliseconds((obs->timeSinceStartup.value() - timeSinceStartupStart));

                    obs->accelCompXYZ.emplace(std::stod(v.at(1)), std::stod(v.at(2)), std::stod(v.at(3)));
                    obs->gyroCompXYZ.emplace(std::stod(v.at(4)) - 0 * 2.54542332900245e-07, std::stod(v.at(5)),
                                             std::stod(v.at(6)) + 0 * 7.29207107393694e-05);
                    obs->magCompXYZ.emplace(0.0, 0.0, 0.0);

                    // Calls all the callbacks
                    if (InsTime currentTime = util::time::GetCurrentTime();
                        !currentTime.empty())
                    {
                        obs->insTime = currentTime;
                    }

                    this->invokeCallbacks(OutputPortIndex_ImuObs, obs);
                }
            }

            // if (!breakStream)
            // {
            do_receive();
            // }
            // else
            // {
            //     TestThread.join();
            // }
        });
}

bool NAV::SkydelImuStream::initialize()
{
    //    sensor = std::make_unique<>();

    //    int ReceiverThread()
    //  {
    // breakStream = false;

    do_receive();

    TestThread = std::thread([=, this]() { ioservice.run(); });
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //io_context.run();

    //return 0;
    //};

    LOG_TRACE("{}: initialized", name);

    return true;
}

void NAV::SkydelImuStream::deinitialize()
{
    LOG_TRACE("{}: called", nameId());

    // breakStream = true;

    // LOG_TRACE("{}: de-initialized", nameId());

    // if (!isInitialized())
    // {
    //     return;
    // }

    //TestThread.join();

    // startTime = InsTime{};
}

void NAV::SkydelImuStream::readImuThread(void* userData)
{
    //LOG_TRACE("mm: Test");
    //std::cout << userData << std::endl;
    //return true;
    //std::cout << "mmTest" << std::endl;
    auto* skydel = static_cast<SkydelImuStream*>(userData);
    auto obs = std::make_shared<ImuObs>(skydel->imuPos);
    LOG_TRACE("mm: Test");
    //if (InsTime currentTime = util::time::GetCurrentTime();
    //   !currentTime.empty())
    //{
    //    obs->insTime = currentTime;
    //}
    skydel->invokeCallbacks(OutputPortIndex_ImuObs, obs);
}