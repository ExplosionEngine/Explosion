//
// Created by johnk on 2024/10/15.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/World.h>
#include <Render/Scene.h>

namespace Runtime {
    struct SceneState {
        Render::Scene* scene;
        // TODO views
    };

    class EClass() SceneSystem final : public System {
        EPolyClassBody(SceneSystem)

        SceneSystem();
        ~SceneSystem() override;

        void Setup(Commands& inCommands) const override;
        void Tick(Commands& inCommands, float inTimeMs) const override;
    };
}
