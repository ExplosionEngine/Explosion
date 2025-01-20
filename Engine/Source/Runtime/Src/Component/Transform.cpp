//
// Created by johnk on 2024/10/14.
//

#include <Runtime/Component/Transform.h>

namespace Runtime {
    Transform::Transform() = default;

    Transform::Transform(const Common::FTransform& inLocalToWorld)
        : localToWorld(inLocalToWorld)
    {
    }

    ChildOfConstraint::ChildOfConstraint() = default;

    ChildOfConstraint::ChildOfConstraint(Entity inParent, const Common::FTransform& inLocalToParent)
        : parent(inParent)
        , localToParent(inLocalToParent)
    {
    }

    CopyConstraint::CopyConstraint() = default;

    CopyConstraint::CopyConstraint(Entity inTarget)
        : target(inTarget)
    {
    }
}
