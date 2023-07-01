//
// Created by johnk on 2023/6/21.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>

namespace Common {
    template <typename T>
    struct TransformBase {
        Vector<T, 3> scale;
        Quaternion<T> rotation;
        Vector<T, 3> translation;
    };

    template <typename T>
    struct Transform : public TransformBase<T> {
        inline Transform();
        inline Transform(Quaternion<T> inRotation, Vector<T, 3> inTranslation);
        inline Transform(Vector<T, 3> inScale, Quaternion<T> inRotation, Vector<T, 3> inTranslation);
        inline Transform(const Transform& other);
        inline Transform(Transform&& other) noexcept;
        inline Transform& operator=(const Transform& other);

        inline bool operator==(const Transform& rhs) const;
        inline bool operator!=(const Transform& rhs) const;

        inline Transform operator+(const Vector<T, 3>& inTranslation) const;
        inline Transform operator|(const Quaternion<T>& inRotation) const;
        inline Transform operator*(const Vector<T, 3>& inScale) const;

        inline Transform& operator+=(const Vector<T, 3>& inTranslation);
        inline Transform& operator|=(const Quaternion<T>& inRotation);
        inline Transform& operator*=(const Vector<T, 3>& inScale);

        inline Transform& Translate(const Vector<T, 3>& inTranslation);
        inline Transform& Rotate(const Quaternion<T>& inRotation);
        inline Transform& Scale(const Vector<T, 3>& inScale);

        Matrix<T, 4, 4> GetTranslationMatrix() const;
        Matrix<T, 4, 4> GetRotationMatrix() const;
        Matrix<T, 4, 4> GetScaleMatrix() const;
        // scale -> rotate -> translate
        Matrix<T, 4, 4> GetTransformMatrix() const;
        // rotate -> translate
        Matrix<T, 4, 4> GetTransformMatrixNoScale() const;
        Vector<T, 3> TransformPosition(const Vector<T, 3>& inPosition) const;
        Vector<T, 4> TransformPosition(const Vector<T, 4>& inPosition) const;

        template <typename IT>
        Transform<IT> CastTo() const;
    };

    using HTransform = Transform<HFloat>;
    using FTransform = Transform<float>;
    using DTransform = Transform<double>;
}

namespace Common {
    template <typename T>
    Transform<T>::Transform()
    {
        this->scale = VecConsts<T, 3>::unit;
        this->rotation = QuatConsts<T>::identity;
        this->translation = VecConsts<T, 3>::zero;
    }

    template <typename T>
    Transform<T>::Transform(Quaternion<T> inRotation, Vector<T, 3> inTranslation)
    {
        this->scale = VecConsts<T, 3>::unit;
        this->rotation = inRotation;
        this->translation = inTranslation;
    }

    template <typename T>
    Transform<T>::Transform(Vector<T, 3> inScale, Quaternion<T> inRotation, Vector<T, 3> inTranslation)
    {
        this->scale = inScale;
        this->rotation = inRotation;
        this->translation = inTranslation;
    }

    template <typename T>
    Transform<T>::Transform(const Transform& other)
    {
        this->scale = other.scale;
        this->rotation = other.rotation;
        this->translation = other.translation;
    }

    template <typename T>
    Transform<T>::Transform(Transform&& other) noexcept
    {
        this->scale = std::move(other.scale);
        this->rotation = std::move(other.rotation);
        this->translation = std::move(other.translation);
    }

    template <typename T>
    Transform<T>& Transform<T>::operator=(const Transform& other)
    {
        this->scale = other.scale;
        this->rotation = other.rotation;
        this->translation = other.translation;
        return *this;
    }

    template <typename T>
    bool Transform<T>::operator==(const Transform& rhs) const
    {
        return this->scale == rhs.scale
            && this->rotation == rhs.rotation
            && this->translation == rhs.translation;
    }

    template <typename T>
    bool Transform<T>::operator!=(const Transform& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T>
    Transform<T> Transform<T>::operator+(const Vector<T, 3>& inTranslation) const
    {
        Transform<T> result;
        result.scale = this->scale;
        result.rotation = this->rotation;
        result.translation = this->translation + inTranslation;
        return result;
    }

    template <typename T>
    Transform<T> Transform<T>::operator|(const Quaternion<T>& inRotation) const
    {
        Transform<T> result;
        result.scale = this->scale;
        result.rotation = this->rotation * inRotation;
        result.translation = this->translation;
        return result;
    }

    template <typename T>
    Transform<T> Transform<T>::operator*(const Vector<T, 3>& inScale) const
    {
        Transform<T> result;
        result.scale = this->scale * inScale;
        result.rotation = this->rotation;
        result.translation = this->translation;
        return result;
    }

    template <typename T>
    Transform<T>& Transform<T>::operator+=(const Vector<T, 3>& inTranslation)
    {
        return Translate(inTranslation);
    }

    template <typename T>
    Transform<T>& Transform<T>::operator|=(const Quaternion<T>& inRotation)
    {
        return Rotate(inRotation);
    }

    template <typename T>
    Transform<T>& Transform<T>::operator*=(const Vector<T, 3>& inScale)
    {
        return Scale(inScale);
    }

    template <typename T>
    Transform<T>& Transform<T>::Translate(const Vector<T, 3>& inTranslation)
    {
        this->translation += inTranslation;
        return *this;
    }

    template <typename T>
    Transform<T>& Transform<T>::Rotate(const Quaternion<T>& inRotation)
    {
        this->rotation *= inRotation;
        return *this;
    }

    template <typename T>
    Transform<T>& Transform<T>::Scale(const Vector<T, 3>& inScale)
    {
        this->scale *= inScale;
        return *this;
    }

    template <typename T>
    Matrix<T, 4, 4> Transform<T>::GetTranslationMatrix() const
    {
        Matrix<T, 4, 4> result = MatConsts<T, 4, 4>::identity;
        result.SetCol(3, this->translation.x, this->translation.y, this->translation.z, 1);
        return result;
    }

    template <typename T>
    Matrix<T, 4, 4> Transform<T>::GetRotationMatrix() const
    {
        return this->rotation.GetRotationMatrix();
    }

    template <typename T>
    Matrix<T, 4, 4> Transform<T>::GetScaleMatrix() const
    {
        Matrix<T, 4, 4> result = MatConsts<T, 4, 4>::identity;
        result.At(0, 0) = this->scale.x;
        result.At(1, 1) = this->scale.y;
        result.At(2, 2) = this->scale.z;
        return result;
    }

    template <typename T>
    Matrix<T, 4, 4> Transform<T>::GetTransformMatrix() const
    {
        return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
    }

    template <typename T>
    Matrix<T, 4, 4> Transform<T>::GetTransformMatrixNoScale() const
    {
        return GetTranslationMatrix() * GetRotationMatrix();
    }

    template <typename T>
    Vector<T, 3> Transform<T>::TransformPosition(const Vector<T, 3>& inPosition) const
    {
        Matrix<T, 4, 1> posColMat = Matrix<T, 4, 1>::FromColVecs(Vector<T, 4>(inPosition.x, inPosition.y, inPosition.z, 1));
        return (GetTransformMatrix() * posColMat).Col(0).template SubVec<0, 1, 2>();
    }

    template <typename T>
    Vector<T, 4> Transform<T>::TransformPosition(const Vector<T, 4>& inPosition) const
    {
        Matrix<T, 4, 1> posColMat = Matrix<T, 4, 1>::FromColVecs(inPosition);
        return (GetTransformMatrix() * posColMat).Col(0);
    }

    template <typename T>
    template <typename IT>
    Transform<IT> Transform<T>::CastTo() const
    {
        Transform<IT> result;
        result.translation = this->translation.template CastTo<IT>();
        result.rotation = this->rotation.template CastTo<IT>();
        result.scale = this->scale.template CastTo<IT>();
        return result;
    }
}
