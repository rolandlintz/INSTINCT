/**
 * @file UartSensor.hpp
 * @brief Abstract Uart Sensor Class
 * @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
 * @date 2020-03-11
 */

#pragma once

#include <string>
#include <deque>

namespace NAV
{
/// Abstract Uart Sensor Class
class UartSensor
{
  public:
    /// Available Baudrates
    enum Baudrate
    {
        BAUDRATE_FASTEST = 0,     ///< Fastest possible Baudrate will be automatically chosen
        BAUDRATE_9600 = 9600,     ///< Baudrate with   9600 symbols per second [Baud]
        BAUDRATE_19200 = 19200,   ///< Baudrate with  19200 symbols per second [Baud]
        BAUDRATE_38400 = 38400,   ///< Baudrate with  38400 symbols per second [Baud]
        BAUDRATE_57600 = 57600,   ///< Baudrate with  57600 symbols per second [Baud]
        BAUDRATE_115200 = 115200, ///< Baudrate with 115200 symbols per second [Baud]
        BAUDRATE_128000 = 128000, ///< Baudrate with 128000 symbols per second [Baud]
        BAUDRATE_230400 = 230400, ///< Baudrate with 230400 symbols per second [Baud]
        BAUDRATE_460800 = 460800, ///< Baudrate with 460800 symbols per second [Baud]
        BAUDRATE_921600 = 921600  ///< Baudrate with 921600 symbols per second [Baud]
    };

    UartSensor(const UartSensor&) = delete;            ///< Copy constructor
    UartSensor(UartSensor&&) = delete;                 ///< Move constructor
    UartSensor& operator=(const UartSensor&) = delete; ///< Copy assignment operator
    UartSensor& operator=(UartSensor&&) = delete;      ///< Move assignment operator

  protected:
    /**
     * @brief Construct a new Uart Sensor object
     * 
     * @param[in, out] options Program options string list
     */
    explicit UartSensor(std::deque<std::string>& options);

    UartSensor() = default;          ///< Default constructor
    virtual ~UartSensor() = default; ///< Destructor

    /// COM port where the sensor is attached to
    ///
    /// - "COM1" (Windows format for physical and virtual (USB) serial port)
    /// - "/dev/ttyS1" (Linux format for physical serial port)
    /// - "/dev/ttyUSB0" (Linux format for virtual (USB) serial port)
    /// - "/dev/tty.usbserial-FTXXXXXX" (Mac OS X format for virtual (USB) serial port)
    /// - "/dev/ttyS0" (CYGWIN format. Usually the Windows COM port number minus 1. This would connect to COM1)
    std::string sensorPort;

    /// Baudrate for the sensor
    Baudrate sensorBaudrate = BAUDRATE_FASTEST;
};

} // namespace NAV