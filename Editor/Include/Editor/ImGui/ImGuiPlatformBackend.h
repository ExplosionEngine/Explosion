#pragma once

#include <Common/Utility.h>

struct GLFWwindow;
struct ImGuiContext;

namespace Editor {
    struct ImGuiWindowDesc;

    class ImGuiPlatformBackend final {
    public:
        ImGuiPlatformBackend(GLFWwindow* inWindow, const ImGuiWindowDesc& inDesc);
        ~ImGuiPlatformBackend();

        NonCopyable(ImGuiPlatformBackend)
        NonMovable(ImGuiPlatformBackend)

        void NewFrame(float inDeltaSeconds) const;

    private:
        GLFWwindow* window;
        const ImGuiWindowDesc& desc;
        ImGuiContext* context;
    };
}
