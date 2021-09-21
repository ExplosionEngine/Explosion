//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_TRANSFORMCOMPONENT_H
#define EXPLOSION_TRANSFORMCOMPONENT_H

#include <Math/Math.h>
#include <Math/Transform.h>
#include <Engine/ECS.h>

namespace Explosion {

    struct LocalTransformComponent {
        Math::Transform local;
    };

    struct GlobalTransformComponent {
        Math::Transform global;
        Math::Matrix4 matrix;
    };

    struct ObjectIdComponent {
        ECS::Entity id;  // Entity & EntityMask
    };
}

#endif // EXPLOSION_TRANSFORMCOMPONENT_H
