//
// Created by johnk on 2025/3/13.
//

#pragma once

#include <Render/Scene.h>
#include <Runtime/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass(globalComp, transient) SceneHolder final {
        EClassBody(SceneHolder)

        explicit SceneHolder(Render::Scene* inScene);

        Render::Scene* scene;
    };
}
