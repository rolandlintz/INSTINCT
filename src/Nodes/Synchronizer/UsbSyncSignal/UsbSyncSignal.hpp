/**
 * @file UsbSyncSignal.hpp
 * @brief Class to send Sync Signals over the USB RTS Pin
 * @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
 * @date 2020-03-20
 */

#pragma once

#include <string>

#include "Nodes/Node.hpp"

#include <sys/ioctl.h> //ioctl

namespace NAV
{
/// Class to send Sync Signals over the USB RTS Pin
class UsbSyncSignal : public Node
{
  public:
    UsbSyncSignal(const UsbSyncSignal&) = delete;            ///< Copy constructor
    UsbSyncSignal(UsbSyncSignal&&) = delete;                 ///< Move constructor
    UsbSyncSignal& operator=(const UsbSyncSignal&) = delete; ///< Copy assignment operator
    UsbSyncSignal& operator=(UsbSyncSignal&&) = delete;      ///< Move assignment operator

  protected:
    /**
     * @brief Construct a new Usb Sync Signal object
     * 
     * @param[in] name Name of the Object
     * @param[in, out] options Program options string list
     */
    UsbSyncSignal(std::string name, std::deque<std::string>& options);

    /// Default destructor
    ~UsbSyncSignal() override;

    /// File descriptor of the sensor port
    int fd;

    /// COM port where to send the sync to
    std::string port;

    /// Flag for the USB Pin to trigger
    int pinFlag = TIOCM_RTS;
};

} // namespace NAV
