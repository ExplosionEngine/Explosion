//
// Created by johnk on 2025/2/18.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Client.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass(global, transient) GWorldContext final {
        EClassBody(GWorldContext)

        GWorldContext();

        EProperty() Client* client;
    };
}
