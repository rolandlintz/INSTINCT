/// @file ImuError.hpp
/// @brief Adds errors (biases and noise) to IMU measurements
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-12-21

#pragma once

#include "internal/Node/Node.hpp"

#include "NodeData/IMU/ImuObs.hpp"

namespace NAV
{
/// Adds errors (biases and noise) to IMU measurements
class ImuError : public Node
{
  public:
    /// @brief Default constructor
    ImuError();
    /// @brief Destructor
    ~ImuError() override;
    /// @brief Copy constructor
    ImuError(const ImuError&) = delete;
    /// @brief Move constructor
    ImuError(ImuError&&) = delete;
    /// @brief Copy assignment operator
    ImuError& operator=(const ImuError&) = delete;
    /// @brief Move assignment operator
    ImuError& operator=(ImuError&&) = delete;

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
    constexpr static size_t OutputPortIndex_ImuObs = 0; ///< @brief Flow (ImuObs)
    constexpr static size_t InputPortIndex_ImuObs = 0;  ///< @brief Flow (ImuObs)

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Add errors to the ImuObs observation
    /// @param[in] nodeData ImuObs to process
    /// @param[in] linkId Id of the link over which the data is received
    void receiveObs(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId linkId);
};

} // namespace NAV
