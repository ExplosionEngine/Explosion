//
// Created by johnk on 2025/3/13.
//

#include <Runtime/Component/Player.h>

namespace Runtime {
    LocalPlayer::LocalPlayer()
        : activeCamera(entityNull)
    {
    }

#if BUILD_EDITOR
    EditorVirtualPlayer::EditorVirtualPlayer()
        : activeCamera(entityNull)
    {
    }
#endif
}
