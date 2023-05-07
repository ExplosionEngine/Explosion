//
// Created by Zach Lee on 2022/7/30.
//

#pragma once

#include <Common/Math/Constant.h>
#include <Common/Math/Matrix3.h>
#include <Common/Math/Matrix4.h>
#include <Common/Math/Vector2.h>
#include <Common/Math/Vector3.h>
#include <Common/Math/Vector4.h>

namespace Common {
    inline Vector2 Min(const Vector2 &lhs, const Vector2 &rhs);
    inline Vector3 Min(const Vector3 &lhs, const Vector3 &rhs);
    inline Vector4 Min(const Vector4 &lhs, const Vector4 &rhs);

    inline Vector2 Max(const Vector2 &lhs, const Vector2 &rhs);
    inline Vector3 Max(const Vector3 &lhs, const Vector3 &rhs);
    inline Vector4 Max(const Vector4 &lhs, const Vector4 &rhs);

    inline float ToRadian(float value)
    {
        return value / 180.f * PI;
    }

    inline Vector4 Cast(const Vector3 &vec)
    {
        return {vec.x, vec.y, vec.z, 0.f};
    }

    inline Matrix4 Cast(const Matrix3 &mat)
    {
        Matrix4 ret;
        ret.m[0] = Cast(mat[0]);
        ret.m[1] = Cast(mat[1]);
        ret.m[2] = Cast(mat[2]);
        ret.m[3] = Vector4(0, 0, 0, 1);
        return ret;
    }

    inline Matrix4 MakePerspective(float fovy, float aspect, float near, float far)
    {
        float const inverseHalfTan = cos(0.5f * fovy) / sin(0.5f * fovy);

        Matrix4 ret;
        ret[0][0] = inverseHalfTan / aspect;
        ret[1][1] = inverseHalfTan;
        ret[2][2] = -(far + near) / (far - near);
        ret[2][3] = -1.f;
        ret[3][2] = -(2.f * far * near) / (far - near);
        return ret;
    }

    inline Matrix4 MakeOrthogonal(float left, float right, float top, float bottom, float near, float far)
    {
        Matrix4 ret;
        ret[0][0] = 2.f / (right - left);
        ret[1][1] = 2.f / (top - bottom);
        ret[2][2] = - 2.f / (far - near);
        ret[3][0] = - (right + left) / (right - left);
        ret[3][1] = - (top + bottom) / (top - bottom);
        ret[3][2] = - (far + near) / (far - near);
        return ret;
    }

    inline Vector2 Min(const Vector2 &lhs, const Vector2 &rhs)
    {
        Vector2 ret;
        ret.x = std::fmin(lhs.x, rhs.x);
        ret.y = std::fmin(lhs.y, rhs.y);
        return ret;
    }

    inline Vector3 Min(const Vector3 &lhs, const Vector3 &rhs)
    {
        Vector3 ret;
        ret.x = std::fmin(lhs.x, rhs.x);
        ret.y = std::fmin(lhs.y, rhs.y);
        ret.z = std::fmin(lhs.z, rhs.z);
        return ret;
    }

    inline Vector4 Min(const Vector4 &lhs, const Vector4 &rhs)
    {
        Vector4 ret;
        ret.x = std::fmin(lhs.x, rhs.x);
        ret.y = std::fmin(lhs.y, rhs.y);
        ret.z = std::fmin(lhs.z, rhs.z);
        ret.w = std::fmin(lhs.w, rhs.w);
        return ret;
    }

    inline Vector2 Max(const Vector2 &lhs, const Vector2 &rhs)
    {
        Vector2 ret;
        ret.x = std::fmax(lhs.x, rhs.x);
        ret.y = std::fmax(lhs.y, rhs.y);
        return ret;
    }

    inline Vector3 Max(const Vector3 &lhs, const Vector3 &rhs)
    {
        Vector3 ret;
        ret.x = std::fmax(lhs.x, rhs.x);
        ret.y = std::fmax(lhs.y, rhs.y);
        ret.z = std::fmax(lhs.z, rhs.z);
        return ret;
    }

    inline Vector4 Max(const Vector4 &lhs, const Vector4 &rhs)
    {
        Vector4 ret;
        ret.x = std::fmax(lhs.x, rhs.x);
        ret.y = std::fmax(lhs.y, rhs.y);
        ret.z = std::fmax(lhs.z, rhs.z);
        ret.w = std::fmax(lhs.w, rhs.w);
        return ret;
    }
} // namespace Common
