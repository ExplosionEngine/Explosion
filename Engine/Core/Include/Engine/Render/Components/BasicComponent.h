//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_TRANSFORMCOMPONENT_H
#define EXPLOSION_TRANSFORMCOMPONENT_H

#include <Math/Math.h>
#include <Engine/ECS.h>

namespace Explosion {

    struct Transform {
        Math::Vector3 position;
        Math::Vector3 scale;
        Math::Quaternion rotate;
    };

    struct LocalTransformComponent {
        Transform local;
    };

    struct GlobalTransformComponent {
        Transform global;
    };
}

#endif // EXPLOSION_TRANSFORMCOMPONENT_H
