//
// Created by Zach Lee on 2021/9/21.
//

#ifndef EXPLOSION_TRANSFORM_H
#define EXPLOSION_TRANSFORM_H

#include <Math/Math.h>

namespace Explosion::Math {

    struct Transform {
        Vector3 position  = ConstVector::ZERO;
        Vector3 scale     = ConstVector::UNIT;
        Quaternion rotate = ConstQuaternion::UNIT;

        Matrix4 ToMatrix() const;

        void Decompose(const Matrix4& matrix);
    };

}

#endif//EXPLOSION_TRANSFORM_H
