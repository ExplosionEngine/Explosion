#include <algorithm>
#include <cstdint>

#include <Editor/SceneClient.h>
#include <Editor/Tab/SceneTab.h>
#include <Editor/Widget/TablerIcons.h>
#include <Runtime/Canvas.h>

namespace Editor {
    SceneTab::SceneTab(SceneClient& inSceneClient, Runtime::Canvas& inCanvas)
        : EditorTab(EditorTabDesc {
            .id = "Scene",
            .title = "Scene",
            .icon = Icons::Tabler::viewportWide,
            .defaultDockArea = TabDockArea::center,
            .windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse,
            .contentPadding = false
        })
        , sceneClient(inSceneClient)
        , canvas(inCanvas)
    {
    }

    void SceneTab::RenderContents()
    {
        ImVec2 size = ImGui::GetContentRegionAvail();
        size.x = std::max(size.x, 1.0f);
        size.y = std::max(size.y, 1.0f);

        const ImVec2 framebufferScale = ImGui::GetIO().DisplayFramebufferScale;
        sceneClient.ResizeRenderSurface(
            std::max(1u, static_cast<uint32_t>(size.x * framebufferScale.x)),
            std::max(1u, static_cast<uint32_t>(size.y * framebufferScale.y)));
        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(canvas.GetTextureView())), size);
        sceneClient.SetSceneHovered(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        sceneClient.SetSceneFocused(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows));
    }

    void SceneTab::OnHidden()
    {
        sceneClient.EndCameraLook();
        sceneClient.SetSceneHovered(false);
        sceneClient.SetSceneFocused(false);
    }
}
