//
// Created by LiZhen on 2021/9/12.
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

    struct TransformComponent {
        Transform local;
        Transform global;
    };

}

#endif // EXPLOSION_TRANSFORMCOMPONENT_H
