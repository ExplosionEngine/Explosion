#include <algorithm>
#include <cstdint>

#include <Editor/SceneViewport.h>
#include <Editor/Tab/SceneTab.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor {
    SceneTab::SceneTab(SceneViewport& inViewport)
        : EditorTab(EditorTabDesc {
            .id = "Scene",
            .title = "Scene",
            .icon = Icons::Tabler::viewportWide,
            .defaultDockArea = TabDockArea::center,
            .windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse,
            .contentPadding = false
        })
        , viewport(inViewport)
    {
    }

    void SceneTab::RenderContents()
    {
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x = std::max(size.x, 1.0f);
        size.y = std::max(size.y, 1.0f);

        const ImVec2 framebufferScale = ImGui::GetIO().DisplayFramebufferScale;
        viewport.SetVisible(true);
        viewport.Resize(
            std::max(1u, static_cast<uint32_t>(size.x * framebufferScale.x)),
            std::max(1u, static_cast<uint32_t>(size.y * framebufferScale.y)));
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(viewport.GetTextureView())), size);
        viewport.SetHovered(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        viewport.SetFocused(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows));
    }

    void SceneTab::OnHidden()
    {
        viewport.SetVisible(false);
    }
}
