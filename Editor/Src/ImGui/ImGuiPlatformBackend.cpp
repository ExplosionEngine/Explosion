#include <algorithm>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include <Editor/ImGui/ImGuiPlatformBackend.h>
#include <Editor/ImGui/ImGuiWindow.h>
#include <Editor/Utils/ImGuiCompatibility.h>

namespace Editor {
    ImGuiPlatformBackend::ImGuiPlatformBackend(GLFWwindow* inWindow, const ImGuiWindowDesc& inDesc)
        : window(inWindow)
        , desc(inDesc)
        , context(ImGui::GetCurrentContext())
    {
        ImGui::GetIO().BackendPlatformName = "ExplosionGLFW";
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, [](GLFWwindow* inWindow, int inKey, int inScancode, int inAction, int inMods) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGuiCompatibility::UpdateKeyModifiers(inMods);
            const ImGuiKey key = ImGuiCompatibility::ToImGuiKey(inKey);
            if (key != ImGuiKey_None) {
                ImGuiIO& io = ImGui::GetIO();
                io.AddKeyEvent(key, inAction != GLFW_RELEASE);
                io.SetKeyEventNativeData(key, inKey, inScancode);
            }
            if (backend.desc.onKey) {
                backend.desc.onKey(inKey, inScancode, inAction, inMods);
            }
        });
        glfwSetCharCallback(window, [](GLFWwindow* inWindow, unsigned int inCodepoint) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGui::GetIO().AddInputCharacter(inCodepoint);
            if (backend.desc.onChar) {
                backend.desc.onChar(inCodepoint);
            }
        });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* inWindow, int inButton, int inAction, int inMods) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGuiCompatibility::UpdateKeyModifiers(inMods);
            if (const auto button = ImGuiCompatibility::ToImGuiMouseButton(inButton)) {
                ImGui::GetIO().AddMouseButtonEvent(*button, inAction == GLFW_PRESS);
            }
            if (backend.desc.onMouseButton) {
                backend.desc.onMouseButton(inButton, inAction, inMods);
            }
        });
        glfwSetCursorPosCallback(window, [](GLFWwindow* inWindow, double inX, double inY) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGui::GetIO().AddMousePosEvent(static_cast<float>(inX), static_cast<float>(inY));
            if (backend.desc.onCursor) {
                backend.desc.onCursor(inX, inY);
            }
        });
        glfwSetScrollCallback(window, [](GLFWwindow* inWindow, double inX, double inY) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGui::GetIO().AddMouseWheelEvent(static_cast<float>(inX), static_cast<float>(inY));
            if (backend.desc.onScroll) {
                backend.desc.onScroll(inX, inY);
            }
        });
        glfwSetWindowFocusCallback(window, [](GLFWwindow* inWindow, int inFocused) -> void {
            const auto& backend = *static_cast<ImGuiPlatformBackend*>(glfwGetWindowUserPointer(inWindow));
            ImGui::SetCurrentContext(backend.context);
            ImGui::GetIO().AddFocusEvent(inFocused);
            if (backend.desc.onFocus) {
                backend.desc.onFocus(inFocused == GLFW_TRUE);
            }
        });
    }

    ImGuiPlatformBackend::~ImGuiPlatformBackend()
    {
        glfwSetKeyCallback(window, nullptr);
        glfwSetCharCallback(window, nullptr);
        glfwSetMouseButtonCallback(window, nullptr);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
        glfwSetWindowFocusCallback(window, nullptr);
        glfwSetWindowUserPointer(window, nullptr);
        ImGui::SetCurrentContext(context);
        ImGui::GetIO().BackendPlatformName = nullptr;
    }

    void ImGuiPlatformBackend::NewFrame(float inDeltaSeconds) const
    {
        int width = 0;
        int height = 0;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetWindowSize(window, &width, &height);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
        io.DisplayFramebufferScale = ImVec2(
            width > 0 ? static_cast<float>(framebufferWidth) / static_cast<float>(width) : 1.0f,
            height > 0 ? static_cast<float>(framebufferHeight) / static_cast<float>(height) : 1.0f);
        io.DeltaTime = std::max(inDeltaSeconds, 1.0f / 1000.0f);
    }
}
