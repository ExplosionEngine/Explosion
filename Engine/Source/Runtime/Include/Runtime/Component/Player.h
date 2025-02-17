//
// Created by johnk on 2025/2/17.
//

#pragma once

#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass(transient) Player final {
        EClassBody(Player)

        Player();

        EProperty() Entity camera;
        // TODO controller etc.
    };
}
