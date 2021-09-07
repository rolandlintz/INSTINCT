/// @file Combiner.hpp
/// @brief Combiner Node
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-02-01

#pragma once

#include "internal/Node/Node.hpp"

#include "util/Eigen.hpp"

namespace NAV
{
class Combiner : public Node
{
  public:
    /// @brief Default constructor
    Combiner();
    /// @brief Destructor
    ~Combiner() override;
    /// @brief Copy constructor
    Combiner(const Combiner&) = delete;
    /// @brief Move constructor
    Combiner(Combiner&&) = delete;
    /// @brief Copy assignment operator
    Combiner& operator=(const Combiner&) = delete;
    /// @brief Move assignment operator
    Combiner& operator=(Combiner&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

    /// @brief Called when a new link is to be established
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    /// @return True if link is allowed, false if link is rejected
    bool onCreateLink(Pin* startPin, Pin* endPin) override;

    /// @brief Called when a link was deleted
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    void afterDeleteLink(Pin* startPin, Pin* endPin) override;

  private:
    constexpr static size_t OutputPortIndex_Flow = 0;       ///< @brief Flow
    constexpr static size_t InputPortIndex_Flow_First = 0;  ///< @brief Flow
    constexpr static size_t InputPortIndex_Flow_Second = 1; ///< @brief Flow

    /// @brief Set the Pin Identifiers for the other pin depending on the connected pin
    /// @param[in] connectedPinIndex The connected pin
    /// @param[in] otherPinIndex The unconnected pin which needs the identifiers to be set
    /// @param[in] dataIdentifiers The data Identifier to be considered
    void setPinIdentifiers(size_t connectedPinIndex, size_t otherPinIndex, const std::vector<std::string>& dataIdentifiers);

    void updateOutputPin(const std::vector<std::string>& oldDataIdentifiers);

    /// @brief Receive data
    /// @param[in] nodeData Observation received
    /// @param[in] linkId Id of the link over which the data is received
    void receiveData(const std::shared_ptr<NodeData>& nodeData, ax::NodeEditor::LinkId linkId);
};

} // namespace NAV
