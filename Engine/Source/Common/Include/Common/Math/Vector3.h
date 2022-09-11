//
// Created by Zach Lee on 2021/11/12.
//


#pragma once

#include <Common/Math/Math.h>

namespace Common {

    struct Vector3 {
        union {
            float v[3];
            struct {
                float x;
                float y;
                float z;
            };
        };

        inline Vector3();
        inline explicit Vector3(float v);
        inline Vector3(float x_, float y_, float z_);

        inline Vector3 Cross(const Vector3 &rhs) const;
        inline float Dot(const Vector3 &rhs) const;
        inline void Normalize();

        inline Vector3 operator+(const Vector3& rhs) const;

        inline Vector3 operator-() const;
        inline Vector3 operator-(const Vector3& rhs) const;

        inline Vector3 operator*(const Vector3& rhs) const;
        inline Vector3 operator*(float m) const;

        inline Vector3 operator/(const Vector3& rhs) const;
        inline Vector3 operator/(float d) const;

        inline Vector3& operator+=(const Vector3& rhs);
        inline Vector3& operator-=(const Vector3& rhs);
        inline Vector3& operator*=(const Vector3& rhs);
        inline Vector3& operator/=(const Vector3& rhs);
        inline Vector3& operator*=(float m);
        inline Vector3& operator/=(float d);

        inline float &operator[](uint32_t i);
        inline float operator[](uint32_t i) const;
    };
} // namespace Common
#include <Common/Math/Vector3.inl>