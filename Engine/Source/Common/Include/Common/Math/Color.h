//
// Created by johnk on 2023/7/7.
//

#pragma once

#include <cstdint>
#include <sstream>

#include <Common/Debug.h>

namespace Common {
    struct Color;
    struct LinearColor;

    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        inline Color();
        inline Color(uint8_t inR, uint8_t inG, uint8_t inB);
        inline Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);
        inline explicit Color(const LinearColor& inColor);
        inline Color(const Color& inOther);
        inline Color(Color&& inOther) noexcept;
        inline Color& operator=(const Color& inOther);

        inline LinearColor ToLinearColor() const;
        std::string ToHexString() const;
    };

    struct LinearColor {
        float r;
        float g;
        float b;
        float a;

        inline LinearColor();
        inline LinearColor(float inR, float inG, float inB);
        inline LinearColor(float inR, float inG, float inB, float inA);
        inline explicit LinearColor(const Color& inColor);
        inline LinearColor(const LinearColor& inOther);
        inline LinearColor(LinearColor&& inOther) noexcept;
        inline LinearColor& operator=(const LinearColor& inOther);

        inline void CheckValid() const;
        inline Color ToColor() const;
    };

    struct ColorConsts {
        static const Color white;
        static const Color black;
        static const Color red;
        static const Color blue;
        static const Color green;
        // TODO more color here
    };
}
