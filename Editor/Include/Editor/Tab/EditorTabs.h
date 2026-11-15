#pragma once

#include <concepts>
#include <utility>
#include <vector>

#include <Common/Memory.h>
#include <Editor/Tab/EditorTab.h>

namespace Editor {
    class EditorTabs final {
    public:
        template <typename T, typename... Args> requires std::derived_from<T, EditorTab> T& Emplace(Args&&... inArgs);
        void Render();
        void RenderViewMenuItems();
        void BuildInitialDockLayout(ImGuiID inDockSpaceId, const ImVec2& inSize) const;

    private:
        std::vector<Common::UniquePtr<EditorTab>> tabs;
    };
}

namespace Editor {
    template <typename T, typename... Args> requires std::derived_from<T, EditorTab>
    T& EditorTabs::Emplace(Args&&... inArgs)
    {
        auto tab = std::make_unique<T>(std::forward<Args>(inArgs)...);
        T& result = *tab;
        tabs.emplace_back(std::move(tab));
        return result;
    }
}
