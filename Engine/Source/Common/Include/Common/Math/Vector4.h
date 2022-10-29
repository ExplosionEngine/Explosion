//
// Created by Zach Lee on 2021/11/12.
//


#pragma once

#include <Common/Math/Math.h>

namespace Common {

    struct Vector4 {
        union {
            float v[4];
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

        inline Vector4();
        inline explicit Vector4(float v);
        inline Vector4(float x_, float y_, float z_, float w_);

        inline void Normalize();
        inline float Dot(const Vector4 &rhs) const;

        inline Vector4 operator+(const Vector4& rhs) const;

        inline Vector4 operator-() const;
        inline Vector4 operator-(const Vector4& rhs) const;

        inline Vector4 operator*(const Vector4& rhs) const;
        inline Vector4 operator*(float m) const;

        inline Vector4 operator/(const Vector4& rhs) const;
        inline Vector4 operator/(float d) const;

        inline Vector4& operator+=(const Vector4& rhs);
        inline Vector4& operator-=(const Vector4& rhs);
        inline Vector4& operator*=(const Vector4& rhs);
        inline Vector4& operator/=(const Vector4& rhs);
        inline Vector4& operator*=(float m);
        inline Vector4& operator/=(float d);

        inline float &operator[](uint32_t i);
        inline float operator[](uint32_t i) const;
    };
} // namespace Common

#include <Common/Math/Vector4.inl>