/// @file KvhDataLogger.hpp
/// @brief Data Logger for Kvh observations
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2020-06-30

#pragma once

#include "internal/Node/Node.hpp"
#include "Nodes/DataLogger/Protocol/FileWriter.hpp"

namespace NAV
{
class NodeData;

/// Data Logger for KVH observations
class KvhDataLogger : public Node, public FileWriter
{
  public:
    /// @brief Default constructor
    KvhDataLogger();
    /// @brief Destructor
    ~KvhDataLogger() override;
    /// @brief Copy constructor
    KvhDataLogger(const KvhDataLogger&) = delete;
    /// @brief Move constructor
    KvhDataLogger(KvhDataLogger&&) = delete;
    /// @brief Copy assignment operator
    KvhDataLogger& operator=(const KvhDataLogger&) = delete;
    /// @brief Move assignment operator
    KvhDataLogger& operator=(KvhDataLogger&&) = delete;

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

    /// @brief Function called by the flow executer after finishing to flush out remaining data
    void flush() override;

  private:
    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// @brief Write Observation to the file
    /// @param[in] nodeData The received observation
    /// @param[in] linkId Id of the link over which the data is received
    void writeObservation(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId linkId);
};

} // namespace NAV
