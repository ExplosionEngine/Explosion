#pragma once

#include <Editor/SceneViewport.h>
#include <Editor/Tab/EditorTabs.h>
#include <Editor/ImGui/ImGuiWindow.h>

namespace Editor {
    class EditorContext;
    class EditorLogStream;

    class EditorMainWindow final : public ImGuiWindow {
    public:
        EditorMainWindow(EditorContext& inContext, EditorLogStream& inLogStream, Common::Path inAssetDirectory);
        ~EditorMainWindow() override;

        void Render(float inDeltaSeconds) override;

    private:
        void RenderContents() override;
        void DrawDockSpace() const;
        void RenderMenuBar();
        void HandleKey(int inKey, int inAction);
        void HandleMouseButton(int inButton, int inAction);
        void HandleCursor(double inX, double inY);

        EditorContext& context;
        // Releasing viewport mouse capture during destruction can dispatch a cursor callback.
        double lastCursorX;
        double lastCursorY;
        bool hasLastCursor;
        SceneViewport sceneViewport;
        EditorTabs tabs;
    };
}
