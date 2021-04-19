/// @file SkydelImuStream.hpp
/// @brief
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-04-19

#pragma once

#include "Nodes/DataProvider/IMU/Imu.hpp"

namespace NAV
{
/// SkydelImuStream Sensor Class
class SkydelImuStream : public Imu
{
  public:
    /// @brief Default constructor
    SkydelImuStream();
    /// @brief Destructor
    ~SkydelImuStream() override;
    /// @brief Copy constructor
    SkydelImuStream(const SkydelImuStream&) = delete;
    /// @brief Move constructor
    SkydelImuStream(SkydelImuStream&&) = delete;
    /// @brief Copy assignment operator
    SkydelImuStream& operator=(const SkydelImuStream&) = delete;
    /// @brief Move assignment operator
    SkydelImuStream& operator=(SkydelImuStream&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

    /// @brief ImGui config window which is shown on double click
    /// @attention Don't forget to set hasConfig to true in the constructor of the node
    void guiConfig() override;

    /// @brief Saves the node into a json object
    [[nodiscard]] json save() const override;

    /// @brief Restores the node from a json object
    /// @param[in] j Json object with the node state
    void restore(const json& j) override;

    /// @brief Resets the node. It is guaranteed that the node is initialized when this is called.
    bool resetNode() override;

  private:
    constexpr static size_t OutputPortIndex_ImuObs = 1; ///< @brief Flow (ImuObs)

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// @brief Function which performs the async data reading
    /// @param[in, out] userData Pointer to the SkydelImuStream object
    // static void readImuThread(void* userData);
};

} // namespace NAV