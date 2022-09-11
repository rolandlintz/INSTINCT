/// @file RtklibPosConverter.hpp
/// @brief Convert RTKLib pos files into PosVel
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2022-06-27

#pragma once

#include "internal/Node/Node.hpp"

namespace NAV
{
/// Convert RTKLib pos files into PosVel
class RtklibPosConverter : public Node
{
  public:
    /// @brief Default constructor
    RtklibPosConverter();
    /// @brief Destructor
    ~RtklibPosConverter() override;
    /// @brief Copy constructor
    RtklibPosConverter(const RtklibPosConverter&) = delete;
    /// @brief Move constructor
    RtklibPosConverter(RtklibPosConverter&&) = delete;
    /// @brief Copy assignment operator
    RtklibPosConverter& operator=(const RtklibPosConverter&) = delete;
    /// @brief Move assignment operator
    RtklibPosConverter& operator=(RtklibPosConverter&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

  private:
    constexpr static size_t OUTPUT_PORT_INDEX_POSVEL = 0;    ///< @brief Flow
    constexpr static size_t INPUT_PORT_INDEX_RTKLIB_POS = 0; ///< @brief Flow

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Converts the RtklibPosObs into PosVel
    /// @param[in] queue Queue with all the received data messages
    /// @param[in] pinIdx Index of the pin the data is received on
    void receiveObs(InputPin::NodeDataQueue& queue, size_t pinIdx);
};

} // namespace NAV
