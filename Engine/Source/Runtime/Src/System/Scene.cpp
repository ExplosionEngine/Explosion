//
// Created by johnk on 2024/10/15.
//

#include <Runtime/System/Scene.h>
#include <Render/RenderModule.h>

namespace Runtime {
    SceneSystem::SceneSystem() = default;

    SceneSystem::~SceneSystem() = default;

    void SceneSystem::Setup(Commands& inCommands) const
    {
        inCommands.EmplaceState<SceneState>();
        inCommands.PatchState<SceneState>([](auto& state) -> void {
            auto& renderModule = Core::ModuleManager::Get().GetTyped<Render::RenderModule>("Render");
            state.scene = renderModule.AllocateScene();
        });
    }

    void SceneSystem::Tick(Commands& inCommands, float inTimeMs) const
    {
        // TODO
    }
}
