//
// Created by johnk on 2023/8/16.
//

#pragma once

#include <functional>

#include <Common/Math/Transform.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class EClass() TransformComponent : public Component {
    public:
        EClassBody(TransformComponent)

        ECtor()
        TransformComponent();

        EFunc()
        const Common::FVec3& GetPosition() const;

        EFunc()
        const Common::FQuat& GetRotation() const;

        EFunc()
        const Common::FVec3& GetScale() const;

        EFunc()
        void SetPosition(const Common::FVec3& inPosition);

        EFunc()
        void SetRotation(const Common::FQuat& inRotation);

        EFunc()
        void SetScale(const Common::FVec3& inScale);

        EFunc()
        void LookTo(const Common::FVec3& inTargetPosition, const Common::FVec3& inUpDirection = Common::FVec3Consts::unitZ);

        EFunc()
        void MoveAndLookAt(const Common::FVec3& inPosition, const Common::FVec3& inTargetPosition, const Common::FVec3& inUpDirection = Common::FVec3Consts::unitZ);

    private:
        EProperty(category=Transform)
        Common::FTransform transform;
    };
}
