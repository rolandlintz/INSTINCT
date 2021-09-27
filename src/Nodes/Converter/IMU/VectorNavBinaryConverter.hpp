/// @file VectorNavBinaryConverter.hpp
/// @brief Converts VectorNavBinaryOutput
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-07-09

#pragma once

#include "internal/Node/Node.hpp"

#include "NodeData/IMU/VectorNavBinaryOutput.hpp"
#include "NodeData/IMU/ImuObsWDelta.hpp"
#include "NodeData/State/PosVelAtt.hpp"

#include <array>
#include <memory>

namespace NAV
{
/// Converts VectorNavBinaryOutput
class VectorNavBinaryConverter : public Node
{
  public:
    /// @brief Default constructor
    VectorNavBinaryConverter();
    /// @brief Destructor
    ~VectorNavBinaryConverter() override;
    /// @brief Copy constructor
    VectorNavBinaryConverter(const VectorNavBinaryConverter&) = delete;
    /// @brief Move constructor
    VectorNavBinaryConverter(VectorNavBinaryConverter&&) = delete;
    /// @brief Copy assignment operator
    VectorNavBinaryConverter& operator=(const VectorNavBinaryConverter&) = delete;
    /// @brief Move assignment operator
    VectorNavBinaryConverter& operator=(VectorNavBinaryConverter&&) = delete;

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

  private:
    constexpr static size_t OutputPortIndex_Converted = 0;            ///< @brief Flow
    constexpr static size_t InputPortIndex_VectorNavBinaryOutput = 0; ///< @brief Flow (VectorNavBinaryOutput)

    /// Enum specifying the type of the output message
    enum OutputType
    {
        OutputType_ImuObsWDelta, ///< Extract ImuObsWDelta data
        OutputType_PosVelAtt,    ///< Extract PosVelAtt data
    };

    /// The selected output type in the GUI
    OutputType outputType = OutputType_ImuObsWDelta;

    /// Enum specifying the source for the PosVelAtt conversion
    enum PosVelSource
    {
        PosVelSource_Best,  ///< INS > GNSS1 > GNSS2
        PosVelSource_Ins,   ///< Take only INS values into account
        PosVelSource_Gnss1, ///< Take only GNSS1 values into account
        PosVelSource_Gnss2, ///< Take only GNSS2 values into account
    };

    /// The selected PosVel source in the GUI
    PosVelSource posVelSource = PosVelSource_Best;

    /// GUI option. If checked forces position to a static value and velocity to 0
    bool forceStatic = false;

    /// Tracker for the initial position in case of static data
    Eigen::Vector3d initPosition = Eigen::Vector3d::Zero();

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Converts the VectorNavBinaryOutput observation to the selected message type
    /// @param[in] nodeData VectorNavBinaryOutput to process
    /// @param[in] linkId Id of the link over which the data is received
    void receiveObs(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId linkId);

    /// @brief Converts the VectorNavBinaryOutput to a ImuObsWDelta observation
    /// @param[in] nodeData VectorNavBinaryOutput to process
    /// @return The converted data
    std::shared_ptr<const ImuObsWDelta> convert2ImuObsWDelta(const std::shared_ptr<const VectorNavBinaryOutput>& vnObs);

    /// @brief Converts the VectorNavBinaryOutput to a PosVelAtt observation
    /// @param[in] nodeData VectorNavBinaryOutput to process
    /// @return The converted data
    std::shared_ptr<const PosVelAtt> convert2PosVelAtt(const std::shared_ptr<const VectorNavBinaryOutput>& vnObs);
};

} // namespace NAV
