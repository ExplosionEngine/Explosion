//
// Created by johnk on 2025/3/13.
//

#pragma once

#include <Render/View.h>
#include <Runtime/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass(transient) LocalPlayer {
        EClassBody(Player)

        LocalPlayer();

        Entity activeCamera;
        Render::ViewState* viewState;
    };

#if BUILD_EDITOR
    struct RUNTIME_API EClass(transient) EditorVirtualPlayer {
        EClassBody(EditorVirtualPlayer)

        EditorVirtualPlayer();

        Entity activeCamera;
        Render::ViewState* viewState;
    };
#endif
}
