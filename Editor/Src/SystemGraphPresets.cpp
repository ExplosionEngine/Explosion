#include <Editor/System/Camera.h>
#include <Editor/SystemGraphPresets.h>
#include <Runtime/System/Scene.h>
#include <Runtime/System/Transform.h>

namespace Editor {
    const Runtime::SystemGraph& EditorSystemGraphPresets::DefaultEditorWorld()
    {
        static Runtime::SystemGraph graph = []() -> Runtime::SystemGraph {
            Runtime::SystemGraph systemGraph;

            auto& interactionGroup = systemGraph.AddGroup("EditorInteraction", Runtime::SystemExecuteStrategy::sequential);
            interactionGroup.EmplaceSystem<EditorCameraSystem>();

            auto& transformGroup = systemGraph.AddGroup("Transform", Runtime::SystemExecuteStrategy::sequential);
            transformGroup.EmplaceSystem<Runtime::TransformSystem>();

            auto& sceneGroup = systemGraph.AddGroup("Scene", Runtime::SystemExecuteStrategy::sequential);
            sceneGroup.EmplaceSystem<Runtime::SceneSystem>();

            return systemGraph;
        }();
        return graph;
    }
}
