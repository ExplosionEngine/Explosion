#pragma once

#include <Editor/Tab/EditorTab.h>

namespace Runtime {
    class Canvas;
}

namespace Editor {
    class SceneClient;

    class SceneTab final : public EditorTab {
    public:
        SceneTab(SceneClient& inSceneClient, Runtime::Canvas& inCanvas);

    private:
        void RenderContents() override;
        void OnHidden() override;

        SceneClient& sceneClient;
        Runtime::Canvas& canvas;
    };
}
