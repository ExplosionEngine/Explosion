//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_TRANSFORMCOMPONENT_H
#define EXPLOSION_TRANSFORMCOMPONENT_H

#include <Math/Math.h>

namespace Explosion {

    struct Transform {
        Vector3 position;
        Vector3 scale;
        Quaternion rotate;
    };

    struct LocalTransformComponent {
        Transform local;
    };

    struct GlobalTransformComponent {
        Transform global;
    };

    struct HierarchyComponent {
        Entity first;
        Entity next;
        Entity prev;
        Entity parent;
    };

}

#endif // EXPLOSION_TRANSFORMCOMPONENT_H
