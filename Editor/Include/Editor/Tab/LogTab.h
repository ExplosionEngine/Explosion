#pragma once

#include <Editor/Tab/EditorTab.h>

namespace Editor {
    class EditorLogStream;

    class LogTab final : public EditorTab {
    public:
        explicit LogTab(EditorLogStream& inLogStream);

    private:
        void RenderContents() override;

        EditorLogStream& logStream;
    };
}
