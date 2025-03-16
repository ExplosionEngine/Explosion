//
// Created by johnk on 2025/2/28.
//

#pragma once

#include <Runtime/Meta.h>
#include <Core/Uri.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass(gameReadOnly) GameSettings {
    public:
        EClassBody(GameSettings)

        GameSettings();

        EProperty(category=Player) uint8_t maxLocalPlayerNum;
        EProperty(category=Player) uint8_t initialLocalPlayerNum;

        EProperty(category=Map) Core::Uri editorStartupLevel;
        EProperty(category=Map) Core::Uri gameStartupLevel;

        // TODO more
    };
}
