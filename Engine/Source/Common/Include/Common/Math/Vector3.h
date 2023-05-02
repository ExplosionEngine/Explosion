//
// Created by Zach Lee on 2021/11/12.
//


#pragma once

#include <Common/Math/Constant.h>

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

    inline Vector3::Vector3() : Vector3(0, 0, 0)
    {
    }

    inline Vector3::Vector3(float v) : Vector3(v, v, v)
    {
    }

    inline Vector3::Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_)
    {
    }

    inline Vector3 Vector3::operator+(const Vector3& rhs) const
    {
        return Vector3(*this) += rhs;
    }

    inline Vector3 Vector3::operator-() const
    {
        return Vector3(0) - *this;
    }

    inline Vector3 Vector3::operator-(const Vector3& rhs) const
    {
        return Vector3(*this) -= rhs;
    }

    inline Vector3 Vector3::operator*(const Vector3& rhs) const
    {
        return Vector3(*this) *= rhs;
    }

    inline Vector3 Vector3::operator*(float m) const
    {
        return Vector3(*this) *= m;
    }

    inline Vector3 Vector3::operator/(const Vector3& rhs) const
    {
        return Vector3(*this) /= rhs;
    }

    inline Vector3 Vector3::operator/(float d) const
    {
        return Vector3(*this) /= d;
    }

    inline Vector3& Vector3::operator+=(const Vector3& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator-=(const Vector3& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator*=(const Vector3& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator/=(const Vector3& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    inline Vector3& Vector3::operator*=(float m)
    {
        x *= m;
        y *= m;
        z *= m;
        return *this;
    }

    inline Vector3& Vector3::operator/=(float d)
    {
        x /= d;
        y /= d;
        z /= d;
        return *this;
    }

    inline float &Vector3::operator[](uint32_t i)
    {
        return v[i];
    }

    inline float Vector3::operator[](uint32_t i) const
    {
        return v[i];
    }

    inline Vector3 Vector3::Cross(const Vector3 &rhs) const
    {
        return {y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y};
    }

    inline float Vector3::Dot(const Vector3 &rhs) const
    {
        Vector3 ret = (*this) * rhs;
        return ret.x + ret.y + ret.z;
    }

    inline void Vector3::Normalize()
    {
        float inverseSqrt = 1 / sqrt(Dot(*this));
        Vector3::operator*=(inverseSqrt);
    }
} // namespace Common
