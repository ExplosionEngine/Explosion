#pragma once

#include <cstdint>
#include <string>

#include <imgui.h>

#include <Common/Utility.h>

namespace Editor {
    enum class TabDockArea : uint8_t {
        center,
        rightTop,
        rightBottom,
        bottom
    };

    struct EditorTabDesc {
        std::string id;
        std::string title;
        std::string icon;
        TabDockArea defaultDockArea = TabDockArea::center;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
        bool contentPadding = true;
    };

    class EditorTab {
    public:
        virtual ~EditorTab();

        NonCopyable(EditorTab)
        NonMovable(EditorTab)

        const EditorTabDesc& GetDesc() const;
        const std::string& GetWindowName() const;
        bool IsOpen() const;
        void SetOpen(bool inOpen);
        void Render();
        void RenderViewMenuItem();

    protected:
        explicit EditorTab(EditorTabDesc inDesc);

        virtual void RenderContents() = 0;
        virtual void OnHidden();

    private:
        EditorTabDesc desc;
        std::string windowName;
        bool open;
    };
}
