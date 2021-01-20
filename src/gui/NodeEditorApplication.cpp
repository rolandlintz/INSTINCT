#include "NodeEditorApplication.hpp"

#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include "ImGuiFileDialog.h"

#include "implot.h"

#include "gui/Shortcuts.hpp"
#include "gui/TouchTracker.hpp"

#include "gui/panels/BlueprintNodeBuilder.hpp"
namespace util = ax::NodeEditor::Utilities;

#include "gui/panels/LeftPane.hpp"

#include "gui/menus/MainMenuBar.hpp"

#include "gui/widgets/Splitter.hpp"
#include "gui/widgets/HelpMarker.hpp"
#include "gui/widgets/Spinner.hpp"

#include "internal/Pin.hpp"
#include "Nodes/Node.hpp"
#include "internal/Link.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "NodeRegistry.hpp"

#include "internal/FlowManager.hpp"
#include "internal/FlowExecutor.hpp"

#include <string>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <filesystem>

ax::NodeEditor::EditorContext* m_Editor = nullptr;

void NAV::gui::NodeEditorApplication::OnStart()
{
    LOG_TRACE("called");

    ed::Config config;

    // config.SettingsFile = "NavSoS.json";
    // config.UserPointer = this;

    // Stops the Editor from creating a log file, as we do it ourselves
    // clang-format off
    config.SaveSettings = [](const char* /*data*/, size_t /*size*/,
                             ed::SaveReasonFlags /*reason*/, void* /*userPointer*/) -> bool {
        // clang-format on

        return false;
    };

    // Trigger the changed bar on the node overview list when a node is moved
    // clang-format off
    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* /*data*/, size_t /*size*/,
                                 ed::SaveReasonFlags /*reason*/, void* /*userPointer*/) -> bool {
        // clang-format on

        // auto* self = static_cast<NodeEditorApplication*>(userPointer);

        auto* node = nm::FindNode(nodeId);
        if (!node)
        {
            return false;
        }
        flow::ApplyChanges();
        gui::TouchNode(nodeId);

        return true;
    };

    m_Editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Editor);

    ImGui::GetStyle().FrameRounding = 4.0F;
    ed::GetStyle().FlowDuration = 1.0F;

    m_HeaderBackground = LoadTexture("resources/images/BlueprintBackground.png");

    flow::SetProgramRootPath(std::filesystem::current_path());
}

void NAV::gui::NodeEditorApplication::OnStop()
{
    LOG_TRACE("called");

    FlowExecutor::stop();

    initList.clear();
    nm::Stop();

    nm::DeleteAllNodes();

    auto releaseTexture = [this](ImTextureID& id) {
        if (id)
        {
            DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(m_HeaderBackground);

    if (m_Editor)
    {
        ed::DestroyEditor(m_Editor);
        m_Editor = nullptr;
    }
}

bool NAV::gui::NodeEditorApplication::OnQuitRequest()
{
    LOG_TRACE("called");

    if (flow::HasUnsavedChanges())
    {
        globalAction = GlobalActions::QuitUnsaved;
        return false;
    }

    return true;
}

void NAV::gui::NodeEditorApplication::ShowQuitRequested()
{
    ax::NodeEditor::EnableShortcuts(false);

    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper)
    {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            return;
        }
    }

    ImGui::OpenPopup("Quit with unsaved changes?");
    if (ImGui::BeginPopupModal("Quit with unsaved changes?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you want to save your changes or discard them?");
        if (ImGui::Button("Save"))
        {
            if (flow::GetCurrentFilename().empty())
            {
                if (std::string targetPath = flow::GetProgramRootPath() + "/flow";
                    std::filesystem::current_path() != targetPath && std::filesystem::exists(targetPath))
                {
                    LOG_DEBUG("Changing current path to: {}", std::filesystem::current_path().string());
                    std::filesystem::current_path(targetPath);
                }
                igfd::ImGuiFileDialog::Instance()->OpenModal("Save Flow", "Save Flow", ".flow", "", 1, nullptr, ImGuiFileDialogFlags_ConfirmOverwrite);
                igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".flow", ImVec4(0.0F, 1.0F, 0.0F, 0.9F));
            }
            else
            {
                flow::SaveFlowAs(flow::GetCurrentFilename());
                globalAction = GlobalActions::None;
                flow::DiscardChanges();
                Quit();
                ImGui::CloseCurrentPopup();
            }
        }

        if (igfd::ImGuiFileDialog::Instance()->FileDialog("Save Flow"))
        {
            if (igfd::ImGuiFileDialog::Instance()->IsOk)
            {
                flow::SetCurrentFilename(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
                flow::SaveFlowAs(flow::GetCurrentFilename());
            }

            globalAction = GlobalActions::None;
            igfd::ImGuiFileDialog::Instance()->CloseDialog();
            Quit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard"))
        {
            globalAction = GlobalActions::None;
            flow::DiscardChanges();
            Quit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void NAV::gui::NodeEditorApplication::ShowSaveAsRequested()
{
    ax::NodeEditor::EnableShortcuts(false);

    if (std::string targetPath = flow::GetProgramRootPath() + "/flow";
        std::filesystem::current_path() != targetPath && std::filesystem::exists(targetPath))
    {
        LOG_DEBUG("Changing current path to: {}", std::filesystem::current_path().string());
        std::filesystem::current_path(targetPath);
    }
    igfd::ImGuiFileDialog::Instance()->OpenDialog("Save Flow", "Save Flow", ".flow", "", 1, nullptr, ImGuiFileDialogFlags_ConfirmOverwrite);
    igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".flow", ImVec4(0.0F, 1.0F, 0.0F, 0.9F));

    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper)
    {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            igfd::ImGuiFileDialog::Instance()->CloseDialog();
            return;
        }
    }

    if (igfd::ImGuiFileDialog::Instance()->FileDialog("Save Flow"))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk)
        {
            flow::SetCurrentFilename(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
            flow::SaveFlowAs(flow::GetCurrentFilename());
        }

        globalAction = GlobalActions::None;
        ax::NodeEditor::EnableShortcuts(true);
        igfd::ImGuiFileDialog::Instance()->CloseDialog();
    }
}

void NAV::gui::NodeEditorApplication::ShowClearNodesRequested()
{
    ax::NodeEditor::EnableShortcuts(false);

    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper)
    {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            return;
        }
    }

    ImGui::OpenPopup("Clear Nodes and discard unsaved changes?");
    if (ImGui::BeginPopupModal("Clear Nodes and discard unsaved changes?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Do you want to save your changes before clearing the nodes?");
        if (ImGui::Button("Save"))
        {
            if (flow::GetCurrentFilename().empty())
            {
                if (std::string targetPath = flow::GetProgramRootPath() + "/flow";
                    std::filesystem::current_path() != targetPath && std::filesystem::exists(targetPath))
                {
                    LOG_DEBUG("Changing current path to: {}", std::filesystem::current_path().string());
                    std::filesystem::current_path(targetPath);
                }
                igfd::ImGuiFileDialog::Instance()->OpenModal("Save Flow", "Save Flow", ".flow", "", 1, nullptr, ImGuiFileDialogFlags_ConfirmOverwrite);
                igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".flow", ImVec4(0.0F, 1.0F, 0.0F, 0.9F));
            }
            else
            {
                flow::SaveFlowAs(flow::GetCurrentFilename());
                globalAction = GlobalActions::None;
                nm::DeleteAllLinks();
                nm::DeleteAllNodes();
                flow::DiscardChanges();
                flow::SetCurrentFilename("");
                ax::NodeEditor::EnableShortcuts(true);
                ImGui::CloseCurrentPopup();
            }
        }

        if (igfd::ImGuiFileDialog::Instance()->FileDialog("Save Flow"))
        {
            if (igfd::ImGuiFileDialog::Instance()->IsOk)
            {
                flow::SetCurrentFilename(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
                flow::SaveFlowAs(flow::GetCurrentFilename());

                igfd::ImGuiFileDialog::Instance()->CloseDialog();
                nm::DeleteAllLinks();
                nm::DeleteAllNodes();
                flow::DiscardChanges();
                flow::SetCurrentFilename("");
            }

            globalAction = GlobalActions::None;

            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Discard"))
        {
            globalAction = GlobalActions::None;
            nm::DeleteAllLinks();
            nm::DeleteAllNodes();
            flow::DiscardChanges();
            flow::SetCurrentFilename("");
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void NAV::gui::NodeEditorApplication::ShowLoadRequested()
{
    ax::NodeEditor::EnableShortcuts(false);

    if (std::string targetPath = flow::GetProgramRootPath() + "/flow";
        std::filesystem::current_path() != targetPath && std::filesystem::exists(targetPath))
    {
        LOG_DEBUG("Changing current path to: {}", std::filesystem::current_path().string());
        std::filesystem::current_path(targetPath);
    }
    igfd::ImGuiFileDialog::Instance()->OpenDialog("Load Flow", "Load Flow", ".flow", "", 1, nullptr);
    igfd::ImGuiFileDialog::Instance()->SetExtentionInfos(".flow", ImVec4(0.0F, 1.0F, 0.0F, 0.9F));

    static bool loadSuccessful = true;

    auto& io = ImGui::GetIO();
    if (!io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper)
    {
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            loadSuccessful = true;
            igfd::ImGuiFileDialog::Instance()->CloseDialog();
            return;
        }
    }

    if (igfd::ImGuiFileDialog::Instance()->FileDialog("Load Flow"))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk)
        {
            loadSuccessful = flow::LoadFlow(igfd::ImGuiFileDialog::Instance()->GetFilePathName());
            if (loadSuccessful)
            {
                globalAction = GlobalActions::None;
                ax::NodeEditor::EnableShortcuts(true);
                frameCountNavigate = ImGui::GetFrameCount();
                igfd::ImGuiFileDialog::Instance()->CloseDialog();
            }
        }
        else
        {
            globalAction = GlobalActions::None;
            ax::NodeEditor::EnableShortcuts(true);
            igfd::ImGuiFileDialog::Instance()->CloseDialog();
        }
    }
    if (!loadSuccessful)
    {
        ImGui::OpenPopup("Could not open file");
    }

    if (ImGui::BeginPopupModal("Could not open file", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("The filename specified is invalid\nor the program has insufficient\npermissions to access the file");
        if (ImGui::Button("Close"))
        {
            loadSuccessful = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void NAV::gui::NodeEditorApplication::ShowRenameNodeRequest(Node*& renameNode)
{
    ed::EnableShortcuts(false);
    ImGui::OpenPopup("Rename Group Box");
    if (ImGui::BeginPopupModal("Rename Group Box", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string nameBackup = renameNode->name;
        if (nameBackup.empty())
        {
            nameBackup = renameNode->name;
        }

        auto& io = ImGui::GetIO();
        if (!io.KeyCtrl && !io.KeyAlt && !io.KeyShift && !io.KeySuper)
        {
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
            {
                if (renameNode)
                {
                    renameNode->name = nameBackup;
                }
                nameBackup.clear();
                renameNode = nullptr;
                ax::NodeEditor::EnableShortcuts(true);
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }
        }

        if (ImGui::InputTextWithHint("", "Enter the title here", &renameNode->name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            nameBackup.clear();
            renameNode = nullptr;
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        gui::widgets::HelpMarker("Hold SHIFT or use mouse to select text.\n"
                                 "CTRL+Left/Right to word jump.\n"
                                 "CTRL+A or double-click to select all.\n"
                                 "CTRL+X,CTRL+C,CTRL+V clipboard.\n"
                                 "CTRL+Z,CTRL+Y undo/redo.\n"
                                 "ESCAPE to revert.");
        if (ImGui::Button("Accept"))
        {
            nameBackup.clear();
            renameNode = nullptr;
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            if (renameNode)
            {
                renameNode->name = nameBackup;
            }
            nameBackup.clear();
            renameNode = nullptr;
            ax::NodeEditor::EnableShortcuts(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void NAV::gui::NodeEditorApplication::OnFrame(float deltaTime)
{
    if (frameCountNavigate && ImGui::GetFrameCount() - frameCountNavigate > 3)
    {
        frameCountNavigate = 0;
        ed::NavigateToContent();
    }

    switch (globalAction)
    {
    case GlobalActions::Quit:
        Quit();
        break;
    case GlobalActions::QuitUnsaved:
        ShowQuitRequested();
        break;
    case GlobalActions::SaveAs:
        ShowSaveAsRequested();
        break;
    case GlobalActions::Clear:
        ShowClearNodesRequested();
        break;
    case GlobalActions::Load:
        ShowLoadRequested();
        break;
    default:
        break;
    }

    if (!initList.empty())
    {
        Node* node = initList.front().first;
        bool init = initList.front().second;
        if ((init && !node->isInitializing())        // Finished with init (success or fail)
            || (!init && !node->isDeinitializing())) // Finished with deinit
        {
            initList.pop_front();
            currentInitNodeId = 0;
        }
        else if (currentInitNodeId == 0) // Currently no thread running
        {
            currentInitNodeId = size_t(node->id);
            if (initThread.joinable())
            {
                initThread.request_stop();
                initThread.join();
            }
            initThread = std::jthread([node, init]() {
                if (init)
                {
                    node->initializeNode();
                }
                else
                {
                    node->deinitializeNode();
                }
            });
        }
    }

    gui::UpdateTouch(deltaTime);

    if (ed::AreShortcutsEnabled())
    {
        gui::checkShortcuts(globalAction);
    }

    gui::menus::ShowMainMenuBar(globalAction);

    ed::SetCurrentEditor(m_Editor);

    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId contextPinId = 0;
    static bool createNewNode = false;
    static Pin* newNodeLinkPin = nullptr;

    static float leftPaneWidth = 400.0F;
    static float rightPaneWidth = 800.0F;
    gui::widgets::Splitter(true, 4.0F, &leftPaneWidth, &rightPaneWidth, 50.0F, 50.0F);

    gui::panels::ShowLeftPane(leftPaneWidth - 4.0F);

    ImGui::SameLine(0.0F, 12.0F);

    // ToolTips have to be shown outside of the NodeEditor Context, so save the Tooltip and push it afterwards
    std::string tooltipText;

    ed::Begin("Node editor");
    {
        static Pin* newLinkPin = nullptr;

        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        static util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));

        for (const auto& node : nm::m_Nodes()) // Blueprint || Simple
        {
            if (node->kind != Node::Kind::Blueprint && node->kind != Node::Kind::Simple)
            {
                continue;
            }

            const auto isSimple = node->kind == Node::Kind::Simple;

            bool hasOutputDelegates = false;
            bool hasOutputFlows = false;
            for (const auto& output : node->outputPins)
            {
                if (output.type == Pin::Type::Delegate)
                {
                    hasOutputDelegates = true;
                }
                else if (output.type == Pin::Type::Flow)
                {
                    hasOutputFlows = true;
                }
            }

            builder.Begin(node->id);

            if (!isSimple) // Header Text for Blueprint Nodes
            {
                if (node->isInitialized())
                {
                    builder.Header(ImColor(128, 255, 128)); // Light green
                }
                else
                {
                    builder.Header(node->color);
                }
                ImGui::Spring(0);
                ImGui::TextUnformatted(node->name.c_str());
                ImGui::Spring(1);
                if (node->isInitializing())
                {
                    gui::widgets::Spinner(("##Spinner " + node->nameId()).c_str(), ImColor(144, 238, 144), 10.0F, 1.0F);
                }
                else if (node->isDeinitializing())
                {
                    gui::widgets::Spinner(("##Spinner " + node->nameId()).c_str(), ImColor(255, 160, 122), 10.0F, 1.0F);
                }
                else if (hasOutputFlows)
                {
                    bool itemDisabled = false;
                    if (!node->isInitialized() && !node->callbacksEnabled)
                    {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5F);
                        itemDisabled = true;
                    }

                    ImGui::Checkbox("", &node->callbacksEnabled);
                    if (ImGui::IsItemHovered())
                    {
                        ed::Suspend();
                        ImGui::SetTooltip("Enable Callbacks");
                        ed::Resume();
                    }

                    if (itemDisabled)
                    {
                        ImGui::PopItemFlag();
                        ImGui::PopStyleVar();
                    }
                }
                else
                {
                    ImGui::Dummy(ImVec2(0, 28));
                }
                if (hasOutputDelegates)
                {
                    ImGui::BeginVertical("delegates", ImVec2(0, 28));
                    ImGui::Spring(1, 0);
                    for (const auto& output : node->outputPins)
                    {
                        if (output.type != Pin::Type::Delegate)
                        {
                            continue;
                        }

                        auto alpha = ImGui::GetStyle().Alpha;
                        if (newLinkPin && !newLinkPin->canCreateLink(output) && &output != newLinkPin)
                        {
                            alpha = alpha * (48.0F / 255.0F);
                        }

                        ed::BeginPin(output.id, ed::PinKind::Output);
                        ed::PinPivotAlignment(ImVec2(1.0F, 0.5F));
                        ed::PinPivotSize(ImVec2(0, 0));
                        ImGui::BeginHorizontal(output.id.AsPointer());
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                        if (!output.name.empty())
                        {
                            ImGui::TextUnformatted(output.name.c_str());
                            ImGui::Spring(0);
                        }
                        output.drawPinIcon(nm::IsPinLinked(output.id), static_cast<int>(alpha * 255));
                        ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                        ImGui::EndHorizontal();
                        ImGui::PopStyleVar();
                        ed::EndPin();

                        //DrawItemRect(ImColor(255, 0, 0));
                    }
                    ImGui::Spring(1, 0);
                    ImGui::EndVertical();
                    ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                }
                else
                {
                    ImGui::Spring(0);
                }
                builder.EndHeader();
            }

            for (const auto& input : node->inputPins) // Input Pins
            {
                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !newLinkPin->canCreateLink(input) && &input != newLinkPin)
                {
                    alpha = alpha * (48.0F / 255.0F);
                }

                builder.Input(input.id);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                input.drawPinIcon(nm::IsPinLinked(input.id), static_cast<int>(alpha * 255));
                if (ImGui::IsItemHovered())
                {
                    tooltipText = fmt::format("{}", fmt::join(input.dataIdentifier, "\n"));
                }

                ImGui::Spring(0);
                if (!input.name.empty())
                {
                    ImGui::TextUnformatted(input.name.c_str());
                    ImGui::Spring(0);
                }
                ImGui::PopStyleVar();
                util::BlueprintNodeBuilder::EndInput();
            }

            if (isSimple) // Middle Text for Simple Nodes
            {
                builder.Middle();

                ImGui::Spring(1, 0);
                ImGui::TextUnformatted(node->name.c_str());
                ImGui::Spring(1, 0);
            }

            for (const auto& output : node->outputPins) // Output Pins
            {
                if (!isSimple && output.type == Pin::Type::Delegate)
                {
                    continue;
                }

                auto alpha = ImGui::GetStyle().Alpha;
                if (newLinkPin && !newLinkPin->canCreateLink(output) && &output != newLinkPin)
                {
                    alpha = alpha * (48.0F / 255.0F);
                }

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output.id);
                if (!output.name.empty())
                {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output.name.c_str());
                }
                ImGui::Spring(0);
                output.drawPinIcon(nm::IsPinLinked(output.id), static_cast<int>(alpha * 255));
                if (ImGui::IsItemHovered())
                {
                    tooltipText = fmt::format("{}", fmt::join(output.dataIdentifier, "\n"));
                }
                ImGui::PopStyleVar();
                util::BlueprintNodeBuilder::EndOutput();
            }

            builder.End();
        }

        for (const auto& node : nm::m_Nodes()) // GroupBox
        {
            if (node->kind != Node::Kind::GroupBox)
            {
                continue;
            }

            const float commentAlpha = 0.75F;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node->id);
            ImGui::PushID(node->id.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            ImGui::TextUnformatted(node->name.c_str());
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ed::Group(node->size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node->id))
            {
                //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                auto min = ed::GetGroupMin();
                //auto max = ed::GetGroupMax();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::TextUnformatted(node->name.c_str());
                ImGui::EndGroup();

                auto* drawList = ed::GetHintBackgroundDrawList();

                auto ImRect_Expanded = [](const ImRect& rect, float x, float y) -> ImRect {
                    auto result = rect;
                    result.Min.x -= x;
                    result.Min.y -= y;
                    result.Max.x += x;
                    result.Max.y += y;
                    return result;
                };

                auto hintBounds = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0F);

                drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0F);

                //ImGui::PopStyleVar();
            }
            ed::EndGroupHint();
        }

        for (const auto& link : nm::m_Links()) // Links
        {
            ed::Link(link.id, link.startPinId, link.endPinId, link.color, 2.0F);
        }

        if (!createNewNode)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0F))
            {
                auto showLabel = [](const char* label, ImColor color) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto* drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15F);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0;
                ed::PinId endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto* startPin = nm::FindPin(startPinId);
                    auto* endPin = nm::FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin && startPin->kind == Pin::Kind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0F);
                        }
                        else if (endPin->kind == startPin->kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0F);
                        }
                        else if (endPin->parentNode == startPin->parentNode)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0F);
                        }
                        else if (endPin->type != startPin->type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0F);
                        }
                        else if (nm::IsPinLinked(endPin->id))
                        {
                            showLabel("End Pin already linked", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0F);
                        }
                        else if (startPin->type == Pin::Type::Flow
                                 && !NAV::NodeRegistry::NodeDataTypeIsChildOf(startPin->dataIdentifier, endPin->dataIdentifier))
                        {
                            showLabel(fmt::format("The data type [{}]\ncan't be linked to [{}]",
                                                  fmt::join(startPin->dataIdentifier, ","),
                                                  fmt::join(endPin->dataIdentifier, ","))
                                          .c_str(),
                                      ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0F);
                        }
                        else if (startPin->type == Pin::Type::Delegate
                                 && (startPin->parentNode == nullptr
                                     || std::find(endPin->dataIdentifier.begin(), endPin->dataIdentifier.end(), startPin->parentNode->type()) == endPin->dataIdentifier.end()))
                        {
                            if (startPin->parentNode != nullptr)
                            {
                                showLabel(fmt::format("The delegate type [{}]\ncan't be linked to [{}]",
                                                      startPin->parentNode->type(),
                                                      fmt::join(endPin->dataIdentifier, ","))
                                              .c_str(),
                                          ImColor(45, 32, 32, 180));
                            }

                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0F);
                        }
                        else if ((startPin->type == Pin::Type::Object || startPin->type == Pin::Type::Matrix || startPin->type == Pin::Type::Function)
                                 && (startPin->dataIdentifier.empty()
                                     || endPin->dataIdentifier.empty()
                                     || std::find(endPin->dataIdentifier.begin(), endPin->dataIdentifier.end(), startPin->dataIdentifier.front()) == endPin->dataIdentifier.end()))
                        {
                            showLabel(fmt::format("The data type [{}]\ncan't be linked to [{}]",
                                                  fmt::join(startPin->dataIdentifier, ","),
                                                  fmt::join(endPin->dataIdentifier, ","))
                                          .c_str(),
                                      ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0F);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0F))
                            {
                                nm::CreateLink(startPin, endPin);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    newLinkPin = nm::FindPin(pinId);
                    if (newLinkPin)
                    {
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));
                    }

                    if (ed::AcceptNewItem())
                    {
                        createNewNode = true;
                        newNodeLinkPin = nm::FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ed::Resume();
                    }
                }
            }
            else
            {
                newLinkPin = nullptr;
            }

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        nm::DeleteLink(linkId);
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (Node* node = nm::FindNode(nodeId))
                    {
                        if (node->isInitializing() || node->isDeinitializing())
                        {
                            break;
                        }
                    }
                    if (ed::AcceptDeletedItem())
                    {
                        nm::DeleteNode(nodeId);
                    }
                }
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    auto openPopupPosition = ImGui::GetMousePos();
    static bool showBackgroundContextMenu = false;
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
    {
        ImGui::OpenPopup("Node Context Menu");
    }
    else if (ed::ShowPinContextMenu(&contextPinId))
    {
        ImGui::OpenPopup("Pin Context Menu");
    }
    else if (ed::ShowLinkContextMenu(&contextLinkId) && ed::IsActive())
    {
        ImGui::OpenPopup("Link Context Menu");
    }
    else if (ed::ShowBackgroundContextMenu() && ed::IsActive())
    {
        ImGui::OpenPopup("Create New Node");
        showBackgroundContextMenu = true;
        newNodeLinkPin = nullptr;
    }
    else if (ed::NodeId doubleClickedNodeId = ed::GetDoubleClickedNode())
    {
        Node* node = nm::FindNode(doubleClickedNodeId);
        node->showConfig = true;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    static Node* renameNode = nullptr;
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto* node = nm::FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %lu", size_t(node->id));
            ImGui::Text("Type: %s", node->type().c_str());
            ImGui::Text("Kind: %s", std::string(node->kind).c_str());
            ImGui::Text("Inputs: %lu", node->inputPins.size());
            ImGui::Text("Outputs: %lu", node->outputPins.size());
            ImGui::Separator();
            if (ImGui::MenuItem(node->isInitialized() ? "Reinitialize" : "Initialize", "", false, !node->isInitializing() && !node->isDeinitializing()))
            {
                node->isInitializing_ = true;
                initList.emplace_back(node, true);
            }
            if (ImGui::MenuItem("Deinitialize", "", false, node->isInitialized() && !node->isInitializing() && !node->isDeinitializing()))
            {
                node->isDeinitializing_ = true;
                initList.emplace_back(node, false);
            }
            if (ImGui::MenuItem("Rename"))
            {
                renameNode = node;
            }
            if (ImGui::MenuItem("Delete", "", false, !node->isInitializing() && !node->isDeinitializing()))
            {
                ed::DeleteNode(contextNodeId);
            }
        }
        else
        {
            ImGui::Text("Unknown node: %lu", size_t(contextNodeId));
        }

        ImGui::EndPopup();
    }

    if (renameNode) // Popup for renaming a node
    {
        ShowRenameNodeRequest(renameNode);
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto* pin = nm::FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %lu", size_t(pin->id));
            ImGui::Text("Node: %s", pin->parentNode ? std::to_string(size_t(pin->parentNode->id)).c_str() : "<none>");
            ImGui::Text("Type: %s", std::string(pin->type).c_str());
            if (!pin->callbacks.empty())
            {
                ImGui::Text("Callbacks:");
                ImGui::Indent();
                for (auto& callback : pin->callbacks)
                {
                    ImGui::BulletText("%s", std::get<0>(callback)->nameId().c_str());
                }
                ImGui::Unindent();
            }
        }
        else
        {
            ImGui::Text("Unknown pin: %lu", size_t(contextPinId));
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto* link = nm::FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %lu", size_t(link->id));
            ImGui::Text("From: %lu", size_t(link->startPinId));
            ImGui::Text("To: %lu", size_t(link->endPinId));
        }
        else
        {
            ImGui::Text("Unknown link: %lu", size_t(contextLinkId));
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
        {
            ed::DeleteLink(contextLinkId);
        }
        ImGui::EndPopup();
    }

    static bool setKeyboardFocus = true;
    if (ImGui::BeginPopup("Create New Node"))
    {
        ax::NodeEditor::EnableShortcuts(false);
        auto newNodePostion = openPopupPosition;

        static ImGuiTextFilter filter;

        filter.Draw("");

        if (setKeyboardFocus)
        {
            filter.Clear();
            setKeyboardFocus = false;
            ImGui::SetKeyboardFocusHere(0);
        }

        Node* node = nullptr;
        for (const auto& [category, nodeInfoList] : NAV::NodeRegistry::RegisteredNodes())
        {
            // Prevent category from showing, if it is empty
            bool categoryHasItems = false;
            for (const auto& nodeInfo : nodeInfoList)
            {
                if (nodeInfo.hasCompatiblePin(newNodeLinkPin)
                    && (filter.PassFilter(nodeInfo.type.c_str()) || filter.PassFilter(category.c_str())))
                {
                    categoryHasItems = true;
                    break;
                }
            }
            if (categoryHasItems)
            {
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode((category + "##NewNodeTree").c_str()))
                {
                    for (const auto& nodeInfo : nodeInfoList)
                    {
                        const auto& displayName = nodeInfo.type;
                        const auto& constructor = nodeInfo.constructor;
                        ImGui::Indent();
                        if (nodeInfo.hasCompatiblePin(newNodeLinkPin)
                            && (filter.PassFilter(nodeInfo.type.c_str()) || filter.PassFilter(category.c_str()))
                            && ImGui::MenuItem(displayName.c_str()))
                        {
                            filter.Clear();
                            node = constructor();
                            nm::AddNode(node);
                            ax::NodeEditor::EnableShortcuts(true);
                        }
                        ImGui::Unindent();
                    }
                    ImGui::TreePop();
                }
            }
        }

        if (node)
        {
            createNewNode = false;

            ed::SetNodePosition(node->id, newNodePostion);

            if (auto* startPin = newNodeLinkPin)
            {
                auto& pins = startPin->kind == Pin::Kind::Input ? node->outputPins : node->inputPins;

                for (auto& pin : pins)
                {
                    if (startPin->canCreateLink(pin))
                    {
                        auto* endPin = &pin;
                        if (startPin->kind == Pin::Kind::Input)
                        {
                            std::swap(startPin, endPin);
                        }

                        nm::CreateLink(startPin, endPin);

                        break;
                    }
                }
            }
        }

        ImGui::EndPopup();
    }
    else
    {
        setKeyboardFocus = true;
        if (showBackgroundContextMenu)
        {
            showBackgroundContextMenu = false;
            ax::NodeEditor::EnableShortcuts(true);
        }
        createNewNode = false;
    }
    ImGui::PopStyleVar();

    for (const auto& node : nm::m_Nodes()) // Config Windows for nodes
    {
        if (node->hasConfig && node->showConfig)
        {
            if (!ImGui::Begin(fmt::format("{} ({})", node->type(), size_t(node->id)).c_str(), &(node->showConfig),
                              ImGuiWindowFlags_None))
            {
                if (node->nodeDisabledShortcuts)
                {
                    node->nodeDisabledShortcuts = false;
                    ax::NodeEditor::EnableShortcuts(true);
                }
                ImGui::End();
                continue;
            }

            ax::NodeEditor::EnableShortcuts(false);
            node->nodeDisabledShortcuts = true;
            node->guiConfig();

            ImGui::End();
        }
        else if (node->nodeDisabledShortcuts)
        {
            node->nodeDisabledShortcuts = false;
            ax::NodeEditor::EnableShortcuts(true);
        }
    }
    ed::Resume();

    ed::End();

    // Push the Tooltip on the stack if needed
    if (!tooltipText.empty())
    {
        ImGui::SetTooltip("%s", tooltipText.c_str());
    }

    ImGui::ShowDemoWindow();
    ImPlot::ShowDemoWindow();
    //ImGui::ShowMetricsWindow();

    std::string title = (flow::HasUnsavedChanges() ? "● " : "")
                        + (flow::GetCurrentFilename().empty() ? "" : flow::GetCurrentFilename() + " - ")
                        + "NavSoS";
    SetTitle(title.c_str());
}
