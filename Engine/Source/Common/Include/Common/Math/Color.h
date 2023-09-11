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

        Color();
        Color(uint8_t inR, uint8_t inG, uint8_t inB);
        Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA);
        explicit Color(const LinearColor& inColor);
        Color(const Color& inOther);
        Color(Color&& inOther) noexcept;
        Color& operator=(const Color& inOther);

        inline LinearColor ToLinearColor() const;
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
        // TODO more color here
    };
}
