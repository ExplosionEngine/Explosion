//
// Created by johnk on 2024/10/14.
//

#pragma once

#include <Common/Container.h>
#include <Mirror/Meta.h>
#include <Runtime/Api.h>
#include <Runtime/ECS.h>

namespace Runtime {
    struct RUNTIME_API EClass(transient) Camera final {
        EClassBody(Camera)

        Camera();

        EProperty() bool perspective;
        EProperty() float nearPlane;
        EProperty() std::optional<float> farPlane;
        // only need when perspective
        EProperty() std::optional<float> fov;
    };

    // TODO scene capture
}
