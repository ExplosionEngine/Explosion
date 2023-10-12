//
// Created by johnk on 2023/6/4.
//

#pragma once

#include <Common/Math/Half.h>
#include <Common/Math/Matrix.h>

namespace Common {
    template <typename T> struct Angle;
    template <typename T> struct Radian;

    template <typename T>
    requires isFloatingPointV<T>
    struct AngleBase {
        T value;
    };

    template <typename T>
    requires isFloatingPointV<T>
    struct RadianBase {
        T value;
    };

    template <typename T>
    requires isFloatingPointV<T>
    struct QuaternionBase {
        T x;
        T y;
        T z;
        T w;
    };

    template <typename T>
    struct Angle : public AngleBase<T> {
        inline Angle();
        inline explicit Angle(T inValue);
        inline explicit Angle(const Radian<T>& inValue);
        inline Angle(const Angle& inValue);
        inline Angle(Angle&& inValue) noexcept;
        inline Angle& operator=(const Angle& inValue);
        inline Angle& operator=(const Radian<T>& inValue);
        inline T ToRadian() const;
    };

    template <typename T>
    struct Radian : public RadianBase<T> {
        inline Radian();
        inline explicit Radian(T inValue);
        inline explicit Radian(const Angle<T>& inValue);
        inline Radian(const Radian& inValue);
        inline Radian(Radian&& inValue) noexcept;
        inline Radian& operator=(const Radian& inValue);
        inline Radian& operator=(const Angle<T>& inValue);
        inline T ToAngle() const;
    };

    // left-hand coordinates system
    // +x -> from screen outer to inner
    // +y -> from left to right
    // +z -> from bttom to up
    template <typename T>
    struct Quaternion : public QuaternionBase<T> {
        static inline Quaternion FromEulerZYX(T inAngleX, T inAngleY, T inAngleZ);
        static inline Quaternion FromEulerZYX(const Radian<T>& inRadianX, const Radian<T>& inRadianY, const Radian<T>& inRadianZ);

        inline Quaternion();
        inline Quaternion(T inW, T inX, T inY, T inZ);
        inline Quaternion(const Vector<T, 3>& inAxis, float inAngle);
        inline Quaternion(const Vector<T, 3>& inAxis, const Radian<T>& inRadian);
        inline Quaternion(const Quaternion& inValue);
        inline Quaternion(Quaternion&& inValue) noexcept;
        inline Quaternion& operator=(const Quaternion& inValue);

        inline bool operator==(const Quaternion& rhs) const;
        inline bool operator!=(const Quaternion& rhs) const;
        inline Quaternion operator+(const Quaternion& rhs) const;
        inline Quaternion operator-(const Quaternion& rhs) const;
        inline Quaternion operator*(T rhs) const;
        inline Quaternion operator*(const Quaternion& rhs) const;
        inline Quaternion operator/(T rhs) const;

        inline Quaternion& operator+=(const Quaternion& rhs);
        inline Quaternion& operator-=(const Quaternion& rhs);
        inline Quaternion& operator*=(T rhs) const;
        inline Quaternion& operator*=(const Quaternion& rhs);
        inline Quaternion& operator/=(T rhs);

        inline Vector<T, 3> ImaginaryPart() const;
        inline T Model() const;
        inline Quaternion Negatived() const;
        inline Quaternion Conjugated() const;
        inline Quaternion Normalized() const;
        inline T Dot(const Quaternion& rhs) const;
        // when axis faced to us, ccw as positive direction
        inline Vector<T, 3> RotateVector(const Vector<T, 3>& inVector) const;
        inline Matrix<T, 4, 4> GetRotationMatrix() const;
    };

    template <typename T>
    struct QuatConsts {
        static const Quaternion<T> zero;
        static const Quaternion<T> identity;
    };

    using HAngle = Angle<HFloat>;
    using FAngle = Angle<float>;
    using DAngle = Angle<double>;

    using HRadian = Radian<HFloat>;
    using FRadian = Radian<float>;
    using DRadian = Radian<double>;

    using HQuat = Quaternion<HFloat>;
    using FQuat = Quaternion<float>;
    using DQuat = Quaternion<double>;

    using HQuatConsts = QuatConsts<HFloat>;
    using FQuatConsts = QuatConsts<float>;
    using DQuatConsts = QuatConsts<double>;
}

namespace Common {
    template <typename T>
    Angle<T>::Angle()
    {
        this->value = 0;
    }

    template <typename T>
    Angle<T>::Angle(T inValue)
    {
        this->value = inValue;
    }

    template <typename T>
    Angle<T>::Angle(const Radian<T>& inValue) : Angle<T>(inValue.ToAngle()) {}

    template <typename T>
    Angle<T>::Angle(const Angle& inValue) : Angle<T>(inValue.value) {}

    template <typename T>
    Angle<T>::Angle(Angle&& inValue) noexcept : Angle<T>(inValue.value) {}

    template <typename T>
    Angle<T>& Angle<T>::operator=(const Angle& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    Angle<T>& Angle<T>::operator=(const Radian<T>& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    T Angle<T>::ToRadian() const
    {
        return this->value / 180.0f * pi;
    }

    template <typename T>
    Radian<T>::Radian()
    {
        this->value = 0;
    }

    template <typename T>
    Radian<T>::Radian(T inValue)
    {
        this->value = inValue;
    }

    template <typename T>
    Radian<T>::Radian(const Angle<T>& inValue) : Radian<T>(inValue.ToRadian()) {}

    template <typename T>
    Radian<T>::Radian(const Radian& inValue) : Radian(inValue.value) {}

    template <typename T>
    Radian<T>::Radian(Radian&& inValue) noexcept : Radian(inValue.value) {}

    template <typename T>
    Radian<T>& Radian<T>::operator=(const Radian& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    Radian<T>& Radian<T>::operator=(const Angle<T>& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <typename T>
    T Radian<T>::ToAngle() const
    {
        return this->value * 180.0f / pi;
    }

    template <typename T>
    const Quaternion<T> QuatConsts<T>::zero = Quaternion<T>();

    template <typename T>
    const Quaternion<T> QuatConsts<T>::identity = Quaternion<T>(1, 0, 0, 0);

    template <typename T>
    Quaternion<T> Quaternion<T>::FromEulerZYX(T inAngleX, T inAngleY, T inAngleZ)
    {
        return Quaternion<T>(VecConsts<T, 3>::unitZ, inAngleZ)
            * Quaternion<T>(VecConsts<T, 3>::unitY, inAngleY)
            * Quaternion<T>(VecConsts<T, 3>::unitX, inAngleX);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::FromEulerZYX(const Radian<T>& inRadianX, const Radian<T>& inRadianY, const Radian<T>& inRadianZ)
    {
        return FromEulerZYX(inRadianX.ToAngle(), inRadianY.ToAngle(), inRadianZ.ToAngle());
    }

    template <typename T>
    Quaternion<T>::Quaternion()
    {
        this->w = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    template <typename T>
    Quaternion<T>::Quaternion(T inW, T inX, T inY, T inZ)
    {
        this->w = inW;
        this->x = inX;
        this->y = inY;
        this->z = inZ;
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Vector<T, 3>& inAxis, float inAngle)
    {
        Vector<T, 3> axis = inAxis.Normalized();
        T halfRadian = Angle<T>(inAngle).ToRadian() / 2.0f;
        T halfRadianSin = std::sin(halfRadian);
        T halfRadianCos = std::cos(halfRadian);

        this->w = halfRadianCos;
        this->x = axis.x * halfRadianSin;
        this->y = axis.y * halfRadianSin;
        this->z = axis.z * halfRadianSin;
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Vector<T, 3>& inAxis, const Radian<T>& inRadian)
        : Quaternion(inAxis, inRadian.ToAngle())
    {
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Quaternion& inValue)
        : Quaternion(inValue.w, inValue.x, inValue.y, inValue.z)
    {
    }

    template <typename T>
    Quaternion<T>::Quaternion(Quaternion&& inValue) noexcept
        : Quaternion(inValue.w, inValue.x, inValue.y, inValue.z)
    {
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator=(const Quaternion& inValue)
    {
        this->w = inValue.w;
        this->x = inValue.x;
        this->y = inValue.y;
        this->z = inValue.z;
        return *this;
    }

    template <typename T>
    bool Quaternion<T>::operator==(const Quaternion& rhs) const
    {
        return CompareNumber(this->w, rhs.w)
            && CompareNumber(this->x, rhs.x)
            && CompareNumber(this->y, rhs.y)
            && CompareNumber(this->z, rhs.z);
    }

    template <typename T>
    bool Quaternion<T>::operator!=(const Quaternion& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator+(const Quaternion& rhs) const
    {
        Quaternion<T> result;
        result.w = this->w + rhs.w;
        result.x = this->x + rhs.x;
        result.y = this->y + rhs.y;
        result.z = this->z + rhs.z;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator-(const Quaternion& rhs) const
    {
        Quaternion<T> result;
        result.w = this->w - rhs.w;
        result.x = this->x - rhs.x;
        result.y = this->y - rhs.y;
        result.z = this->z - rhs.z;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator*(T rhs) const
    {
        Quaternion<T> result;
        result.w = this->w * rhs;
        result.x = this->x * rhs;
        result.y = this->y * rhs;
        result.z = this->z * rhs;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
    {
        Quaternion<T> result;
        result.w = this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z;
        result.x = this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y;
        result.y = this->w * rhs.y - this->x * rhs.z + this->y * rhs.w + this->z * rhs.x;
        result.z = this->w * rhs.z + this->x * rhs.y - this->y * rhs.x + this->z * rhs.w;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator/(T rhs) const
    {
        Quaternion<T> result;
        result.w = this->w / rhs;
        result.x = this->x / rhs;
        result.y = this->y / rhs;
        result.z = this->z / rhs;
        return result;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& rhs)
    {
        this->w += rhs.w;
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& rhs)
    {
        this->w -= rhs.w;
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(T rhs) const
    {
        this->w *= rhs;
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs)
    {
        *this = this * rhs;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator/=(T rhs)
    {
        this->w /= rhs;
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        return *this;
    }

    template <typename T>
    Vector<T, 3> Quaternion<T>::ImaginaryPart() const
    {
        return Vector<T, 3>(this->x, this->y, this->z);
    }

    template <typename T>
    T Quaternion<T>::Model() const
    {
        return std::sqrt(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::Negatived() const
    {
        Quaternion<T> result;
        result.w = -this->w;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::Conjugated() const
    {
        Quaternion<T> result;
        result.w = this->w;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::Normalized() const
    {
        return this->operator/(Model());
    }

    template <typename T>
    T Quaternion<T>::Dot(const Quaternion& rhs) const
    {
        return this->w * rhs.w * this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
    }

    template <typename T>
    Vector<T, 3> Quaternion<T>::RotateVector(const Vector<T, 3>& inVector) const
    {
        Quaternion<T> v = Quaternion(0, inVector.x, inVector.y, inVector.z);
        Quaternion<T> v2 = Conjugated() * v * (*this);
        return Vector<T, 3>(v2.x, v2.y, v2.z);
    }

    template <typename T>
    Matrix<T, 4, 4> Quaternion<T>::GetRotationMatrix() const
    {
        T xx2 = this->x * this->x * 2;
        T yy2 = this->y * this->y * 2;
        T zz2 = this->z * this->z * 2;

        T wx2 = this->w * this->x * 2;
        T wy2 = this->w * this->y * 2;
        T wz2 = this->w * this->z * 2;
        T xy2 = this->x * this->y * 2;
        T xz2 = this->x * this->z * 2;
        T yz2 = this->y * this->z * 2;

        return Matrix<T, 4, 4>(
            1 - yy2 - zz2, xy2 - wz2, xz2 + wy2, 0,
            xy2 + wz2, 1 - xx2 - zz2, yz2 - wx2, 0,
            xz2 - wy2, yz2 + wx2, 1 - xx2 - yy2, 0,
            0, 0, 0, 1
        );
    }

}
