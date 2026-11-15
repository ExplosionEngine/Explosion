#pragma once

#include <vector>

#include <Editor/Tab/EditorTab.h>
#include <Runtime/ECS.h>

namespace Editor {
    class EditorContext;

    class InspectorTab final : public EditorTab {
    public:
        explicit InspectorTab(EditorContext& inContext);

    private:
        void RenderContents() override;
        void RenderComponents(Runtime::ECRegistry& inRegistry);

        EditorContext& context;
        std::vector<Runtime::CompClass> componentClasses;
    };
}
