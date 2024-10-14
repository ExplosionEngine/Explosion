//
// Created by johnk on 2024/10/15.
//

#include <Runtime/System/Scene.h>
#include <Rendering/RenderingModule.h>

namespace Runtime {
    SceneSystem::SceneSystem() = default;

    SceneSystem::~SceneSystem() = default;

    void SceneSystem::Setup(Commands& inCommands) const
    {
        inCommands.EmplaceState<SceneState>();
        inCommands.PatchState<SceneState>([](auto& state) -> void {
            auto& renderingModule = Core::ModuleManager::Get().GetOrLoadTyped<Rendering::RenderingModule>("Rendering");
            state.scene = renderingModule.AllocateScene();
        });
    }

    void SceneSystem::Tick(Commands& inCommands, float inTimeMs) const
    {
        // TODO
    }
}
