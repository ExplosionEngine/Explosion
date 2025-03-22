//
// Created by johnk on 2025/2/28.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API MetaPresets {
        static constexpr const auto* globalComp = "globalComp";
        static constexpr const auto* gameReadOnly = "gameReadOnly";
    };
}
