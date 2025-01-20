//
// Created by johnk on 2024/10/14.
//

#pragma once

#include <Common/Math/Transform.h>
#include <Mirror/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass() Transform final {
        EClassBody(Transform)

        Transform();
        explicit Transform(const Common::FTransform& inLocalToWorld);

        EProperty() Common::FTransform localToWorld;
    };

    struct RUNTIME_API EClass() ChildOfConstraint final {
        EClassBody(ChildOfConstraint)

        ChildOfConstraint();
        explicit ChildOfConstraint(Entity inParent, const Common::FTransform& inLocalToParent);

        EProperty() Entity parent;
        EProperty() Common::FTransform localToParent;
    };

    struct RUNTIME_API EClass() CopyConstraint final {
        EClassBody(CopyConstraint)

        CopyConstraint();
        explicit CopyConstraint(Entity inTarget);

        EProperty() Entity target;
    };
}
