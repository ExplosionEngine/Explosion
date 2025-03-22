//
// Created by johnk on 2025/3/13.
//

#include <Runtime/Component/Player.h>

namespace Runtime {
    PlayersInfo::PlayersInfo() = default;

    LocalPlayer::LocalPlayer()
        : localPlayerIndex(0)
        , viewState(nullptr)
    {
    }

#if BUILD_EDITOR
    EditorPlayer::EditorPlayer()
        : viewState(nullptr)
    {
    }
#endif

    PlayerStart::PlayerStart() = default;
} // namespace Runtime
