#pragma once

#include <string>

#include <Editor/Tab/EditorTab.h>
#include <Runtime/ECS.h>

namespace Editor {
    class EditorContext;

    class OutlinerTab final : public EditorTab {
    public:
        explicit OutlinerTab(EditorContext& inContext);

    private:
        void RenderContents() override;
        void RenderEntities(Runtime::ECRegistry& inRegistry);

        EditorContext& context;
        std::string createEntityName;
    };
}
