#include <algorithm>

#include <imgui_internal.h>

#include <Editor/Tab/EditorTabs.h>

namespace Editor::TabLayout::Internal {
    constexpr float defaultRightColumnRatio = 0.22f;
    constexpr float minRightColumnWidth = 360.0f;
    constexpr float maxRightColumnWidth = 480.0f;
    constexpr float defaultBottomAreaRatio = 0.22f;
    constexpr float minBottomAreaHeight = 200.0f;
    constexpr float maxBottomAreaHeight = 300.0f;
    constexpr float defaultRightTopRatio = 0.35f;
    constexpr float minRightTopHeight = 280.0f;
    constexpr float maxRightTopHeight = 420.0f;

    static float CalculateDockRatio(float inAvailableSize, float inDefaultRatio, float inMinSize, float inMaxSize)
    {
        if (inAvailableSize <= 0.0f) {
            return inDefaultRatio;
        }
        const float areaSize = std::clamp(inAvailableSize * inDefaultRatio, inMinSize, inMaxSize);
        return std::min(areaSize / inAvailableSize, 0.5f);
    }
}

namespace Editor {
    void EditorTabs::Render()
    {
        for (const auto& tab : tabs) {
            tab->Render();
        }
    }

    void EditorTabs::RenderViewMenuItems()
    {
        for (const auto& tab : tabs) {
            tab->RenderViewMenuItem();
        }
    }

    void EditorTabs::BuildInitialDockLayout(ImGuiID inDockSpaceId, const ImVec2& inSize) const
    {
        if (ImGui::DockBuilderGetNode(inDockSpaceId) != nullptr) {
            return;
        }
        ImGui::DockBuilderAddNode(inDockSpaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(inDockSpaceId, inSize);

        ImGuiID centerNodeId = 0;
        ImGuiID rightColumnNodeId = 0;
        ImGuiID rightTopNodeId = 0;
        ImGuiID rightBottomNodeId = 0;
        ImGuiID bottomNodeId = 0;
        const float rightColumnRatio = TabLayout::Internal::CalculateDockRatio(inSize.x, TabLayout::Internal::defaultRightColumnRatio, TabLayout::Internal::minRightColumnWidth, TabLayout::Internal::maxRightColumnWidth);
        const float bottomRatio = TabLayout::Internal::CalculateDockRatio(inSize.y, TabLayout::Internal::defaultBottomAreaRatio, TabLayout::Internal::minBottomAreaHeight, TabLayout::Internal::maxBottomAreaHeight);
        const float rightTopRatio = TabLayout::Internal::CalculateDockRatio(inSize.y, TabLayout::Internal::defaultRightTopRatio, TabLayout::Internal::minRightTopHeight, TabLayout::Internal::maxRightTopHeight);
        ImGui::DockBuilderSplitNode(inDockSpaceId, ImGuiDir_Right, rightColumnRatio, &rightColumnNodeId, &centerNodeId);
        ImGui::DockBuilderSplitNode(centerNodeId, ImGuiDir_Down, bottomRatio, &bottomNodeId, &centerNodeId);
        ImGui::DockBuilderSplitNode(rightColumnNodeId, ImGuiDir_Up, rightTopRatio, &rightTopNodeId, &rightBottomNodeId);

        for (const auto& tab : tabs) {
            ImGuiID nodeId = centerNodeId;
            switch (tab->GetDesc().defaultDockArea) {
            case TabDockArea::center: nodeId = centerNodeId; break;
            case TabDockArea::rightTop: nodeId = rightTopNodeId; break;
            case TabDockArea::rightBottom: nodeId = rightBottomNodeId; break;
            case TabDockArea::bottom: nodeId = bottomNodeId; break;
            }
            ImGui::DockBuilderDockWindow(tab->GetWindowName().c_str(), nodeId);
        }
        ImGui::DockBuilderFinish(inDockSpaceId);
    }
}
