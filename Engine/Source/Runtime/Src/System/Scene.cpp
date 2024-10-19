//
// Created by johnk on 2024/10/15.
//

#include <Runtime/System/Scene.h>
#include <Runtime/Engine.h>

namespace Runtime {
    SceneSystem::SceneSystem() = default;

    SceneSystem::~SceneSystem() = default;

    void SceneSystem::Setup(Commands& inCommands) const
    {
        inCommands.EmplaceState<SceneState>();
        inCommands.PatchState<SceneState>([](auto& state) -> void {
            auto& renderModule = EngineHolder::Get().GetRenderModule();
            state.scene = renderModule.AllocateScene();
        });
    }

    void SceneSystem::Tick(Commands& inCommands, float inTimeMs) const
    {
        // TODO
    }

    void SceneSystem::Stop(Commands& inCommands) const
    {
        auto& renderModule = EngineHolder::Get().GetRenderModule();
        renderModule.DestroyScene(inCommands.GetState<SceneState>().scene);
    }
}
