//
// Created by johnk on 2023/6/26.
//

#pragma once

#include <optional>

#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <FloatingPoint T>
    struct ReversedZOrthogonalProjectionBase {
        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
    };

    template <FloatingPoint T>
    struct ReversedZPerspectiveProjectionBase {
        T fov;
        T width;
        T height;
        T nearPlane;
        std::optional<T> farPlane;
    };

    template <typename T>
    struct ReversedZOrthogonalProjection : ReversedZOrthogonalProjectionBase<T> {
        ReversedZOrthogonalProjection();
        ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane);
        ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane);

        Mat<T, 4, 4> GetProjectionMatrix() const;
        bool operator==(const ReversedZOrthogonalProjection& inRhs) const;
    };

    template <typename T>
    struct ReversedZPerspectiveProjection : ReversedZPerspectiveProjectionBase<T> {
        ReversedZPerspectiveProjection();
        ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane);
        ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane);

        Mat<T, 4, 4> GetProjectionMatrix() const;
        bool operator==(const ReversedZPerspectiveProjection& inRhs) const;
    };

    using HReversedZOrthoProjection = ReversedZOrthogonalProjection<HFloat>;
    using FReversedZOrthoProjection = ReversedZOrthogonalProjection<float>;
    using DReversedZOrthoProjection = ReversedZOrthogonalProjection<double>;

    using HReversedZPerspectiveProjection = ReversedZPerspectiveProjection<HFloat>;
    using FReversedZPerspectiveProjection = ReversedZPerspectiveProjection<float>;
    using DReversedZPerspectiveProjection = ReversedZPerspectiveProjection<double>;
}

namespace Common {
    template <Serializable T>
    struct Serializer<ReversedZOrthogonalProjection<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::ReversedZOrthogonalProjection")
            + Serializer<T>::typeId;

        static size_t Serialize(SerializeStream& stream, const ReversedZOrthogonalProjection<T>& value)
        {
            return Serializer<T>::Serialize(stream, value.width)
                + Serializer<T>::Serialize(stream, value.height)
                + Serializer<T>::Serialize(stream, value.nearPlane)
                + Serializer<std::optional<T>>::Serialize(stream, value.farPlane);
        }

        static size_t Deserialize(DeserializeStream& stream, ReversedZOrthogonalProjection<T>& value)
        {
            return Serializer<T>::Deserialize(stream, value.width)
                + Serializer<T>::Deserialize(stream, value.height)
                + Serializer<T>::Deserialize(stream, value.nearPlane)
                + Serializer<std::optional<T>>::Deserialize(stream, value.farPlane);
        }
    };

    template <Serializable T>
    struct Serializer<ReversedZPerspectiveProjection<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::ReversedZPerspectiveProjection")
            + Serializer<T>::typeId;

        static size_t Serialize(SerializeStream& stream, const ReversedZPerspectiveProjection<T>& value)
        {
            return Serializer<T>::Serialize(stream, value.fov)
                + Serializer<T>::Serialize(stream, value.width)
                + Serializer<T>::Serialize(stream, value.height)
                + Serializer<T>::Serialize(stream, value.nearPlane)
                + Serializer<std::optional<T>>::Serialize(stream, value.farPlane);
        }

        static size_t Deserialize(DeserializeStream& stream, ReversedZPerspectiveProjection<T>& value)
        {
            return Serializer<T>::Deserialize(stream, value.fov)
                + Serializer<T>::Deserialize(stream, value.width)
                + Serializer<T>::Deserialize(stream, value.height)
                + Serializer<T>::Deserialize(stream, value.nearPlane)
                + Serializer<std::optional<T>>::Deserialize(stream, value.farPlane);
        }
    };

    template <StringConvertible T>
    struct StringConverter<ReversedZOrthogonalProjection<T>> {
        static std::string ToString(const ReversedZOrthogonalProjection<T>& inValue)
        {
            return fmt::format(
                "{width={}, height={}, near={}, far={}}",
                StringConverter<T>::ToString(inValue.width),
                StringConverter<T>::ToString(inValue.height),
                StringConverter<T>::ToString(inValue.nearPlane),
                StringConverter<std::optional<T>>::ToString(inValue.farPlane));
        }
    };

    template <StringConvertible T>
    struct StringConverter<ReversedZPerspectiveProjection<T>> {
        static std::string ToString(const ReversedZPerspectiveProjection<T>& inValue)
        {
            return fmt::format(
                "{fov={}, width={}, height={}, near={}, far={}}",
                StringConverter<T>::ToString(inValue.fov),
                StringConverter<T>::ToString(inValue.width),
                StringConverter<T>::ToString(inValue.height),
                StringConverter<T>::ToString(inValue.nearPlane),
                StringConverter<std::optional<T>>::ToString(inValue.farPlane));
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<ReversedZOrthogonalProjection<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const ReversedZOrthogonalProjection<T>& inValue)
        {
            rapidjson::Value widthJson;
            JsonSerializer<T>::JsonSerialize(widthJson, inAllocator, inValue.width);

            rapidjson::Value heightJson;
            JsonSerializer<T>::JsonSerialize(heightJson, inAllocator, inValue.height);

            rapidjson::Value nearJson;
            JsonSerializer<T>::JsonSerialize(nearJson, inAllocator, inValue.nearPlane);

            rapidjson::Value farJson;
            JsonSerializer<std::optional<T>>::JsonSerialize(farJson, inAllocator, inValue.farPlane);

            outJsonValue.SetObject();
            outJsonValue.AddMember("width", widthJson, inAllocator);
            outJsonValue.AddMember("height", heightJson, inAllocator);
            outJsonValue.AddMember("near", nearJson, inAllocator);
            outJsonValue.AddMember("far", farJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, ReversedZOrthogonalProjection<T>& outValue)
        {
            if (!inJsonValue.IsObject) {
                return;
            }
            if (inJsonValue.HasMember("width")) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue["width"], outValue.width);
            }
            if (inJsonValue.HasMember("height")) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue["height"], outValue.height);
            }
            if (inJsonValue.HasMember("near")) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue["near"], outValue.nearPlane);
            }
            if (inJsonValue.HasMember("far")) {
                JsonSerializer<std::optional<T>>::JsonDeserialize(inJsonValue["far"], outValue.farPlane);
            }
        }
    };
}

namespace Common {
    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection()
    {
        this->width = 0;
        this->height = 0;
        this->nearPlane = 0;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane)
    {
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZOrthogonalProjection<T>::ReversedZOrthogonalProjection(T inWidth, T inHeight, T inNearPlane, T inFarPlane)
    {
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = inFarPlane;
    }

    template <typename T>
    Mat<T, 4, 4> ReversedZOrthogonalProjection<T>::GetProjectionMatrix() const
    {
        if (this->farPlane.has_value()) {
            return Mat<T, 4, 4>(
                2.0f / this->width, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / this->height, 0.0f, 0.0f,
                0.0f, 0.0f, -1.0f / (this->farPlane.value() - this->nearPlane), 1.0f + (this->nearPlane / (this->farPlane.value() - this->nearPlane)),
                0.0f, 0.0f, 0.0f, 1.0f
            );
        } else {
            // Infinite Far Plane
            return Mat<T, 4, 4>(
                2.0f / this->width, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / this->height, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
        }
    }

    template <typename T>
    bool ReversedZOrthogonalProjection<T>::operator==(const ReversedZOrthogonalProjection& inRhs) const
    {
        const auto thisHasFar = this->farPlane.has_value();
        const auto rhsHasFar = inRhs.farPlane.has_value();
        if (thisHasFar && !rhsHasFar || !thisHasFar && rhsHasFar) {
            return false;
        }
        if (thisHasFar && !CompareNumber(this->farPlane.value(), inRhs.farPlane.value())) {
            return false;
        }

        return CompareNumber(this->width, inRhs.width)
            && CompareNumber(this->height, inRhs.height)
            && CompareNumber(this->nearPlane, inRhs.nearPlane);
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection()
    {
        this->fov = 0.0f;
        this->width = 0.0f;
        this->height = 0.0f;
        this->nearPlane = 0;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane)
    {
        this->fov = inFOV;
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = {};
    }

    template <typename T>
    ReversedZPerspectiveProjection<T>::ReversedZPerspectiveProjection(T inFOV, T inWidth, T inHeight, T inNearPlane, T inFarPlane)
    {
        this->fov = inFOV;
        this->width = inWidth;
        this->height = inHeight;
        this->nearPlane = inNearPlane;
        this->farPlane = inFarPlane;
    }

    template <typename T>
    Mat<T, 4, 4> ReversedZPerspectiveProjection<T>::GetProjectionMatrix() const
    {
        Angle<T> angle(this->fov);
        T tanHalfFov = tan(angle.ToRadian() / static_cast<T>(2));

        if (this->farPlane.has_value()) {
            return Mat<T, 4, 4>(
                this->height / (this->width * tanHalfFov), 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
                0.0f, 0.0f, this->nearPlane / (this->nearPlane - this->farPlane.value()), this->nearPlane * this->farPlane.value() / (this->farPlane.value() - this->nearPlane),
                0.0f, 0.0f, 1.0f, 0.0f
            );
        }

        // Infinite Far Plane
        return Mat<T, 4, 4>(
            this->height / (this->width * tanHalfFov), 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, this->nearPlane,
            0.0f, 0.0f, 1.0f, 0.0f
        );
    }

    template <typename T>
    bool ReversedZPerspectiveProjection<T>::operator==(const ReversedZPerspectiveProjection& inRhs) const
    {
        const auto thisHasFar = this->farPlane.has_value();
        const auto rhsHasFar = inRhs.farPlane.has_value();
        if (thisHasFar && !rhsHasFar || !thisHasFar && rhsHasFar) {
            return false;
        }
        if (thisHasFar && !CompareNumber(this->farPlane.value(), inRhs.farPlane.value())) {
            return false;
        }

        return CompareNumber(this->fov, inRhs.fov)
            && CompareNumber(this->width, inRhs.width)
            && CompareNumber(this->height, inRhs.height)
            && CompareNumber(this->nearPlane, inRhs.nearPlane);
    }
}
