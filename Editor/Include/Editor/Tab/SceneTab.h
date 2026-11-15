#pragma once

#include <Editor/Tab/EditorTab.h>

namespace Editor {
    class SceneViewport;

    class SceneTab final : public EditorTab {
    public:
        explicit SceneTab(SceneViewport& inViewport);

    private:
        void RenderContents() override;
        void OnHidden() override;

        SceneViewport& viewport;
    };
}
