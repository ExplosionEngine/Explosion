//
// Created by johnk on 2024/10/14.
//

#pragma once

#include <Common/Math/Transform.h>
#include <Mirror/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass() Transform final {
        EClassBody(Transform)

        Transform();
        explicit Transform(const Common::FTransform& inLocalToWorld);

        EProperty() Common::FTransform localToWorld;
    };
}
