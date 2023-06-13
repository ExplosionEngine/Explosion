//
// Created by johnk on 2023/6/4.
//

#pragma once

#include <Common/Math/Half.h>

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
        Angle();
        explicit Angle(T inValue);
        explicit Angle(const Radian<T>& inValue);
        Angle(const Angle& inValue);
        Angle(Angle&& inValue) noexcept;
        Angle& operator=(const Angle& inValue);
        Angle& operator=(const Radian<T>& inValue);
        T ToRadian() const;
    };

    template <typename T>
    struct Radian : public RadianBase<T> {
        Radian();
        explicit Radian(T inValue);
        explicit Radian(const Angle<T>& inValue);
        Radian(const Radian& inValue);
        Radian(Radian&& inValue) noexcept;
        Radian& operator=(const Radian& inValue);
        Radian& operator=(const Angle<T>& inValue);
        T ToAngle() const;
    };

    template <typename T>
    struct EulerRotation : public EulerRotationBase<T> {
        EulerRotation();
        EulerRotation(T inX, T inY, T inZ);
        EulerRotation(const Radian<T>& inX, const Radian<T>& inY, const Radian<T>& inZ);
        EulerRotation(const EulerRotation& inValue);
        EulerRotation& operator=(const EulerRotation& inValue);
    };

    template <typename T>
    struct Quaternion : public QuaternionBase<T> {
        static Quaternion FromEulerXYZ(const EulerRotation<T>& inEuler);
        static Quaternion FromEulerZYX(const EulerRotation<T>& inEuler);

        Quaternion();
        Quaternion(T inX, T inY, T inZ, T inW);

        // TODO
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
}
