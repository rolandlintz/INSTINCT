#include <catch2/catch.hpp>

#include "util/Logger.hpp"

#include "util/UartSensors/Ublox/UbloxUtilities.hpp"
namespace ub = NAV::sensors::ublox;

#include "uart/protocol/packet.hpp"
#include "uart/sensors/sensors.hpp"

TEST_CASE("[UbloxUtilities] decryptUbloxObs - NMEA", "[UbloxUtilities]")
{
    Logger consoleSink;

    uart::sensors::UartSensor sensor{ uart::Endianness::ENDIAN_LITTLE,
                                      // packetFinderFunction
                                      nullptr,
                                      // packetFinderUserData
                                      nullptr,
                                      // packetTypeFunction
                                      [](const uart::protocol::Packet& /*packet*/) { return uart::protocol::Packet::Type::TYPE_ASCII; },
                                      // isValidFunction
                                      nullptr,
                                      // isErrorFunction
                                      nullptr,
                                      // isResponseFunction
                                      nullptr,
                                      // packetHeaderLength
                                      2 };

    uart::protocol::Packet packet("$GPZDA,141644.00,22,03,2002,00,00*67\r\n", &sensor);
    auto obs = std::make_shared<NAV::UbloxObs>(packet);

    ub::decryptUbloxObs(obs);
    REQUIRE(obs->raw.datastr().substr(0, obs->raw.datastr().size() - 2) == "$GPZDA,141644.00,22,03,2002,00,00*67");
}

TEST_CASE("[UbloxUtilities] decryptUbloxObs - UBX", "[UbloxUtilities]")
{
    Logger consoleSink;

    auto sensorEndianess = uart::Endianness::ENDIAN_LITTLE;
    uart::sensors::UartSensor sensor{ sensorEndianess,
                                      // packetFinderFunction
                                      nullptr,
                                      // packetFinderUserData
                                      nullptr,
                                      // packetTypeFunction
                                      [](const uart::protocol::Packet& /*packet*/) { return uart::protocol::Packet::Type::TYPE_BINARY; },
                                      // isValidFunction
                                      nullptr,
                                      // isErrorFunction
                                      nullptr,
                                      // isResponseFunction
                                      nullptr,
                                      // packetHeaderLength
                                      2 };

    SECTION("CFG")
    {
        //                                                                 navBbrMask
        //                                                                 |           resetMode
        //                                                                 |           |     reserved1
        //                             µ  ,  b  ,Class,  Id ,   Length  ,  |           |     |
        std::vector<uint8_t> data = { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x87, 0x00, 0x00,
                                      0x94, 0xF5 };
        uart::protocol::Packet packet(data, &sensor);
        auto obs = std::make_shared<NAV::UbloxObs>(packet);
        ub::decryptUbloxObs(obs);
        REQUIRE(obs->msgClass == ub::UbxClass::UBX_CLASS_CFG);
        REQUIRE(obs->msgId == ub::UbxCfgMessages::UBX_CFG_RST);
        REQUIRE(obs->payloadLength == 4);
    }

    SECTION("RXM - RAWX")
    {
        //                                                                 rcvToW
        //                                                                 |                                               week
        //                                                                 |                                               |           leapS
        //                                                                 |                                               |           |     numMeas
        //                                                                 |                                               |           |     |     recStat
        //                                                                 |                                               |           |     |     |     version
        //                                                                 |                                               |           |     |     |     |     reserved1
        //                             µ  ,  b  ,Class,  Id ,   Length  ,  |                                               |           |     |     |     |     |
        std::vector<uint8_t> data = { 0xB5, 0x62, 0x02, 0x15, 0x90, 0x03, 0xB8, 0x1E, 0x85, 0xEB, 0xDF, 0x76, 0xFD, 0x40, 0x4C, 0x08, 0x12, 0x1C, 0x01, 0x01, 0x33, 0x39,
                                      0xB0, 0xAE, 0x37, 0x6F, 0x88, 0xB7, 0x81, 0x41, 0x10, 0x3E, 0x8C, 0xAF, 0x98, 0x46, 0xA7, 0x41, 0x9D, 0x74, 0x4C, 0xC5, 0x01, 0x7B, 0x00, 0x00, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xBF, 0x52, 0xD4, 0x24, 0x9C, 0x82, 0x79, 0x41, 0xF4, 0xF9, 0x3D, 0xC1, 0xD2, 0xC1, 0xA0, 0x41, 0xB5, 0x58, 0x02, 0xC5, 0x02, 0x1B, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x2E, 0x4C, 0x68, 0x66, 0x3F, 0xA6, 0x72, 0x41, 0x80, 0xF5, 0xF2, 0xEF, 0x35, 0x80, 0x98, 0x41, 0x6C, 0xC5, 0xA4, 0xC4, 0x00, 0x01, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xAF, 0x20, 0x2D, 0x87, 0xB2, 0xAB, 0x72, 0x41, 0xE4, 0xD1, 0x20, 0xC5, 0x5E, 0x87, 0x98, 0x41, 0x2D, 0xFF, 0xA2, 0xC5, 0x00, 0x08, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x1F, 0x20, 0xB2, 0x26, 0x5E, 0x33, 0x75, 0x41, 0x94, 0x79, 0xDF, 0xB5, 0x40, 0xDA, 0x9B, 0x41, 0x80, 0x12, 0x60, 0x43, 0x00, 0x03, 0x00, 0x00, 0xF4, 0xFB, 0x29, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x23, 0xFE, 0x50, 0x89, 0xC4, 0xF9, 0x74, 0x41, 0x44, 0x7A, 0x8C, 0x73, 0x94, 0x8E, 0x9B, 0x41, 0x15, 0x4B, 0xC7, 0xC5, 0x00, 0x0A, 0x00, 0x00, 0xF4, 0xFB, 0x2B, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xE1, 0x22, 0xAD, 0xBD, 0x9B, 0x9E, 0x71, 0x41, 0x92, 0x7C, 0xD7, 0x12, 0xDA, 0x25, 0x97, 0x41, 0x30, 0x77, 0x25, 0xC5, 0x00, 0x0B, 0x00, 0x00, 0xF4, 0xFB, 0x2E, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x2A, 0x6F, 0xBC, 0x99, 0x74, 0x1E, 0x73, 0x41, 0xDA, 0x44, 0xA3, 0x62, 0x22, 0x1E, 0x99, 0x41, 0x2F, 0x0E, 0x90, 0xC5, 0x00, 0x15, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x5C, 0x06, 0xD1, 0xD0, 0xC1, 0xBA, 0x74, 0x41, 0x9E, 0x8E, 0xDB, 0x63, 0xEE, 0xAE, 0x9B, 0x41, 0x54, 0xB4, 0x95, 0xC5, 0x06, 0x0C, 0x00, 0x06, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0x03, 0x2A, 0xC3, 0xDB, 0xA6, 0x92, 0x73, 0x41, 0x51, 0x06, 0x77, 0xCD, 0xC9, 0xB6, 0x99, 0x41, 0xF4, 0xF9, 0x4A, 0xC4, 0x00, 0x16, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0x1D, 0x70, 0x78, 0xA8, 0xC9, 0xCD, 0x74, 0x41, 0xAF, 0xC9, 0x57, 0x10, 0xCD, 0x54, 0x9B, 0x41, 0x02, 0x6B, 0xD4, 0xC5, 0x00, 0x1B, 0x00, 0x00, 0xF4, 0xFB, 0x2B, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0xA0, 0x26, 0x0D, 0x49, 0x85, 0x68, 0x75, 0x41, 0x9E, 0x02, 0xD1, 0x1C, 0x15, 0x20, 0x9C, 0x41, 0x38, 0x09, 0x0C, 0xC5, 0x00, 0x1C, 0x00, 0x00, 0xF4, 0xFB, 0x2A, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0xDF, 0xFE, 0xDD, 0x41, 0x58, 0x8D, 0x73, 0x41, 0xA8, 0x72, 0x01, 0xDB, 0xD0, 0xAF, 0x99, 0x41, 0xE5, 0x61, 0x19, 0xC5, 0x00, 0x20, 0x00, 0x00, 0xF4, 0xFB, 0x31, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0x87, 0xBC, 0xBD, 0xCA, 0xE5, 0xE5, 0x74, 0x41, 0x63, 0x50, 0xC5, 0x05, 0x86, 0xE3, 0x9B, 0x41, 0xBF, 0xB5, 0xF4, 0x43, 0x06, 0x16, 0x00, 0x04, 0xF4, 0xFB, 0x27, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x1F, 0x9F, 0x11, 0x74, 0x13, 0x25, 0x72, 0x41, 0xB8, 0xCC, 0xB5, 0xB4, 0xBF, 0x34, 0x98, 0x41, 0x00, 0xD7, 0xB9, 0xC2, 0x06, 0x06, 0x00, 0x03, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x0F, 0x00,
                                      0x9F, 0xB8, 0x3D, 0x10, 0x86, 0xC9, 0x75, 0x41, 0xF2, 0xF0, 0xFA, 0xF8, 0x24, 0x1B, 0x9D, 0x41, 0xAC, 0x9E, 0xD6, 0xC5, 0x06, 0x0B, 0x00, 0x07, 0xF4, 0xFB, 0x26, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x39, 0xDA, 0x79, 0x45, 0x6A, 0x84, 0x71, 0x41, 0x1A, 0x73, 0x87, 0x76, 0xF0, 0x68, 0x97, 0x41, 0x0E, 0x2B, 0x95, 0xC5, 0x06, 0x05, 0x00, 0x08, 0xF4, 0xFB, 0x32, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x2C, 0x7F, 0xDC, 0x41, 0x33, 0xF2, 0x74, 0x41, 0x42, 0xF5, 0xFC, 0x42, 0x95, 0x0A, 0x9C, 0x41, 0x7C, 0xCA, 0xEA, 0xC5, 0x06, 0x04, 0x00, 0x0D, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x42, 0xBF, 0x99, 0x81, 0xB1, 0x08, 0x77, 0x41, 0x48, 0x73, 0xE9, 0x73, 0xD5, 0x42, 0x9E, 0x41, 0x69, 0x37, 0x8D, 0xC5, 0x02, 0x04, 0x00, 0x00, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x54, 0x58, 0x9A, 0x6A, 0x7E, 0x03, 0x79, 0x41, 0xBB, 0x6F, 0xEC, 0xC1, 0x52, 0x6E, 0xA0, 0x41, 0x10, 0x8F, 0xBE, 0xC5, 0x02, 0x02, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x4C, 0x0B, 0xCE, 0xD1, 0xEF, 0x75, 0x82, 0x41, 0x63, 0x24, 0xED, 0xDE, 0xBD, 0x40, 0xA8, 0x41, 0x77, 0xEF, 0x52, 0xC5, 0x01, 0x7E, 0x00, 0x00, 0xF4, 0xFB, 0x21, 0x07, 0x02, 0x08, 0x0F, 0x00,
                                      0xFF, 0x81, 0x0D, 0x78, 0xBB, 0x33, 0x74, 0x41, 0x75, 0xDC, 0x0F, 0xEE, 0x68, 0x8A, 0x9A, 0x41, 0xE2, 0xDA, 0xCB, 0xC4, 0x02, 0x09, 0x00, 0x00, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xBD, 0xC4, 0xCE, 0xB9, 0x6D, 0x50, 0x75, 0x41, 0x58, 0xE7, 0xEE, 0xA0, 0x6E, 0x00, 0x9C, 0x41, 0x93, 0x96, 0xA7, 0xC5, 0x02, 0x0B, 0x00, 0x00, 0xF4, 0xFB, 0x27, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x95, 0xBE, 0xAD, 0x6D, 0x4E, 0xB0, 0x74, 0x41, 0x4F, 0xE9, 0x6E, 0xF6, 0x11, 0x2E, 0x9B, 0x41, 0xD6, 0xB1, 0x47, 0xC5, 0x02, 0x24, 0x00, 0x00, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xA0, 0x7A, 0x14, 0xDF, 0xF0, 0x83, 0x71, 0x41, 0x90, 0x88, 0x4D, 0x10, 0x9E, 0x6E, 0x97, 0x41, 0xA0, 0x24, 0xB2, 0xC4, 0x06, 0x15, 0x00, 0x0B, 0xF4, 0xFB, 0x32, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xA8, 0x39, 0xD0, 0xF5, 0xB6, 0x24, 0x72, 0x41, 0xA0, 0xCF, 0xEA, 0x75, 0x57, 0x41, 0x98, 0x41, 0x0D, 0xE8, 0xAB, 0xC5, 0x06, 0x14, 0x00, 0x09, 0xF4, 0xFB, 0x28, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xE7, 0x0F, 0x23, 0x88, 0x64, 0x98, 0x78, 0x41, 0xCE, 0xEA, 0x5B, 0x98, 0xF8, 0x27, 0xA0, 0x41, 0x40, 0x19, 0x90, 0xC2, 0x02, 0x05, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0xE9, 0x9E, 0xFA, 0x7C, 0x25, 0x49, 0x77, 0x41, 0x54, 0xE3, 0x71, 0x8B, 0x82, 0x97, 0x9E, 0x41, 0xF3, 0x88, 0x8C, 0xC5, 0x02, 0x1E, 0x00, 0x00, 0xF4, 0xFB, 0x28, 0x05, 0x01, 0x08, 0x07, 0x00,
                                      0x2F, 0x61 };
        uart::protocol::Packet packet(data, &sensor);
        auto obs = std::make_shared<NAV::UbloxObs>(packet);
        ub::decryptUbloxObs(obs);
        REQUIRE(obs->msgClass == ub::UbxClass::UBX_CLASS_RXM);
        REQUIRE(obs->msgId == ub::UbxRxmMessages::UBX_RXM_RAWX);
        REQUIRE(obs->payloadLength == 0x0390);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).rcvTow == 120685.995);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).week == 0x084C);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).leapS == 0x12);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).numMeas == 0x1C);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).recStat == 0x01);
        REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).version == 0x01);

        union uDouble
        {
            std::array<uint8_t, sizeof(double)> bytes;
            double d;
        };
        union uFloat
        {
            std::array<uint8_t, sizeof(float)> bytes;
            float f;
        };

        SECTION("Measurement 0")
        {
            uDouble prMes = { { 0xB0, 0xAE, 0x37, 0x6F, 0x88, 0xB7, 0x81, 0x41 } };
            prMes.d = uart::stoh(prMes.d, sensorEndianess);
            uDouble cpMes = { { 0x10, 0x3E, 0x8C, 0xAF, 0x98, 0x46, 0xA7, 0x41 } };
            cpMes.d = uart::stoh(cpMes.d, sensorEndianess);
            uFloat doMes = { { 0x9D, 0x74, 0x4C, 0xC5 } };
            doMes.f = uart::stoh(doMes.f, sensorEndianess);
            uint8_t gnssId = 0x01;
            uint8_t svId = 0x7B;
            uint8_t sigId = 0x00;
            uint8_t freqId = 0x00;
            uint16_t locktime = 0xFBF4;
            uint8_t cno = 0x2C;
            uint8_t prStdev = 0x05;
            uint8_t cpStdev = 0x01;
            uint8_t doStdev = 0x08;
            uint8_t trkStat = 0x07;
            uint8_t reserved2 = 0x00;

            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).prMes == prMes.d);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).cpMes == cpMes.d);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).doMes == doMes.f);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).gnssId == gnssId);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).svId == svId);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).sigId == sigId);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).freqId == freqId);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).locktime == locktime);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).cno == cno);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).prStdev == prStdev);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).cpStdev == cpStdev);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).doStdev == doStdev);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).trkStat == trkStat);
            REQUIRE(std::get<ub::UbxRxmRawx>(obs->data).data.at(0).reserved2 == reserved2);
        }
    };

    SECTION("RXM - SFRBX")
    {
        //                                                                 gnssId
        //                                                                 |     svId
        //                                                                 |     |     reserved1
        //                                                                 |     |     |     freqId
        //                                                                 |     |     |     |     numWords
        //                                                                 |     |     |     |     |     chn
        //                                                                 |     |     |     |     |     |     version
        //                                                                 |     |     |     |     |     |     |     reserved2
        //                             µ  ,  b  ,Class,  Id ,   Length  ,  |     |     |     |     |     |     |     |
        std::vector<uint8_t> data = { 0xB5, 0x62, 0x02, 0x13, 0x28, 0x00, 0x01, 0x7B, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00,
                                      0x00, 0x00, 0x60, 0x9A,
                                      0xD8, 0x7F, 0xFD, 0x03,
                                      0xBB, 0x03, 0x00, 0x00,
                                      0x10, 0x80, 0xB8, 0x97,
                                      0x80, 0xFB, 0xFD, 0x3B,
                                      0x44, 0x83, 0xB6, 0x80,
                                      0xF8, 0xAF, 0xAF, 0x3F,
                                      0xB1, 0xE6, 0xD8, 0x3B,
                                      0xA0, 0xD0 };
        uart::protocol::Packet packet(data, &sensor);
        auto obs = std::make_shared<NAV::UbloxObs>(packet);
        ub::decryptUbloxObs(obs);
        REQUIRE(obs->msgClass == ub::UbxClass::UBX_CLASS_RXM);
        REQUIRE(obs->msgId == ub::UbxRxmMessages::UBX_RXM_SFRBX);
        REQUIRE(obs->payloadLength == 0x0028);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).gnssId == 0x01);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).svId == 0x7B);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).freqId == 0x00);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).numWords == 0x08);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).chn == 0x00);
        REQUIRE(std::get<ub::UbxRxmSfrbx>(obs->data).version == 0x02);
    }
}

TEST_CASE("[UbloxUtilities] checksumUBX", "[UbloxUtilities]")
{
    Logger consoleSink;

    //                                                                 navBbrMask
    //                                                                 |           resetMode
    //                                                                 |           |     reserved1
    //                             µ  ,  b  ,Class,  Id ,   Length  ,  |           |     |
    std::vector<uint8_t> data = { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x87, 0x00, 0x00,
                                  0x94, 0xF5 };
    auto checksum = ub::checksumUBX(data);
    REQUIRE(checksum.first == data.at(data.size() - 2));
    REQUIRE(checksum.second == data.at(data.size() - 1));

    //                                            rcvToW
    //                                            |                                               week
    //                                            |                                               |           leapS
    //                                            |                                               |           |     numMeas
    //                                            |                                               |           |     |     recStat
    //                                            |                                               |           |     |     |     version
    //                                            |                                               |           |     |     |     |     reserved1
    //        µ  ,  b  ,Class,  Id ,   Length  ,  |                                               |           |     |     |     |     |
    data = { 0xB5, 0x62, 0x02, 0x15, 0x90, 0x03, 0xB8, 0x1E, 0x85, 0xEB, 0xDF, 0x76, 0xFD, 0x40, 0x4C, 0x08, 0x12, 0x1C, 0x01, 0x01, 0x33, 0x39,
             0xB0, 0xAE, 0x37, 0x6F, 0x88, 0xB7, 0x81, 0x41, 0x10, 0x3E, 0x8C, 0xAF, 0x98, 0x46, 0xA7, 0x41, 0x9D, 0x74, 0x4C, 0xC5, 0x01, 0x7B, 0x00, 0x00, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xBF, 0x52, 0xD4, 0x24, 0x9C, 0x82, 0x79, 0x41, 0xF4, 0xF9, 0x3D, 0xC1, 0xD2, 0xC1, 0xA0, 0x41, 0xB5, 0x58, 0x02, 0xC5, 0x02, 0x1B, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x2E, 0x4C, 0x68, 0x66, 0x3F, 0xA6, 0x72, 0x41, 0x80, 0xF5, 0xF2, 0xEF, 0x35, 0x80, 0x98, 0x41, 0x6C, 0xC5, 0xA4, 0xC4, 0x00, 0x01, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xAF, 0x20, 0x2D, 0x87, 0xB2, 0xAB, 0x72, 0x41, 0xE4, 0xD1, 0x20, 0xC5, 0x5E, 0x87, 0x98, 0x41, 0x2D, 0xFF, 0xA2, 0xC5, 0x00, 0x08, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x1F, 0x20, 0xB2, 0x26, 0x5E, 0x33, 0x75, 0x41, 0x94, 0x79, 0xDF, 0xB5, 0x40, 0xDA, 0x9B, 0x41, 0x80, 0x12, 0x60, 0x43, 0x00, 0x03, 0x00, 0x00, 0xF4, 0xFB, 0x29, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x23, 0xFE, 0x50, 0x89, 0xC4, 0xF9, 0x74, 0x41, 0x44, 0x7A, 0x8C, 0x73, 0x94, 0x8E, 0x9B, 0x41, 0x15, 0x4B, 0xC7, 0xC5, 0x00, 0x0A, 0x00, 0x00, 0xF4, 0xFB, 0x2B, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xE1, 0x22, 0xAD, 0xBD, 0x9B, 0x9E, 0x71, 0x41, 0x92, 0x7C, 0xD7, 0x12, 0xDA, 0x25, 0x97, 0x41, 0x30, 0x77, 0x25, 0xC5, 0x00, 0x0B, 0x00, 0x00, 0xF4, 0xFB, 0x2E, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x2A, 0x6F, 0xBC, 0x99, 0x74, 0x1E, 0x73, 0x41, 0xDA, 0x44, 0xA3, 0x62, 0x22, 0x1E, 0x99, 0x41, 0x2F, 0x0E, 0x90, 0xC5, 0x00, 0x15, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x5C, 0x06, 0xD1, 0xD0, 0xC1, 0xBA, 0x74, 0x41, 0x9E, 0x8E, 0xDB, 0x63, 0xEE, 0xAE, 0x9B, 0x41, 0x54, 0xB4, 0x95, 0xC5, 0x06, 0x0C, 0x00, 0x06, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0x03, 0x2A, 0xC3, 0xDB, 0xA6, 0x92, 0x73, 0x41, 0x51, 0x06, 0x77, 0xCD, 0xC9, 0xB6, 0x99, 0x41, 0xF4, 0xF9, 0x4A, 0xC4, 0x00, 0x16, 0x00, 0x00, 0xF4, 0xFB, 0x2F, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0x1D, 0x70, 0x78, 0xA8, 0xC9, 0xCD, 0x74, 0x41, 0xAF, 0xC9, 0x57, 0x10, 0xCD, 0x54, 0x9B, 0x41, 0x02, 0x6B, 0xD4, 0xC5, 0x00, 0x1B, 0x00, 0x00, 0xF4, 0xFB, 0x2B, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0xA0, 0x26, 0x0D, 0x49, 0x85, 0x68, 0x75, 0x41, 0x9E, 0x02, 0xD1, 0x1C, 0x15, 0x20, 0x9C, 0x41, 0x38, 0x09, 0x0C, 0xC5, 0x00, 0x1C, 0x00, 0x00, 0xF4, 0xFB, 0x2A, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0xDF, 0xFE, 0xDD, 0x41, 0x58, 0x8D, 0x73, 0x41, 0xA8, 0x72, 0x01, 0xDB, 0xD0, 0xAF, 0x99, 0x41, 0xE5, 0x61, 0x19, 0xC5, 0x00, 0x20, 0x00, 0x00, 0xF4, 0xFB, 0x31, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0x87, 0xBC, 0xBD, 0xCA, 0xE5, 0xE5, 0x74, 0x41, 0x63, 0x50, 0xC5, 0x05, 0x86, 0xE3, 0x9B, 0x41, 0xBF, 0xB5, 0xF4, 0x43, 0x06, 0x16, 0x00, 0x04, 0xF4, 0xFB, 0x27, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x1F, 0x9F, 0x11, 0x74, 0x13, 0x25, 0x72, 0x41, 0xB8, 0xCC, 0xB5, 0xB4, 0xBF, 0x34, 0x98, 0x41, 0x00, 0xD7, 0xB9, 0xC2, 0x06, 0x06, 0x00, 0x03, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x0F, 0x00,
             0x9F, 0xB8, 0x3D, 0x10, 0x86, 0xC9, 0x75, 0x41, 0xF2, 0xF0, 0xFA, 0xF8, 0x24, 0x1B, 0x9D, 0x41, 0xAC, 0x9E, 0xD6, 0xC5, 0x06, 0x0B, 0x00, 0x07, 0xF4, 0xFB, 0x26, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x39, 0xDA, 0x79, 0x45, 0x6A, 0x84, 0x71, 0x41, 0x1A, 0x73, 0x87, 0x76, 0xF0, 0x68, 0x97, 0x41, 0x0E, 0x2B, 0x95, 0xC5, 0x06, 0x05, 0x00, 0x08, 0xF4, 0xFB, 0x32, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x2C, 0x7F, 0xDC, 0x41, 0x33, 0xF2, 0x74, 0x41, 0x42, 0xF5, 0xFC, 0x42, 0x95, 0x0A, 0x9C, 0x41, 0x7C, 0xCA, 0xEA, 0xC5, 0x06, 0x04, 0x00, 0x0D, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x42, 0xBF, 0x99, 0x81, 0xB1, 0x08, 0x77, 0x41, 0x48, 0x73, 0xE9, 0x73, 0xD5, 0x42, 0x9E, 0x41, 0x69, 0x37, 0x8D, 0xC5, 0x02, 0x04, 0x00, 0x00, 0xF4, 0xFB, 0x2C, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x54, 0x58, 0x9A, 0x6A, 0x7E, 0x03, 0x79, 0x41, 0xBB, 0x6F, 0xEC, 0xC1, 0x52, 0x6E, 0xA0, 0x41, 0x10, 0x8F, 0xBE, 0xC5, 0x02, 0x02, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x4C, 0x0B, 0xCE, 0xD1, 0xEF, 0x75, 0x82, 0x41, 0x63, 0x24, 0xED, 0xDE, 0xBD, 0x40, 0xA8, 0x41, 0x77, 0xEF, 0x52, 0xC5, 0x01, 0x7E, 0x00, 0x00, 0xF4, 0xFB, 0x21, 0x07, 0x02, 0x08, 0x0F, 0x00,
             0xFF, 0x81, 0x0D, 0x78, 0xBB, 0x33, 0x74, 0x41, 0x75, 0xDC, 0x0F, 0xEE, 0x68, 0x8A, 0x9A, 0x41, 0xE2, 0xDA, 0xCB, 0xC4, 0x02, 0x09, 0x00, 0x00, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xBD, 0xC4, 0xCE, 0xB9, 0x6D, 0x50, 0x75, 0x41, 0x58, 0xE7, 0xEE, 0xA0, 0x6E, 0x00, 0x9C, 0x41, 0x93, 0x96, 0xA7, 0xC5, 0x02, 0x0B, 0x00, 0x00, 0xF4, 0xFB, 0x27, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x95, 0xBE, 0xAD, 0x6D, 0x4E, 0xB0, 0x74, 0x41, 0x4F, 0xE9, 0x6E, 0xF6, 0x11, 0x2E, 0x9B, 0x41, 0xD6, 0xB1, 0x47, 0xC5, 0x02, 0x24, 0x00, 0x00, 0xF4, 0xFB, 0x2D, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xA0, 0x7A, 0x14, 0xDF, 0xF0, 0x83, 0x71, 0x41, 0x90, 0x88, 0x4D, 0x10, 0x9E, 0x6E, 0x97, 0x41, 0xA0, 0x24, 0xB2, 0xC4, 0x06, 0x15, 0x00, 0x0B, 0xF4, 0xFB, 0x32, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xA8, 0x39, 0xD0, 0xF5, 0xB6, 0x24, 0x72, 0x41, 0xA0, 0xCF, 0xEA, 0x75, 0x57, 0x41, 0x98, 0x41, 0x0D, 0xE8, 0xAB, 0xC5, 0x06, 0x14, 0x00, 0x09, 0xF4, 0xFB, 0x28, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xE7, 0x0F, 0x23, 0x88, 0x64, 0x98, 0x78, 0x41, 0xCE, 0xEA, 0x5B, 0x98, 0xF8, 0x27, 0xA0, 0x41, 0x40, 0x19, 0x90, 0xC2, 0x02, 0x05, 0x00, 0x00, 0xF4, 0xFB, 0x25, 0x05, 0x01, 0x08, 0x07, 0x00,
             0xE9, 0x9E, 0xFA, 0x7C, 0x25, 0x49, 0x77, 0x41, 0x54, 0xE3, 0x71, 0x8B, 0x82, 0x97, 0x9E, 0x41, 0xF3, 0x88, 0x8C, 0xC5, 0x02, 0x1E, 0x00, 0x00, 0xF4, 0xFB, 0x28, 0x05, 0x01, 0x08, 0x07, 0x00,
             0x2F, 0x61 };
    checksum = ub::checksumUBX(data);
    REQUIRE(checksum.first == data.at(data.size() - 2));
    REQUIRE(checksum.second == data.at(data.size() - 1));

    //                                            gnssId
    //                                            |     svId
    //                                            |     |     reserved1
    //                                            |     |     |     freqId
    //                                            |     |     |     |     numWords
    //                                            |     |     |     |     |     chn
    //                                            |     |     |     |     |     |     version
    //                                            |     |     |     |     |     |     |     reserved2
    //        µ  ,  b  ,Class,  Id ,   Length  ,  |     |     |     |     |     |     |     |
    data = { 0xB5, 0x62, 0x02, 0x13, 0x28, 0x00, 0x01, 0x7B, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00,
             0x00, 0x00, 0x60, 0x9A,
             0xD8, 0x7F, 0xFD, 0x03,
             0xBB, 0x03, 0x00, 0x00,
             0x10, 0x80, 0xB8, 0x97,
             0x80, 0xFB, 0xFD, 0x3B,
             0x44, 0x83, 0xB6, 0x80,
             0xF8, 0xAF, 0xAF, 0x3F,
             0xB1, 0xE6, 0xD8, 0x3B,
             0xA0, 0xD0 };
    checksum = ub::checksumUBX(data);
    REQUIRE(checksum.first == data.at(data.size() - 2));
    REQUIRE(checksum.second == data.at(data.size() - 1));
}

TEST_CASE("[UbloxUtilities] checksumNMEA", "[UbloxUtilities]")
{
    Logger consoleSink;

    std::string text = "$GPZDA,141644.00,22,03,2002,00,00*67\r\n";
    std::vector<uint8_t> data;
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x67);

    text = "$GNGGA,093318.00,4846.84211,N,00910.29515,E,2,12,0.56,279.0,M,47.6,M,,0000*4A\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x4A);

    text = "$GNTXT,01,01,02,u-blox AG - www.u-blox.com*4E\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x4E);

    text = "$GNTXT,01,01,02,EXT CORE 3.01 (1ec93f)*67\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x67);

    text = "$GNTXT,01,01,02,MOD=NEO-M8U-0*7C\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x7C);

    text = "$GNTXT,01,01,02,GNSS OTP=GPS;GLO*37\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x37);

    text = "$GNTXT,01,01,02,PF=82*72\r\n";
    data.clear();
    std::copy(text.begin(), text.end(), std::back_inserter(data));
    REQUIRE(ub::checksumNMEA(data) == 0x72);
}