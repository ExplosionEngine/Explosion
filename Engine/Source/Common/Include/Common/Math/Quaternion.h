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
    struct EulerRotationBase {
        // store as angle
        T x;
        T y;
        T z;
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

    template <typename T>
    struct EulerRotation : public EulerRotationBase<T> {
        inline EulerRotation();
        inline EulerRotation(T inX, T inY, T inZ);
        inline EulerRotation(const Radian<T>& inX, const Radian<T>& inY, const Radian<T>& inZ);
        inline EulerRotation(const EulerRotation& inValue);
        inline EulerRotation& operator=(const EulerRotation& inValue);
    };

    template <typename T>
    struct Quaternion : public QuaternionBase<T> {
        static inline Quaternion FromEulerXYZ(const EulerRotation<T>& inEuler);
        static inline Quaternion FromEulerZYX(const EulerRotation<T>& inEuler);

        inline Quaternion();
        inline Quaternion(T inX, T inY, T inZ, T inW);
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

        inline T Model() const;
        inline Quaternion Negatived() const;
        inline Quaternion Conjugated() const;
        inline Quaternion Normalized() const;
        inline T Dot(const Quaternion& rhs) const;
        inline Vector<T, 3> RotateVector(const Vector<T, 3>& inVector) const;
        inline Matrix<T, 4, 4> GetRotationMatrix() const;
    };

    template <typename T>
    struct QuatConsts {
        static const Quaternion<T> zero;
        static const Quaternion<T> identity;
    };

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
    EulerRotation<T>::EulerRotation()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    template <typename T>
    EulerRotation<T>::EulerRotation(T inX, T inY, T inZ)
    {
        this->x = inX;
        this->y = inY;
        this->z = inZ;
    }

    template <typename T>
    EulerRotation<T>::EulerRotation(const Radian<T>& inX, const Radian<T>& inY, const Radian<T>& inZ)
        : EulerRotation(inX.ToAngle(), inY.ToAngle(), inZ.ToAngle())
    {
    }

    template <typename T>
    EulerRotation<T>::EulerRotation(const EulerRotation& inValue)
        : EulerRotation(inValue.x, inValue.y, inValue.z)
    {
    }

    template <typename T>
    EulerRotation<T>& EulerRotation<T>::operator=(const EulerRotation& inValue)
    {
        this->x = inValue.x;
        this->y = inValue.y;
        this->z = inValue.z;
        return *this;
    }

    template <typename T>
    const Quaternion<T> zero = Quaternion<T>();

    template <typename T>
    const Quaternion<T> identity = Quaternion<T>(0, 0, 0, 1);

    template <typename T>
    Quaternion<T> Quaternion<T>::FromEulerXYZ(const EulerRotation<T>& inEuler)
    {
        // TODO
        return Quaternion();
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::FromEulerZYX(const EulerRotation<T>& inEuler)
    {
        // TODO
        return Quaternion();
    }

    template <typename T>
    Quaternion<T>::Quaternion()
    {
        this->x = 0;
        this->y = 0;
        this->z = 0;
        this->w = 0;
    }

    template <typename T>
    Quaternion<T>::Quaternion(T inX, T inY, T inZ, T inW)
    {
        this->x = inX;
        this->y = inY;
        this->z = inZ;
        this->w = inW;
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Vector<T, 3>& inAxis, float inAngle)
    {
        Vector<T, 3> axis = inAxis.Normalized();
        T halfRadian = Angle<T>(inAngle).ToRadian() / 2.0f;
        T halfRadianSin = std::sin(halfRadian);
        T halfRadianCos = std::cos(halfRadian);

        this->x = axis.x * halfRadianSin;
        this->y = axis.y * halfRadianSin;
        this->z = axis.z * halfRadianSin;
        this->w = halfRadianCos;
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Vector<T, 3>& inAxis, const Radian<T>& inRadian)
        : Quaternion(inAxis, inRadian.ToAngle())
    {
    }

    template <typename T>
    Quaternion<T>::Quaternion(const Quaternion& inValue)
        : Quaternion(inValue.x, inValue.y, inValue.z, inValue.w)
    {
    }

    template <typename T>
    Quaternion<T>::Quaternion(Quaternion&& inValue) noexcept
        : Quaternion(inValue.x, inValue.y, inValue.z, inValue.w)
    {
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator=(const Quaternion& inValue)
    {
        this->x = inValue.x;
        this->y = inValue.y;
        this->z = inValue.z;
        this->w = inValue.w;
    }

    template <typename T>
    bool Quaternion<T>::operator==(const Quaternion& rhs) const
    {
        return CompareNumber(this->x, rhs.x)
            && CompareNumber(this->y, rhs.y)
            && CompareNumber(this->z, rhs.z)
            && CompareNumber(this->w, rhs.w);
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
        result.x = this->x + rhs.x;
        result.y = this->y + rhs.y;
        result.z = this->z + rhs.z;
        result.w = this->w + rhs.w;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator-(const Quaternion& rhs) const
    {
        Quaternion<T> result;
        result.x = this->x - rhs.x;
        result.y = this->y - rhs.y;
        result.z = this->z - rhs.z;
        result.w = this->w - rhs.w;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator*(T rhs) const
    {
        Quaternion<T> result;
        result.x = this->x * rhs;
        result.y = this->y * rhs;
        result.z = this->z * rhs;
        result.w = this->w * rhs;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
    {
        Quaternion<T> result;
        result.x = this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z;
        result.y = this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y;
        result.z = this->w * rhs.y - this->x * rhs.z + this->y * rhs.w + this->z * rhs.x;
        result.w = this->w * rhs.z + this->x * rhs.y - this->y * rhs.x + this->z * rhs.w;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator/(T rhs) const
    {
        Quaternion<T> result;
        result.x = this->x / rhs;
        result.y = this->y / rhs;
        result.z = this->z / rhs;
        result.w = this->w / rhs;
        return result;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        this->w += rhs.w;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        this->w -= rhs.w;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(T rhs) const
    {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        this->w *= rhs;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs)
    {
        T x = this->x;
        T y = this->y;
        T z = this->z;
        T w = this->w;

        this->x = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
        this->y = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
        this->z = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
        this->w = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
        return *this;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator/=(T rhs)
    {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        this->w /= rhs;
        return *this;
    }

    template <typename T>
    T Quaternion<T>::Model() const
    {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::Negatived() const
    {
        Quaternion<T> result;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        result.w = -this->w;
        return result;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::Conjugated() const
    {
        Quaternion<T> result;
        result.x = -this->x;
        result.y = -this->y;
        result.z = -this->z;
        result.w = this->w;
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
        return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z + this->w * rhs.w;
    }

    template <typename T>
    Vector<T, 3> Quaternion<T>::RotateVector(const Vector<T, 3>& inVector) const
    {
        Quaternion pure = Quaternion(inVector.x, inVector.y, inVector.z, 0);
        return this->operator*(pure) * Conjugated();
    }

    template <typename T>
    Matrix<T, 4, 4> Quaternion<T>::GetRotationMatrix() const
    {
        // TODO
        return Matrix<T, 4, 4>();
    }
}
