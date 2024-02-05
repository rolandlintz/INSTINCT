// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "UbloxGnssObsConverter.hpp"

#include <map>

#include "util/Logger.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "NodeData/GNSS/UbloxObs.hpp"
#include "NodeData/GNSS/GnssObs.hpp"

NAV::UbloxGnssObsConverter::UbloxGnssObsConverter()
    : Node(typeStatic())
{
    LOG_TRACE("{}: called", name);
    _hasConfig = false;

    nm::CreateInputPin(this, "UbloxObs", Pin::Type::Flow, { NAV::UbloxObs::type() }, &UbloxGnssObsConverter::receiveObs);

    nm::CreateOutputPin(this, "GnssObs", Pin::Type::Flow, { NAV::GnssObs::type() });
}

NAV::UbloxGnssObsConverter::~UbloxGnssObsConverter()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::UbloxGnssObsConverter::typeStatic()
{
    return "UbloxGnssObsConverter";
}

std::string NAV::UbloxGnssObsConverter::type() const
{
    return typeStatic();
}

std::string NAV::UbloxGnssObsConverter::category()
{
    return "Converter";
}

bool NAV::UbloxGnssObsConverter::initialize()
{
    LOG_TRACE("{}: called", nameId());

    return true;
}

void NAV::UbloxGnssObsConverter::receiveObs(NAV::InputPin::NodeDataQueue& queue, size_t /* pinIdx */)
{
    auto ubloxObs = std::static_pointer_cast<const UbloxObs>(queue.extract_front());

    namespace ubx = vendor::ublox;

    if (ubloxObs->msgClass == ubx::UBX_CLASS_RXM)
    {
        if (static_cast<ubx::UbxRxmMessages>(ubloxObs->msgId) == ubx::UbxRxmMessages::UBX_RXM_RAWX)
        {
            LOG_DATA("{}: Converting message at [{}]", nameId(), ubloxObs->insTime.toYMDHMS(GPST));
            auto gnssObs = std::make_shared<GnssObs>();
            gnssObs->insTime = ubloxObs->insTime;

            const auto& ubxRxmRawx = std::get<ubx::UbxRxmRawx>(ubloxObs->data);

            for (const auto& satSys : SatelliteSystem::GetAll())
            {
                std::map<uint16_t, GnssObs::ObservationData> sortedObsData;
                for (const auto& data : ubxRxmRawx.data)
                {
                    if (ubx::getSatSys(data.gnssId) != satSys) { continue; }

                    SatSigId satSigId(ubx::getCode(data.gnssId, data.sigId), data.svId);
                    // LOG_DATA("{}: Reading [{}]", nameId(), satSigId);
                    GnssObs::ObservationData obsData(satSigId);
                    if (data.prValid())
                    {
                        obsData.pseudorange = GnssObs::ObservationData::Pseudorange{
                            .value = data.prMes,
                            .SSI = 0,
                        };
                    }
                    if (data.cpValid())
                    {
                        obsData.carrierPhase = GnssObs::ObservationData::CarrierPhase{
                            .value = data.cpMes,
                            .SSI = 0,
                            .LLI = 0,
                        };
                    }
                    obsData.doppler = data.doMes;
                    obsData.CN0 = data.cno;

                    sortedObsData.insert(std::make_pair(satSigId.satNum, obsData));
                    gnssObs->satData(satSigId.toSatId()).frequencies |= satSigId.freq();
                }
                for (const auto& obsData : sortedObsData)
                {
                    // LOG_DATA("{}: Adding [{}]", nameId(), obsData.second.satSigId);
                    gnssObs->data.push_back(obsData.second);
                }
            }

            invokeCallbacks(OUTPUT_PORT_INDEX_GNSS_OBS, gnssObs);
        }
    }
}