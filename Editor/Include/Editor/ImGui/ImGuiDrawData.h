#pragma once

#include <imgui.h>

#include <Common/Utility.h>

namespace Editor {
    // Owns draw buffers and copied callback payloads, resolving ImGui texture references before queueing. Texture
    // views and external callback data are borrowed; their owners must drain rendering before releasing them.
    class ImGuiDrawData final {
    public:
        explicit ImGuiDrawData(const ImDrawData& inDrawData);
        ~ImGuiDrawData();

        NonCopyable(ImGuiDrawData)
        NonMovable(ImGuiDrawData)

        ImDrawData& Get();

    private:
        ImDrawData drawData;
    };
}
