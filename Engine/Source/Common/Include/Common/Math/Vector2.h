//
// Created by Zach Lee on 2021/11/12.
//


#pragma once

#include <Common/Math/Math.h>

namespace Common {

    struct Vector2 {
        union {
            float v[2];
            struct {
                float x;
                float y;
            };
        };

        inline Vector2();
        inline explicit Vector2(float v);
        inline Vector2(float x_, float y_);

        inline Vector2 operator+(const Vector2& rhs) const;

        inline Vector2 operator-() const;
        inline Vector2 operator-(const Vector2& rhs) const;

        inline Vector2 operator*(const Vector2& rhs) const;
        inline Vector2 operator*(float m) const;

        inline Vector2 operator/(const Vector2& rhs) const;
        inline Vector2 operator/(float d) const;

        inline Vector2& operator+=(const Vector2& rhs);
        inline Vector2& operator-=(const Vector2& rhs);
        inline Vector2& operator*=(const Vector2& rhs);
        inline Vector2& operator/=(const Vector2& rhs);
        inline Vector2& operator*=(float m);
        inline Vector2& operator/=(float d);

        inline float &operator[](uint32_t i);
        inline float operator[](uint32_t i) const;
    };
} // namespace Common
#include <Common/Math/Vector2.inl>