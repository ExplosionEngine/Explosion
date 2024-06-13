//
// Created by johnk on 2023/8/17.
//

#include <cmath>
#include <sstream>

#include <Common/Math/Color.h>

namespace Common {
    Color::Color()
        : r(0), g(0), b(0), a(0)
    {
    }

    Color::Color(const uint8_t inR, const uint8_t inG, const uint8_t inB)
        : r(inR), g(inG), b(inB), a(255)
    {
    }

    Color::Color(const uint8_t inR, const uint8_t inG, const uint8_t inB, const uint8_t inA)
        : r(inR), g(inG), b(inB), a(inA)
    {
    }

    Color::Color(const LinearColor& inColor)
    {
        *this = inColor.ToColor();
    }

    Color::Color(const Color& inOther) = default;

    Color::Color(Color&& inOther) noexcept
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
    }

    Color& Color::operator=(const Color& inOther) = default;

    LinearColor Color::ToLinearColor() const
    {
        return {
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f
        };
    }

    std::string Color::ToHexString() const
    {
        std::stringstream result;
        result << "0x" << std::hex << r << std::hex << g << std::hex << b << std::hex << a;
        return result.str();
    }

    LinearColor::LinearColor()
        : r(0), g(0), b(0), a(0)
    {
        CheckValid();
    }

    LinearColor::LinearColor(float inR, float inG, float inB)
        : r(inR), g(inG), b(inB), a(1.0f)
    {
        CheckValid();
    }

    LinearColor::LinearColor(float inR, float inG, float inB, float inA)
        : r(inR), g(inG), b(inB), a(inA)
    {
        CheckValid();
    }

    LinearColor::LinearColor(const Color& inColor)
    {
        *this = inColor.ToLinearColor();
        CheckValid();
    }

    LinearColor::LinearColor(const LinearColor& inOther)
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
        CheckValid();
    }

    LinearColor::LinearColor(LinearColor&& inOther) noexcept
        : r(inOther.r), g(inOther.g), b(inOther.b), a(inOther.a)
    {
        CheckValid();
    }

    LinearColor& LinearColor::operator=(const LinearColor& inOther)
    {
        r = inOther.r;
        g = inOther.g;
        b = inOther.b;
        a = inOther.a;
        CheckValid();
        return *this;
    }

    Color LinearColor::ToColor() const
    {
        return {
            static_cast<uint8_t>(std::round(r * 255)),
            static_cast<uint8_t>(std::round(g * 255)),
            static_cast<uint8_t>(std::round(b * 255)),
            static_cast<uint8_t>(std::round(a * 255))
        };
    }

    void LinearColor::CheckValid() const
    {
        Assert(r >= 0.0f && r <= 1.0f);
        Assert(g >= 0.0f && g <= 1.0f);
        Assert(b >= 0.0f && b <= 1.0f);
        Assert(a >= 0.0f && a <= 1.0f);
    }

    const Color ColorConsts::white = Color(255, 255, 255, 255);
    const Color ColorConsts::black = Color(0, 0, 0, 1);
    const Color ColorConsts::red = Color(1, 0, 0, 1);
    const Color ColorConsts::green = Color(0, 1, 0, 1);
    const Color ColorConsts::blue = Color(0, 0, 1, 1);

    const LinearColor LinearColorConsts::white = LinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    const LinearColor LinearColorConsts::black = LinearColor(0.0f, 0.0f, 0.0f, 1.0f);
    const LinearColor LinearColorConsts::red = LinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    const LinearColor LinearColorConsts::green = LinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    const LinearColor LinearColorConsts::blue = LinearColor(0.0f, 0.0f, 1.0f, 1.0f);
}
