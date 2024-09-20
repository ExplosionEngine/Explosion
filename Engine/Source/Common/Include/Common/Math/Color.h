//
// Created by johnk on 2023/7/7.
//

#pragma once

#include <cstdint>

#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    struct Color;
    struct LinearColor;

    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color();
        Color(uint8_t inR, uint8_t inG, uint8_t inB);
        Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);
        explicit Color(const LinearColor& inColor);
        Color(const Color& inOther);
        Color(Color&& inOther) noexcept;
        Color& operator=(const Color& inOther);
        bool operator==(const Color& inRhs) const;

        LinearColor ToLinearColor() const;
        std::string ToHexString() const;
    };

    struct LinearColor {
        float r;
        float g;
        float b;
        float a;

        LinearColor();
        LinearColor(float inR, float inG, float inB);
        LinearColor(float inR, float inG, float inB, float inA);
        explicit LinearColor(const Color& inColor);
        LinearColor(const LinearColor& inOther);
        LinearColor(LinearColor&& inOther) noexcept;
        LinearColor& operator=(const LinearColor& inOther);
        bool operator==(const LinearColor& inRhs) const;

        void CheckValid() const;
        Color ToColor() const;
    };

    struct ColorConsts {
        static const Color white;
        static const Color black;
        static const Color red;
        static const Color blue;
        static const Color green;
    };

    struct LinearColorConsts {
        static const LinearColor white;
        static const LinearColor black;
        static const LinearColor red;
        static const LinearColor blue;
        static const LinearColor green;
    };
}

namespace Common {
    template <>
    struct Serializer<Color> {
        static constexpr size_t typeId = HashUtils::StrCrc32("Common::Color");

        static size_t Serialize(BinarySerializeStream& stream, const Color& value)
        {
            return Serializer<uint8_t>::Serialize(stream, value.r)
                + Serializer<uint8_t>::Serialize(stream, value.g)
                + Serializer<uint8_t>::Serialize(stream, value.b)
                + Serializer<uint8_t>::Serialize(stream, value.a);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Color& value)
        {
            return Serializer<uint8_t>::Deserialize(stream, value.r)
                + Serializer<uint8_t>::Deserialize(stream, value.g)
                + Serializer<uint8_t>::Deserialize(stream, value.b)
                + Serializer<uint8_t>::Deserialize(stream, value.a);
        }
    };

    template <>
    struct Serializer<LinearColor> {
        static constexpr size_t typeId = HashUtils::StrCrc32("Common::LinearColor");

        static size_t Serialize(BinarySerializeStream& stream, const LinearColor& value)
        {
            return Serializer<float>::Serialize(stream, value.r)
                + Serializer<float>::Serialize(stream, value.g)
                + Serializer<float>::Serialize(stream, value.b)
                + Serializer<float>::Serialize(stream, value.a);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, LinearColor& value)
        {
            return Serializer<float>::Deserialize(stream, value.r)
                + Serializer<float>::Deserialize(stream, value.g)
                + Serializer<float>::Deserialize(stream, value.b)
                + Serializer<float>::Deserialize(stream, value.a);
        }
    };

    template <>
    struct StringConverter<Color> {
        static std::string ToString(const Color& inValue)
        {
            return fmt::format(
                "{}r={}, g={}, b={}, a={}{}",
                "{",
                StringConverter<uint8_t>::ToString(inValue.r),
                StringConverter<uint8_t>::ToString(inValue.g),
                StringConverter<uint8_t>::ToString(inValue.b),
                StringConverter<uint8_t>::ToString(inValue.a),
                "}");
        }
    };

    template <>
    struct StringConverter<LinearColor> {
        static std::string ToString(const LinearColor& inValue)
        {
            return fmt::format(
                "{}r={}, g={}, b={}, a={}{}",
                "{",
                StringConverter<float>::ToString(inValue.r),
                StringConverter<float>::ToString(inValue.g),
                StringConverter<float>::ToString(inValue.b),
                StringConverter<float>::ToString(inValue.a),
                "}");
        }
    };

    template <>
    struct JsonSerializer<Color> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Color& inValue)
        {
            outJsonValue.SetObject();
            outJsonValue.AddMember("r", inValue.r, inAllocator);
            outJsonValue.AddMember("g", inValue.g, inAllocator);
            outJsonValue.AddMember("b", inValue.b, inAllocator);
            outJsonValue.AddMember("a", inValue.a, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Color& outValue)
        {
            if (!inJsonValue.IsObject()) {
                return;
            }
            if (inJsonValue.HasMember("r") && inJsonValue["r"].IsUint()) {
                outValue.r = static_cast<uint8_t>(inJsonValue["r"].GetUint());
            }
            if (inJsonValue.HasMember("g") && inJsonValue["g"].IsUint()) {
                outValue.g = static_cast<uint8_t>(inJsonValue["g"].GetUint());
            }
            if (inJsonValue.HasMember("b") && inJsonValue["b"].IsUint()) {
                outValue.b = static_cast<uint8_t>(inJsonValue["b"].GetUint());
            }
            if (inJsonValue.HasMember("a") && inJsonValue["a"].IsUint()) {
                outValue.a = static_cast<uint8_t>(inJsonValue["a"].GetUint());
            }
        }
    };

    template <>
    struct JsonSerializer<LinearColor> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const LinearColor& inValue)
        {
            outJsonValue.SetObject();
            outJsonValue.AddMember("r", inValue.r, inAllocator);
            outJsonValue.AddMember("g", inValue.g, inAllocator);
            outJsonValue.AddMember("b", inValue.b, inAllocator);
            outJsonValue.AddMember("a", inValue.a, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, LinearColor& outValue)
        {
            if (!inJsonValue.IsObject()) {
                return;
            }
            if (inJsonValue.HasMember("r") && inJsonValue["r"].IsFloat()) {
                outValue.r = inJsonValue["r"].GetFloat();
            }
            if (inJsonValue.HasMember("g") && inJsonValue["g"].IsFloat()) {
                outValue.g = inJsonValue["g"].GetFloat();
            }
            if (inJsonValue.HasMember("b") && inJsonValue["b"].IsFloat()) {
                outValue.b = inJsonValue["b"].GetFloat();
            }
            if (inJsonValue.HasMember("a") && inJsonValue["a"].IsFloat()) {
                outValue.a = inJsonValue["a"].GetFloat();
            }
        }
    };
}
