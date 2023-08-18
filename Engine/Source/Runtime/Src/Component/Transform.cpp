//
// Created by johnk on 2023/8/16.
//

#include <Runtime/Component/Transform.h>

namespace Runtime {
    TransformComponent::TransformComponent()
        : transform()
    {
    }

    const Common::FVec3& TransformComponent::GetPosition() const
    {
        return transform.translation;
    }

    const Common::FQuat& TransformComponent::GetRotation() const
    {
        return transform.rotation;
    }

    const Common::FVec3& TransformComponent::GetScale() const
    {
        return transform.scale;
    }

    void TransformComponent::SetPosition(const Common::FVec3& inPosition)
    {
        transform.translation = inPosition;
    }

    void TransformComponent::SetRotation(const Common::FQuat& inRotation)
    {
        transform.rotation = inRotation;
    }

    void TransformComponent::SetScale(const Common::FVec3& inScale)
    {
        transform.scale = inScale;
    }

    void TransformComponent::LookTo(const Common::FVec3& inTargetPosition, const Common::FVec3& inUpDirection)
    {
        transform.LookTo(transform.translation, inTargetPosition, inUpDirection);
    }

    void TransformComponent::MoveAndLookAt(const Common::FVec3& inPosition, const Common::FVec3& inTargetPosition, const Common::FVec3& inUpDirection)
    {
        transform.LookTo(inPosition, inTargetPosition, inUpDirection);
    }
}
