//
// Created by Zach Lee on 2021/11/12.
//


#pragma once

#include <Common/Math/Math.h>
#include <Common/Math/Vector3.h>
#include <Common/Math/Vector4.h>

namespace Common {

    struct Matrix4 {
        union {
            float v[16];
            struct {
                Vector4 m[4];
            };
        };

        inline Matrix4();
        inline Matrix4(const Vector4 &r0, const Vector4 &r1, const Vector4 &r2, const Vector4 &r3);

        inline static const Matrix4 &Identity();

        inline void Translate(const Vector3 &rhs);
        inline Matrix4 Inverse() const;
        inline Matrix4 InverseTranspose() const;

        inline Matrix4 operator+(const Matrix4& rhs) const;
        inline Matrix4 operator-(const Matrix4& rhs) const;
        inline Matrix4 operator*(const Matrix4& rhs) const;
        inline Matrix4 operator*(float multiplier) const;
        inline Matrix4 operator/(float divisor) const;
        inline Matrix4 operator-() const;

        inline Matrix4& operator+=(const Matrix4& rhs);
        inline Matrix4& operator-=(const Matrix4& rhs);
        inline Matrix4& operator*=(const Matrix4& rhs);
        inline Matrix4& operator*=(float multiplier);
        inline Matrix4& operator/=(float divisor);

        inline Vector4 operator*(const Vector4& rhs) const;

        inline Vector4 &operator[](uint32_t i);
        inline Vector4 operator[](uint32_t i) const;
    };
} // namespace Common
#include <Common/Math/Matrix4.inl>