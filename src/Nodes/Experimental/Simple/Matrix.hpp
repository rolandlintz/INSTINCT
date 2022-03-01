/// @file Matrix.hpp
/// @brief Provides a Matrix Object
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-01-25

#pragma once

#include "internal/Node/Node.hpp"

#include "util/Eigen.hpp"

namespace NAV::experimental
{
/// @brief Provides a Matrix Object
class Matrix : public Node
{
  public:
    /// @brief Default constructor
    Matrix();
    /// @brief Destructor
    ~Matrix() override;
    /// @brief Copy constructor
    Matrix(const Matrix&) = delete;
    /// @brief Move constructor
    Matrix(Matrix&&) = delete;
    /// @brief Copy assignment operator
    Matrix& operator=(const Matrix&) = delete;
    /// @brief Move assignment operator
    Matrix& operator=(Matrix&&) = delete;

    /// @brief String representation of the Class Type
    [[nodiscard]] static std::string typeStatic();

    /// @brief String representation of the Class Type
    [[nodiscard]] std::string type() const override;

    /// @brief String representation of the Class Category
    [[nodiscard]] static std::string category();

    /// @brief ImGui config window which is shown on double click
    /// @attention Don't forget to set _hasConfig to true in the constructor of the node
    void guiConfig() override;

    /// @brief Saves the node into a json object
    [[nodiscard]] json save() const override;

    /// @brief Restores the node from a json object
    /// @param[in] j Json object with the node state
    void restore(const json& j) override;

    /// @brief Called when a new link is to be established
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    /// @return True if link is allowed, false if link is rejected
    bool onCreateLink(Pin* startPin, Pin* endPin) override;

    /// @brief Called when a link is to be deleted
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    void onDeleteLink(Pin* startPin, Pin* endPin) override;

    /// @brief Notifies the node, that some data was changed on one of it's output ports
    /// @param[in] linkId Id of the link on which data is changed
    void notifyOnOutputValueChanged(ax::NodeEditor::LinkId linkId) override;

  private:
    constexpr static size_t OUTPUT_PORT_INDEX_FULL_MATRIX = 0; ///< @brief Matrix

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// @brief Adds/Deletes Output Pins depending on the variable nBlocks
    void updateNumberOfOutputPins();

    /// Number of Rows
    int _nRows = 3;
    /// Number of Columns
    int _nCols = 3;
    /// Number of subblocks of the matrix
    int _nBlocks = 0;

    /// List of subblocks
    std::vector<BlockMatrix> _blocks;

    /// The matrix object
    Eigen::MatrixXd _matrix;

    /// The initial matrix set in the gui
    Eigen::MatrixXd _initMatrix;
};

} // namespace NAV::experimental
