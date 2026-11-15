#include <algorithm>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <Editor/EditorWindow.h>
#include <Editor/ImGui/ImGuiPlatformBackend.h>
#include <Editor/Utils/ImGuiCompatibility.h>

namespace Editor {
    ImGuiPlatformBackend::ImGuiPlatformBackend(EditorWindow& inWindow)
        : window(inWindow)
    {
        ImGui::GetIO().BackendPlatformName = "ExplosionGLFW";
        keyHandle = window.onKey.BindLambda([](int inKey, int inScancode, int inAction, int inMods) -> void {
            ImGuiCompatibility::UpdateKeyModifiers(inMods);
            const ImGuiKey key = ImGuiCompatibility::ToImGuiKey(inKey);
            if (key != ImGuiKey_None) {
                ImGuiIO& io = ImGui::GetIO();
                io.AddKeyEvent(key, inAction != GLFW_RELEASE);
                io.SetKeyEventNativeData(key, inKey, inScancode);
            }
        });
        charHandle = window.onChar.BindLambda([](uint32_t inCodepoint) -> void {
            ImGui::GetIO().AddInputCharacter(inCodepoint);
        });
        mouseButtonHandle = window.onMouseButton.BindLambda([](int inButton, int inAction, int inMods) -> void {
            ImGuiCompatibility::UpdateKeyModifiers(inMods);
            if (const auto button = ImGuiCompatibility::ToImGuiMouseButton(inButton)) {
                ImGui::GetIO().AddMouseButtonEvent(*button, inAction == GLFW_PRESS);
            }
        });
        cursorHandle = window.onCursor.BindLambda([](double inX, double inY) -> void {
            ImGui::GetIO().AddMousePosEvent(static_cast<float>(inX), static_cast<float>(inY));
        });
        scrollHandle = window.onScroll.BindLambda([](double inX, double inY) -> void {
            ImGui::GetIO().AddMouseWheelEvent(static_cast<float>(inX), static_cast<float>(inY));
        });
        focusHandle = window.onFocus.BindLambda([](bool inFocused) -> void {
            ImGui::GetIO().AddFocusEvent(inFocused);
        });
    }

    ImGuiPlatformBackend::~ImGuiPlatformBackend()
    {
        window.onKey.Unbind(keyHandle);
        window.onChar.Unbind(charHandle);
        window.onMouseButton.Unbind(mouseButtonHandle);
        window.onCursor.Unbind(cursorHandle);
        window.onScroll.Unbind(scrollHandle);
        window.onFocus.Unbind(focusHandle);
        ImGui::GetIO().BackendPlatformName = nullptr;
    }

    void ImGuiPlatformBackend::NewFrame(float inDeltaSeconds) const
    {
        int width = 0;
        int height = 0;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetWindowSize(window.GetNativeWindow(), &width, &height);
        glfwGetFramebufferSize(window.GetNativeWindow(), &framebufferWidth, &framebufferHeight);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
        io.DisplayFramebufferScale = ImVec2(
            width > 0 ? static_cast<float>(framebufferWidth) / static_cast<float>(width) : 1.0f,
            height > 0 ? static_cast<float>(framebufferHeight) / static_cast<float>(height) : 1.0f);
        io.DeltaTime = std::max(inDeltaSeconds, 1.0f / 1000.0f);
    }
}
