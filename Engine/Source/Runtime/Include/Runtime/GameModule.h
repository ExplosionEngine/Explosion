//
// Created by johnk on 2025/3/24.
//

#pragma once

#include <Runtime/Engine.h>

namespace Runtime {
    struct RUNTIME_API GameModule : EngineModule { // NOLINT
        Engine* CreateEngine(const EngineInitParams& inParams) override = 0;

        virtual std::string_view GetGameName() const = 0;
    };
}
