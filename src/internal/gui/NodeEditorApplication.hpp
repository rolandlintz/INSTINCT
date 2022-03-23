/// @file NodeEditorApplication.hpp
/// @brief GUI callbacks
/// @author T. Topp (thomas@topp.cc)
/// @date 2020-12-14

#pragma once

#include <application.h>

#include <imgui.h>
#include <implot.h>

#include "internal/gui/GlobalActions.hpp"

#include <thread>
#include <deque>

namespace NAV
{
class Node;
class Pin;

namespace gui
{
/// @brief Application class providing all relevant GUI callbacks
class NodeEditorApplication : public Application
{
  public:
    /// @brief Default constructor
    NodeEditorApplication() = delete;
    /// @brief Destructor
    ~NodeEditorApplication() override;
    /// @brief Copy constructor
    NodeEditorApplication(const NodeEditorApplication&) = delete;
    /// @brief Move constructor
    NodeEditorApplication(NodeEditorApplication&&) = delete;
    /// @brief Copy assignment operator
    NodeEditorApplication& operator=(const NodeEditorApplication&) = delete;
    /// @brief Move assignment operator
    NodeEditorApplication& operator=(NodeEditorApplication&&) = delete;

    /// Bring the Application Constructors into this class (NOLINTNEXTLINE)
    using Application::Application;

    // static Node* SpawnInputActionNode();

    // static Node* SpawnLessNode();

    // static Node* SpawnGroupBox();

    /// @brief Called when the application is started
    void OnStart() override;

    /// @brief Called when the application is stopped
    void OnStop() override;

    /// @brief Called when the user request the application to close
    ///
    /// Checks whether there are unsaved changes and then prompts the user to save them before quit
    /// @return True if no unsaved changes
    bool OnQuitRequest() override;

    /// @brief Called on every frame
    /// @param[in] deltaTime Time since last frame
    void OnFrame(float deltaTime) override;

    /// @brief Shows a PopupModal asking the user if he wants to quit with unsaved changes
    void ShowQuitRequested();
    /// @brief Shows a PopupModel where the user can select a path to save his flow to
    void ShowSaveAsRequested();
    /// @brief Shows a PopupModel to clear the current flow
    void ShowClearNodesRequested();
    /// @brief Shows a PopupModel loading a new flow
    void ShowLoadRequested();
    /// @brief Shows a PopupModal where the user can rename the node
    /// @param[in, out] renameNode Pointer to the node to rename. Pointer gets nulled when finished.
    static void ShowRenameNodeRequest(Node*& renameNode);
    /// @brief Shows a PopupModal where the user can rename the pin
    /// @param[in, out] renamePin Pointer to the pin to rename. Pointer gets nulled when finished.
    static void ShowRenamePinRequest(Pin*& renamePin);

    /// @brief Frame counter to block the navigate to content function till nodes are correctly loaded
    int frameCountNavigate = 0;

    /// @brief Pointer to the texture for the instinct logo
    static inline ImTextureID m_InstinctLogo = nullptr;

    /// @brief Default style of the ImPlot library to compare changes against
    static inline ImPlotStyle imPlotReferenceStyle;

    inline static float leftPaneWidth = 350.0F;       ///< Width of the left pane
    inline static float rightPaneWidth = 850.0F;      ///< Width of the right pane
    inline static float menuBarHeight = 20;           ///< Height of the menu bar on top
    constexpr static float SPLITTER_THICKNESS = 4.0F; ///< Thickness of the splitter between left and right pane

  private:
    /// @brief Pointer to the texture for the node headers
    ImTextureID m_HeaderBackground = nullptr;

    /// @brief Global action to execute
    GlobalActions globalAction = GlobalActions::None; // TODO: Move to the GlobalAction.cpp as a global variable

    /// @brief Thread which initializes nodes asynchronously
    std::thread initThread;
    // bool initThread_stopRequested = false;

    /// @brief Id of the node currently initialized
    size_t currentInitNodeId = 0;
    /// List of Node* & flag (init=true, deinit=false)
    std::deque<std::pair<Node*, bool>> initList;
};

} // namespace gui
} // namespace NAV