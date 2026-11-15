#pragma once

#include <Common/FileSystem.h>

#include <Editor/Tab/EditorTabs.h>

namespace Runtime {
    class Canvas;
}

namespace Editor {
    class EditorContext;
    class EditorLogStream;

    class EditorFrame final {
    public:
        EditorFrame(EditorContext& inContext, Runtime::Canvas& inSceneRenderCanvas, EditorLogStream& inLogStream, Common::Path inAssetDirectory);
        ~EditorFrame();

        void Render(bool& outRequestQuit);

    private:
        void DrawDockSpace() const;
        void RenderMenuBar(bool& outRequestQuit);

        EditorContext& context;
        EditorTabs tabs;
    };
}
