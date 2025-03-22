//
// Created by johnk on 2025/3/13.
//

#include <Runtime/System/Player.h>
#include <Runtime/System/Render.h>
#include <Runtime/System/Scene.h>
#include <Runtime/System/Transform.h>
#include <Runtime/SystemGraphPresets.h>

namespace Runtime {
    const SystemGraph& SystemGraphPresets::Default3DWorld()
    {
        static SystemGraph graph = []() -> SystemGraph {
            SystemGraph systemGraph;

            auto& preTransformGroup = systemGraph.AddGroup("PreTransformConcurrent", SystemExecuteStrategy::concurrent);
            preTransformGroup.EmplaceSystem<PlayerSystem>();

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
