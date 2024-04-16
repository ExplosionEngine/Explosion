//
// Created by johnk on 2023/7/7.
//

#pragma once

#include <cstdint>
#include <sstream>

#include <Common/Debug.h>
#include <Common/Serialization.h>

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
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("Common::Color");

        static void Serialize(SerializeStream& stream, const Color& value)
        {
            TypeIdSerializer<Color>::Serialize(stream);

            Serializer<uint8_t>::Serialize(stream, value.r);
            Serializer<uint8_t>::Serialize(stream, value.g);
            Serializer<uint8_t>::Serialize(stream, value.b);
            Serializer<uint8_t>::Serialize(stream, value.a);
        }

        static bool Deserialize(DeserializeStream& stream, Color& value)
        {
            if (!TypeIdSerializer<Color>::Deserialize(stream)) {
                return false;
            }

            Serializer<uint8_t>::Deserialize(stream, value.r);
            Serializer<uint8_t>::Deserialize(stream, value.g);
            Serializer<uint8_t>::Deserialize(stream, value.b);
            Serializer<uint8_t>::Deserialize(stream, value.a);
        }
    };

    template <>
    struct Serializer<LinearColor> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("Common::LinearColor");

        static void Serialize(SerializeStream& stream, const LinearColor& value)
        {
            TypeIdSerializer<Color>::Serialize(stream);

            Serializer<float>::Serialize(stream, value.r);
            Serializer<float>::Serialize(stream, value.g);
            Serializer<float>::Serialize(stream, value.b);
            Serializer<float>::Serialize(stream, value.a);
        }

        static bool Deserialize(DeserializeStream& stream, LinearColor& value)
        {
            if (!TypeIdSerializer<Color>::Deserialize(stream)) {
                return false;
            }

            Serializer<float>::Deserialize(stream, value.r);
            Serializer<float>::Deserialize(stream, value.g);
            Serializer<float>::Deserialize(stream, value.b);
            Serializer<float>::Deserialize(stream, value.a);
        }
    };
}
