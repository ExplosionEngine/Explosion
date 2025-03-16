//
// Created by johnk on 2025/3/13.
//

#pragma once

#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API SystemGraphPresets {
    public:
        static const SystemGraph& Default3DWorld();

        SystemGraphPresets() = delete;
    };
}
