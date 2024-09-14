//
// Created by johnk on 2023/6/21.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <FloatingPoint T>
    struct TransformBase {
        Vec<T, 3> scale;
        Quaternion<T> rotation;
        Vec<T, 3> translation;
    };

    template <typename T>
    struct Transform : TransformBase<T> {
        static Transform LookAt(const Vec<T, 3>& inPosition, const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection = VecConsts<T, 3>::unitZ);

        Transform();
        Transform(Quaternion<T> inRotation, Vec<T, 3> inTranslation);
        Transform(Vec<T, 3> inScale, Quaternion<T> inRotation, Vec<T, 3> inTranslation);
        Transform(const Transform& other);
        Transform(Transform&& other) noexcept;
        Transform& operator=(const Transform& other);

        bool operator==(const Transform& rhs) const;
        bool operator!=(const Transform& rhs) const;

        Transform operator+(const Vec<T, 3>& inTranslation) const;
        Transform operator|(const Quaternion<T>& inRotation) const;
        Transform operator*(const Vec<T, 3>& inScale) const;

        Transform& operator+=(const Vec<T, 3>& inTranslation);
        Transform& operator|=(const Quaternion<T>& inRotation);
        Transform& operator*=(const Vec<T, 3>& inScale);

        Transform& Translate(const Vec<T, 3>& inTranslation);
        Transform& Rotate(const Quaternion<T>& inRotation);
        Transform& Scale(const Vec<T, 3>& inScale);
        Transform& UpdateRotation(const Vec<T, 3>& forward, const Vec<T, 3>& side, const Vec<T, 3>& up);
        Transform& LookTo(const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection = VecConsts<T, 3>::unitZ);
        Transform& MoveAndLookTo(const Vec<T, 3>& inPosition, const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection = VecConsts<T, 3>::unitZ);

        Mat<T, 4, 4> GetTranslationMatrix() const;
        Mat<T, 4, 4> GetRotationMatrix() const;
        Mat<T, 4, 4> GetScaleMatrix() const;
        // scale -> rotate -> translate
        Mat<T, 4, 4> GetTransformMatrix() const;
        // rotate -> translate
        Mat<T, 4, 4> GetTransformMatrixNoScale() const;
        Vec<T, 3> TransformPosition(const Vec<T, 3>& inPosition) const;
        Vec<T, 4> TransformPosition(const Vec<T, 4>& inPosition) const;

        template <typename IT>
        Transform<IT> CastTo() const;
    };

    using HTransform = Transform<HFloat>;
    using FTransform = Transform<float>;
    using DTransform = Transform<double>;
}

namespace Common {
    template <Serializable T>
    struct Serializer<Transform<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Transform")
            + Serializer<T>::typeId;

        static size_t Serialize(SerializeStream& stream, const Transform<T>& value)
        {
            return Serializer<Vec<T, 3>>::Serialize(stream, value.scale)
                + Serializer<Quaternion<T>>::Serialize(stream, value.rotation)
                + Serializer<Vec<T, 3>>::Serialize(stream, value.translation);
        }

        static size_t Deserialize(DeserializeStream& stream, Transform<T>& value)
        {
            return Serializer<Vec<T, 3>>::Deserialize(stream, value.scale)
                + Serializer<Quaternion<T>>::Deserialize(stream, value.rotation)
                + Serializer<Vec<T, 3>>::Deserialize(stream, value.translation);
        }
    };

    template <StringConvertible T>
    struct StringConverter<Transform<T>> {
        static std::string ToString(const Transform<T>& inValue)
        {
            return fmt::format(
                "{scale={}, rotation={}, translation={}}",
                StringConverter<Vec<T, 3>>::ToString(inValue.scale),
                StringConverter<Quaternion<T>>::ToString(inValue.rotation),
                StringConverter<Vec<T, 3>>::ToString(inValue.translation));
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<Transform<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Transform<T>& inValue)
        {
            rapidjson::Value scaleJson;
            JsonSerializer<Vec<T, 3>>::JsonSerialize(scaleJson, inAllocator, inValue.scale);

            rapidjson::Value rotationJson;
            JsonSerializer<Quaternion<T>>::JsonSerialize(rotationJson, inAllocator, inValue.rotation);

            rapidjson::Value translationJson;
            JsonSerializer<Vec<T, 3>>::JsonSerialize(translationJson, inAllocator, inValue.translation);

            outJsonValue.SetObject();
            outJsonValue.AddMember("scale", scaleJson, inAllocator);
            outJsonValue.AddMember("rotation", rotationJson, inAllocator);
            outJsonValue.AddMember("translation", translationJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Transform<T>& outValue)
        {
            JsonSerializer<Vec<T, 3>>::JsonDeserialize(inJsonValue["scale"], outValue.scale);
            JsonSerializer<Quaternion<T>>::JsonDeserialize(inJsonValue["rotation"], outValue.rotation);
            JsonSerializer<Vec<T, 3>>::JsonDeserialize(inJsonValue["scale"], outValue.scale);
        }
    };
}

namespace Common {
    template <typename T>
    Transform<T> Transform<T>::LookAt(const Vec<T, 3>& inPosition, const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection)
    {
        Transform result;
        result.MoveAndLookTo(inPosition, inTargetPosition, inUpDirection);
        return result;
    }

    template <typename T>
    Transform<T>::Transform()
    {
        this->scale = VecConsts<T, 3>::unit;
        this->rotation = QuatConsts<T>::identity;
        this->translation = VecConsts<T, 3>::zero;
    }

    template <typename T>
    Transform<T>::Transform(Quaternion<T> inRotation, Vec<T, 3> inTranslation)
    {
        this->scale = VecConsts<T, 3>::unit;
        this->rotation = inRotation;
        this->translation = inTranslation;
    }

    template <typename T>
    Transform<T>::Transform(Vec<T, 3> inScale, Quaternion<T> inRotation, Vec<T, 3> inTranslation)
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
    Transform<T> Transform<T>::operator+(const Vec<T, 3>& inTranslation) const
    {
        Transform result;
        result.scale = this->scale;
        result.rotation = this->rotation;
        result.translation = this->translation + inTranslation;
        return result;
    }

    template <typename T>
    Transform<T> Transform<T>::operator|(const Quaternion<T>& inRotation) const
    {
        Transform result;
        result.scale = this->scale;
        result.rotation = this->rotation * inRotation;
        result.translation = this->translation;
        return result;
    }

    template <typename T>
    Transform<T> Transform<T>::operator*(const Vec<T, 3>& inScale) const
    {
        Transform result;
        result.scale = this->scale * inScale;
        result.rotation = this->rotation;
        result.translation = this->translation;
        return result;
    }

    template <typename T>
    Transform<T>& Transform<T>::operator+=(const Vec<T, 3>& inTranslation)
    {
        return Translate(inTranslation);
    }

    template <typename T>
    Transform<T>& Transform<T>::operator|=(const Quaternion<T>& inRotation)
    {
        return Rotate(inRotation);
    }

    template <typename T>
    Transform<T>& Transform<T>::operator*=(const Vec<T, 3>& inScale)
    {
        return Scale(inScale);
    }

    template <typename T>
    Transform<T>& Transform<T>::Translate(const Vec<T, 3>& inTranslation)
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
    Transform<T>& Transform<T>::Scale(const Vec<T, 3>& inScale)
    {
        this->scale *= inScale;
        return *this;
    }

    template <typename T>
    Transform<T>& Transform<T>::UpdateRotation(const Vec<T, 3>& forward, const Vec<T, 3>& side, const Vec<T, 3>& up)
    {
        // Transform of an object(camera) is the inverse of transform represented in lookAtMatrix
        // Mat<T, 4, 4> lookAtMat {
        //     s.x, s.y, s.z, -s.Dot(inPosition),
        //     u.x, u.y, u.z, -u.Dot(inPosition),
        //     f.x, f.y. f.z, -f.Dot(inPosition),
        //     0, 0, 0, 1
        // };

        // Rotaion Mat is orthogonal, its inverse equals to its transpose
        // So, we get quaternion from the transposed rotation part of lookAtMatrix
        // Algorithm: https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

        Mat<T, 3, 3> rotMat {
            side.x, up.x, forward.x,
            side.y, up.y, forward.y,
            side.z, up.z, forward.z
        };

        T trace = rotMat.At(0, 0) + rotMat.At(1, 1) + rotMat.At(2, 2);
        if (trace > static_cast<T>(0)) {
            T root = sqrt(trace + static_cast<T>(1.0));
            this->rotation.w = static_cast<T>(0.5) * root;

            root = static_cast<T>(0.5) / root;
            this->rotation.x = root * (rotMat.At(2, 1) - rotMat.At(1, 2));
            this->rotation.y = root * (rotMat.At(0, 2) - rotMat.At(2, 0));
            this->rotation.z = root * (rotMat.At(1, 0) - rotMat.At(0, 1));
        } else if (rotMat.At(0, 0) > rotMat.At(1, 1) && rotMat.At(0, 0) > rotMat.At(2, 2)) {
            T root = sqrt(rotMat.At(0, 0) - rotMat.At(1, 1) - rotMat.At(2, 2) + static_cast<T>(1.0));
            this->rotation.x = static_cast<T>(0.5) * root;

            root = static_cast<T>(0.5) / root;
            this->rotation.y = root * (rotMat.At(0, 1) + rotMat.At(1, 0));
            this->rotation.z = root * (rotMat.At(0, 2) + rotMat.At(2, 0));
            this->rotation.w = root * (rotMat.At(2, 1) - rotMat.At(1, 2));
        } else if (rotMat.At(1, 1) > rotMat.At(2, 2)) {
            T root = sqrt(rotMat.At(1, 1) - rotMat.At(0, 0) - rotMat.At(2, 2) + static_cast<T>(1.0));
            this->rotation.y = static_cast<T>(0.5) * root;

            root = static_cast<T>(0.5) / root;
            this->rotation.x = root * (rotMat.At(0, 1) + rotMat.At(1, 0));
            this->rotation.z = root * (rotMat.At(1, 2) + rotMat.At(2, 1));
            this->rotation.w = root * (rotMat.At(0, 2) - rotMat.At(2, 0));
        } else {
            T root = sqrt(rotMat.At(2, 2) - rotMat.At(0, 0) - rotMat.At(1, 1) + static_cast<T>(1.0));
            this->rotation.z = static_cast<T>(0.5) * root;

            root = static_cast<T>(0.5) / root;
            this->rotation.x = root * (rotMat.At(0, 2) + rotMat.At(2, 0));
            this->rotation.y = root * (rotMat.At(1, 2) + rotMat.At(2, 1));
            this->rotation.w = root * (rotMat.At(1, 0) - rotMat.At(0, 1));
        }

        return *this;
    }

    template <typename T>
    Transform<T>& Transform<T>::LookTo(const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection)
    {
        Vec<T, 3> f(inTargetPosition - this->translation);
        f.Normalize();

        Vec<T, 3> s = inUpDirection.Cross(f);
        s.Normalize();

        Vec<T, 3> u = f.Cross(s);

        this->UpdateRotation(f, s, u);

        return *this;
    }

    template <typename T>
    Transform<T>& Transform<T>::MoveAndLookTo(const Vec<T, 3>& inPosition, const Vec<T, 3>& inTargetPosition, const Vec<T, 3>& inUpDirection)
    {
        // Translation of LookAtMatrix ([s.Dot(inPosition), u.Dot(inPosition), f.Dot(inPosition)]) applying to visible objects is in camera space, whose coordinate system consists of s u and f
        // The translation we need is in world space, exactly inPosition
        this->translation = inPosition;

        Vec<T, 3> f(inTargetPosition - this->translation);
        f.Normalize();

        Vec<T, 3> s = inUpDirection.Cross(f);
        s.Normalize();

        Vec<T, 3> u = f.Cross(s);

        this->UpdateRotation(f, s, u);

        return *this;
    }

    template <typename T>
    Mat<T, 4, 4> Transform<T>::GetTranslationMatrix() const
    {
        Mat<T, 4, 4> result = MatConsts<T, 4, 4>::identity;
        result.SetCol(3, this->translation.x, this->translation.y, this->translation.z, 1);
        return result;
    }

    template <typename T>
    Mat<T, 4, 4> Transform<T>::GetRotationMatrix() const
    {
        return this->rotation.GetRotationMatrix();
    }

    template <typename T>
    Mat<T, 4, 4> Transform<T>::GetScaleMatrix() const
    {
        Mat<T, 4, 4> result = MatConsts<T, 4, 4>::identity;
        result.At(0, 0) = this->scale.x;
        result.At(1, 1) = this->scale.y;
        result.At(2, 2) = this->scale.z;
        return result;
    }

    template <typename T>
    Mat<T, 4, 4> Transform<T>::GetTransformMatrix() const
    {
        return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
    }

    template <typename T>
    Mat<T, 4, 4> Transform<T>::GetTransformMatrixNoScale() const
    {
        return GetTranslationMatrix() * GetRotationMatrix();
    }

    template <typename T>
    Vec<T, 3> Transform<T>::TransformPosition(const Vec<T, 3>& inPosition) const
    {
        Mat<T, 4, 1> posColMat = Mat<T, 4, 1>::FromColVecs(Vec<T, 4>(inPosition.x, inPosition.y, inPosition.z, 1));
        return (GetTransformMatrix() * posColMat).Col(0).template SubVec<0, 1, 2>();
    }

    template <typename T>
    Vec<T, 4> Transform<T>::TransformPosition(const Vec<T, 4>& inPosition) const
    {
        Mat<T, 4, 1> posColMat = Mat<T, 4, 1>::FromColVecs(inPosition);
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
