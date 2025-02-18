//
// Created by johnk on 2023/7/8.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <typename T>
    struct SphereBase {
        Vec<T, 3> center;
        T radius;
    };

    template <typename T>
    struct Sphere : SphereBase<T> {
        Sphere();
        Sphere(T inX, T inY, T inZ, T inRadius);
        Sphere(Vec<T, 3> inCenter, T inRadius);
        Sphere(const Sphere& inOther);
        Sphere(Sphere&& inOther) noexcept;
        Sphere& operator=(const Sphere& inOther);

        T Distance(const Sphere& inOther) const;
        bool Inside(const Vec<T, 3>& inPoint) const;
        bool Intersect(const Sphere& inOther) const;
        bool operator==(const Sphere& inRhs) const;

        template <typename IT> Sphere<IT> CastTo() const;
    };

    using ISphere = Sphere<int32_t>;
    using HSphere = Sphere<HFloat>;
    using FSphere = Sphere<float>;
    using DSphere = Sphere<double>;
}

namespace Common {
    template <Serializable T>
    struct Serializer<Sphere<T>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Sphere")
            + Serializer<T>::typeId;

        static size_t Serialize(BinarySerializeStream& stream, const Sphere<T>& value)
        {
            size_t serialized = 0;
            serialized += Serializer<Vec<T, 3>>::Serialize(stream, value.center);
            serialized += Serializer<T>::Serialize(stream, value.radius);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Sphere<T>& value)
        {
            size_t deserialized = 0;
            deserialized += Serializer<Vec<T, 3>>::Deserialize(stream, value.center);
            deserialized += Serializer<T>::Deserialize(stream, value.radius);
            return deserialized;
        }
    };

    template <StringConvertible T>
    struct StringConverter<Sphere<T>> {
        static std::string ToString(const Sphere<T>& inValue)
        {
            return std::format(
                "{}center={}, radius={}{}",
                "{",
                StringConverter<Vec<T, 3>>::ToString(inValue.center),
                StringConverter<T>::ToString(inValue.radius),
                "}");
        }
    };

    template <JsonSerializable T>
    struct JsonSerializer<Sphere<T>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Sphere<T>& inValue)
        {
            rapidjson::Value centerJson;
            JsonSerializer<Vec<T, 3>>::JsonSerialize(centerJson, inAllocator, inValue.center);

            rapidjson::Value radiusJson;
            JsonSerializer<T>::JsonSerialize(radiusJson, inAllocator, inValue.radius);

            outJsonValue.SetObject();
            outJsonValue.AddMember("center", centerJson, inAllocator);
            outJsonValue.AddMember("radius", radiusJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Sphere<T>& outValue)
        {
            if (!inJsonValue.IsObject()) {
                return;
            }
            if (inJsonValue.HasMember("center")) {
                JsonSerializer<Vec<T, 3>>::JsonDeserialize(inJsonValue["center"], outValue.center);
            }
            if (inJsonValue.HasMember("radius")) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue["radius"], outValue.radius);
            }
        }
    };
}

namespace Common {
    template <typename T>
    Sphere<T>::Sphere()
    {
        this->center = VecConsts<T, 3>::zero;
        this->radius = 0;
    }

    template <typename T>
    Sphere<T>::Sphere(T inX, T inY, T inZ, T inRadius)
    {
        this->center = Vec<T, 3>(inX, inY, inZ);
        this->radius = inRadius;
    }

    template <typename T>
    Sphere<T>::Sphere(Vec<T, 3> inCenter, T inRadius)
    {
        this->center = std::move(inCenter);
        this->radius = inRadius;
    }

    template <typename T>
    Sphere<T>::Sphere(const Sphere<T>& inOther)
    {
        this->center = inOther.center;
        this->radius = inOther.radius;
    }

    template <typename T>
    Sphere<T>::Sphere(Sphere<T>&& inOther) noexcept
    {
        this->center = std::move(inOther.center);
        this->radius = inOther.radius;
    }

    template <typename T>
    Sphere<T>& Sphere<T>::operator=(const Sphere<T>& inOther)
    {
        this->center = inOther.center;
        this->radius = inOther.radius;
        return *this;
    }

    template <typename T>
    T Sphere<T>::Distance(const Sphere& inOther) const
    {
        Vec<T, 3> direction = this->center - inOther.center;
        return direction.Model();
    }

    template <typename T>
    bool Sphere<T>::Inside(const Vec<T, 3>& inPoint) const
    {
        Vec<T, 3> direction = this->center - inPoint;
        return direction.Model() <= this->radius;
    }

    template <typename T>
    bool Sphere<T>::Intersect(const Sphere& inOther) const
    {
        Vec<T, 3> direction = this->center - inOther.center;
        return direction.Model() <= (this->radius + inOther.radius);
    }

    template <typename T>
    bool Sphere<T>::operator==(const Sphere& inRhs) const
    {
        return this->center == inRhs.center
            && CompareNumber(this->radius, inRhs.radius);
    }

    template <typename T>
    template <typename IT>
    Sphere<IT> Sphere<T>::CastTo() const
    {
        return Sphere<IT>(
            this->center.template CastTo<IT>(),
            static_cast<IT>(this->radius)
        );
    }
}
