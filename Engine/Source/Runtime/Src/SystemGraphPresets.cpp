//
// Created by johnk on 2025/3/13.
//

#include <Runtime/SystemGraphPresets.h>
#include <Runtime/System/Render.h>
#include <Runtime/System/Scene.h>
#include <Runtime/System/Transform.h>

namespace Runtime {
    const SystemGraph& SystemGraphPresets::Default3DWorld()
    {
        static SystemGraph graph = []() -> SystemGraph {
            SystemGraph systemGraph;

            auto& transformGroup = systemGraph.AddGroup("Transform", SystemExecuteStrategy::sequential);
            transformGroup.EmplaceSystem<TransformSystem>();

            auto& sceneRenderingGroup = systemGraph.AddGroup("SceneRendering", SystemExecuteStrategy::sequential);
            sceneRenderingGroup.EmplaceSystem<SceneSystem>();
            sceneRenderingGroup.EmplaceSystem<RenderSystem>();

            return systemGraph;
        }();
        return graph;
    }
}
