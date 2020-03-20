/**
 * @file UbloxDataLogger.hpp
 * @brief Data Logger for Ublox observations
 * @author T. Topp (thomas.topp@nav.uni-stuttgart.de)
 * @date 2020-03-17
 */

#pragma once

#include "../DataLogger.hpp"

namespace NAV
{
/// Data Logger for Ublox observations
class UbloxDataLogger : public DataLogger
{
  public:
    /**
     * @brief Construct a new Data Logger object
     * 
     * @param[in] name Name of the Logger
     * @param[in] path Path to the log file
     * @param[in] isBinary Flag if the logfile is a binary file
     */
    UbloxDataLogger(std::string name, std::string path, bool isBinary);

    /// Default destructor
    virtual ~UbloxDataLogger();

    /**
     * @brief Initialize the File
     * 
     * @retval NavStatus Indicates whether initialization was successfull
     */
    NavStatus initialize();

    /**
     * @brief Deinitialize the file
     * 
     * @retval NavStatus Indicates whether deinitialization was successfull
     */
    NavStatus deinitialize();

    /**
     * @brief Write Ublox Observation to the file
     * 
     * @param[in] obs The received observation
     * @param[in, out] userData User data specified when registering the callback
     * @retval NavStatus Indicates whether the write was successfull.
     */
    static NavStatus writeObservation(std::shared_ptr<void> obs, std::shared_ptr<void> userData);
};

} // namespace NAV
