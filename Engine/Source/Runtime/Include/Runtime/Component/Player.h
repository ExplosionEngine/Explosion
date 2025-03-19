//
// Created by johnk on 2025/3/13.
//

#pragma once

#include <Render/View.h>
#include <Runtime/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass(globalComp, transient) PlayersInfo {
        EClassBody(PlayersInfo)

        PlayersInfo();

        std::vector<Entity> players;
    };

    struct RUNTIME_API EClass(transient) LocalPlayer {
        EClassBody(Player)

        LocalPlayer();

        uint8_t localPlayerIndex;
        Render::ViewState* viewState;
    };

#if BUILD_EDITOR
    struct RUNTIME_API EClass(transient) EditorPlayer {
        EClassBody(EditorVirtualPlayer)

        EditorPlayer();

        Render::ViewState* viewState;
    };
#endif

    struct RUNTIME_API EClass() PlayerStart {
        EClassBody(PlayerStart)

        PlayerStart();
    };
}
