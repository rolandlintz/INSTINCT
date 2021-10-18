/// @file Demo.hpp
/// @brief Demo Node which demonstrates all capabilities
/// @author T. Topp (topp@ins.uni-stuttgart.de)
/// @date 2021-01-13

#pragma once

#include "internal/Node/Node.hpp"

#include "util/Eigen.hpp"
#include <array>
#include "util/CallbackTimer.hpp"

namespace NAV
{
/// @brief Demonstrates the basic GUI functionality of nodes
class Demo : public Node
{
  public:
    /// @brief Default constructor
    Demo();
    /// @brief Destructor
    ~Demo() override;
    /// @brief Copy constructor
    Demo(const Demo&) = delete;
    /// @brief Move constructor
    Demo(Demo&&) = delete;
    /// @brief Copy assignment operator
    Demo& operator=(const Demo&) = delete;
    /// @brief Move assignment operator
    Demo& operator=(Demo&&) = delete;

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

    /// @brief Called when a new link is to be established
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    /// @return True if link is allowed, false if link is rejected
    bool onCreateLink(Pin* startPin, Pin* endPin) override;

    /// @brief Called when a link is to be deleted
    /// @param[in] startPin Pin where the link starts
    /// @param[in] endPin Pin where the link ends
    void onDeleteLink(Pin* startPin, Pin* endPin) override;

    /// @brief Data struct transmitted over an output port
    struct DemoData
    {
        std::array<int, 3> integer = { 12, -2, 2 }; ///< Integer inside the DemoData
        bool boolean = false;                       ///< Boolean inside the DemoData
    };

  private:
    constexpr static size_t OutputPortIndex_NodeData = 1; ///< @brief Flow (NodeData)
    constexpr static size_t OutputPortIndex_InsObs = 2;   ///< @brief Flow (InsObs)
    constexpr static size_t OutputPortIndex_Bool = 3;     ///< @brief Bool
    constexpr static size_t OutputPortIndex_Int = 4;      ///< @brief Int
    constexpr static size_t OutputPortIndex_Float = 5;    ///< @brief Float
    constexpr static size_t OutputPortIndex_Double = 6;   ///< @brief Double
    constexpr static size_t OutputPortIndex_String = 7;   ///< @brief String
    constexpr static size_t OutputPortIndex_DemoData = 8; ///< @brief DemoData
    constexpr static size_t OutputPortIndex_Matrix = 9;   ///< @brief Matrix
    constexpr static size_t InputPortIndex_DemoNode = 0;  ///< @brief Delegate (Demo)
    constexpr static size_t InputPortIndex_Bool = 3;      ///< @brief Bool
    constexpr static size_t InputPortIndex_Int = 4;       ///< @brief Int
    constexpr static size_t InputPortIndex_Float = 5;     ///< @brief Float
    constexpr static size_t InputPortIndex_Double = 6;    ///< @brief Double
    constexpr static size_t InputPortIndex_String = 7;    ///< @brief String
    constexpr static size_t InputPortIndex_DemoData = 8;  ///< @brief DemoData
    constexpr static size_t InputPortIndex_Matrix = 9;    ///< @brief Matrix

    /// @brief Initialize the node
    bool initialize() override;

    /// @brief Deinitialize the node
    void deinitialize() override;

    /// @brief Receive Sensor Data
    /// @param[in] nodeData Data to plot
    /// @param[in] linkId Id of the link over which the data is received
    void receiveSensorData(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId linkId);

    /// @brief Receive File Reader Data
    /// @param[in] nodeData Data to plot
    /// @param[in] linkId Id of the link over which the data is received
    void receiveFileReaderData(const std::shared_ptr<const NodeData>& nodeData, ax::NodeEditor::LinkId linkId);

    /// @brief Polls data from the file
    /// @param[in] peek Specifies if the data should be peeked (without moving the read cursor) or read
    /// @return The read observation
    [[nodiscard]] std::shared_ptr<const NodeData> pollData(bool peek = false);

    /// Timer object to handle async data requests
    CallbackTimer timer;

    /// @brief Function which performs the async data reading
    /// @param[in] userData Pointer to the object
    static void readSensorDataThread(void* userData);

    /// @brief Output frequency for the simulated sensor data
    int outputFrequency = 1;
    /// @brief Counter how often sensor data was received
    int receivedDataFromSensorCnt = 0;
    /// @brief Counter how often file reader data was received
    int receivedDataFromFileReaderCnt = 0;

    /// Counter for data Reading
    int iPollData = 0;
    /// Amount of Observations to be read
    int nPollData = 20;

    bool valueBool = true;                                         ///< Value which is represented over the Bool pin
    int valueInt = -1;                                             ///< Value which is represented over the Int pin
    float valueFloat = 65.4F;                                      ///< Value which is represented over the Float pin
    double valueDouble = 1242.342;                                 ///< Value which is represented over the Double pin
    std::string valueString = "Demo";                              ///< Value which is represented over the String pin
    DemoData valueObject;                                          ///< Value which is represented over the Object pin
    Eigen::MatrixXd valueMatrix = Eigen::MatrixXd::Identity(3, 3); ///< Value which is represented over the Matrix pin
    size_t stringUpdateCounter = 0;                                ///< Counter of how often the string was updated

    /// @brief Function to call when the string is updated
    /// @param[in] linkId Id of the Link where the string is connected over
    void stringUpdatedNotifyFunction(ax::NodeEditor::LinkId linkId);
};

} // namespace NAV
