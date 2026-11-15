#include <Editor/Tab/EditorTab.h>
#include <Editor/Widget/IconWidgets.h>

namespace Editor {
    EditorTab::EditorTab(EditorTabDesc inDesc)
        : desc(std::move(inDesc))
        , windowName(Widgets::Label(desc.icon.c_str(), desc.title, desc.id))
        , open(true)
    {
    }

    EditorTab::~EditorTab() = default;

    const EditorTabDesc& EditorTab::GetDesc() const
    {
        return desc;
    }

    const std::string& EditorTab::GetWindowName() const
    {
        return windowName;
    }

    bool EditorTab::IsOpen() const
    {
        return open;
    }

    void EditorTab::SetOpen(bool inOpen)
    {
        if (open == inOpen) {
            return;
        }
        open = inOpen;
        if (!open) {
            OnHidden();
        }
    }

    void EditorTab::Render()
    {
        if (!open) {
            return;
        }
        if (!desc.contentPadding) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }
        bool requestedOpen = open;
        const bool contentsVisible = ImGui::Begin(windowName.c_str(), &requestedOpen, desc.windowFlags);
        if (!desc.contentPadding) {
            ImGui::PopStyleVar();
        }
        SetOpen(requestedOpen);
        if (contentsVisible && open) {
            RenderContents();
        } else if (open) {
            OnHidden();
        }
        ImGui::End();
    }

    void EditorTab::RenderViewMenuItem()
    {
        bool requestedOpen = open;
        if (ImGui::MenuItem(windowName.c_str(), nullptr, &requestedOpen)) {
            SetOpen(requestedOpen);
        }
    }

    void EditorTab::OnHidden()
    {
    }
}
