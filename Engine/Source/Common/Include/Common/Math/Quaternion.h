//
// Created by Zach Lee on 2021/11/13.
//

#pragma once

#include <Common/Math/Math.h>
#include <Common/Math/Vector3.h>

namespace Common {

    struct Quaternion {
        union {
            float v[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

        inline Quaternion();
        inline Quaternion(float w, float x, float y, float z);
        inline Quaternion(float angle, const Vector3 &axis);

        inline void Normalize();
        inline Quaternion Conjugate() const;

        inline Quaternion operator*(const Quaternion &rhs) const;
        inline Vector3 operator*(const Vector3 &rhs) const;

        inline Quaternion& operator*=(float m);
        inline Quaternion& operator/=(float d);
    };
} // namespace Common

#include <Common/Math/Quaternion.inl>