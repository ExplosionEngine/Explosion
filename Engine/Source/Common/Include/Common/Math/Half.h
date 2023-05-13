//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <cmath>

#include <Common/Math/Constant.h>

namespace Common::Internal {
    union FullFloat {
        float value;
        struct {
            // TODO this is little endian, maybe we need support big endian in the future
            uint32_t mantissa : 23;
            uint32_t exponent : 8;
            uint32_t sign : 1;
        };
    };
}

namespace Common {
    struct HalfFloat {
        union {
            uint16_t value;
            struct {
                // TODO this is little endian, maybe we need support big endian in the future
                uint16_t mantissa : 10;
                uint16_t exponent : 5;
                uint16_t sign : 1;
            };
        };

        HalfFloat() : value(0) {}
        HalfFloat(const HalfFloat& inValue) : value(inValue.value) {}
        HalfFloat(HalfFloat&& inValue) noexcept : value(inValue.value) {}

        HalfFloat(float inValue) // NOLINT
        {
            Set(inValue);
        }

        HalfFloat& operator=(float inValue)
        {
            Set(inValue);
            return *this;
        }

        HalfFloat& operator=(const HalfFloat& inValue)
        {
            value = inValue.value;
            return *this;
        }

        void Set(float inValue)
        {
            // TODO check this ?
            Internal::FullFloat full;
            full.value = inValue;

            sign = full.sign;
            if (full.exponent <= 112)
            {
                exponent = 0;
                mantissa = 0;

                const int32_t newExp = full.exponent - 127 + 15;

                if ((14 - newExp) <= 24)
                {
                    uint32_t mantissa = full.mantissa | 0x800000;
                    mantissa = static_cast<uint16_t>(mantissa >> (14 - newExp));
                    if (((mantissa >> (13 - newExp)) & 1) != 0u)
                    {
                        value++;
                    }
                }
            }
            else if (full.exponent >= 143)
            {
                exponent = 30;
                mantissa = 1023;
            }
            else
            {
                exponent = uint16_t(int32_t(full.exponent) - 127 + 15);
                mantissa = uint16_t(full.mantissa >> 13);
            }
        }

        float AsFloat() const
        {
            // TODO check this ?
            Internal::FullFloat result;

            result.sign = sign;
            if (exponent == 0)
            {
                uint32_t tMantissa = mantissa;
                if(tMantissa == 0)
                {
                    result.exponent = 0;
                    result.mantissa = 0;
                }
                else
                {
                    uint32_t mantissaShift = 10 - (uint32_t)(std::log2((float) tMantissa));
                    result.exponent = 127 - (15 - 1) - mantissaShift;
                    result.mantissa = tMantissa << (mantissaShift + 23 - 10);
                }
            }
            else if (exponent == 31)
            {
                result.exponent = 142;
                result.mantissa = 8380416;
            }
            else
            {
                result.exponent = int32_t(exponent) - 15 + 127;
                result.mantissa = uint32_t(mantissa) << 13;
            }

            return result.value;
        }

        operator float() const // NOLINT
        {
            return AsFloat();
        }

        bool operator==(float rhs) const
        {
            return std::abs(AsFloat() - rhs) < halfEpsilon;
        }

        bool operator!=(float rhs) const
        {
            return std::abs(AsFloat() - rhs) >= halfEpsilon;
        }

        bool operator==(const HalfFloat& rhs) const
        {
            return std::abs(AsFloat() - rhs.AsFloat()) < halfEpsilon;
        }

        bool operator!=(const HalfFloat& rhs) const
        {
            return std::abs(AsFloat() - rhs.AsFloat()) >= halfEpsilon;
        }

        bool operator>(const HalfFloat& rhs) const
        {
            return AsFloat() > rhs.AsFloat();
        }

        bool operator<(const HalfFloat& rhs) const
        {
            return AsFloat() < rhs.AsFloat();
        }

        bool operator>=(const HalfFloat& rhs) const
        {
            return AsFloat() >= rhs.AsFloat();
        }

        bool operator<=(const HalfFloat& rhs) const
        {
            return AsFloat() <= rhs.AsFloat();
        }

        HalfFloat operator+(float rhs) const
        {
            return { AsFloat() + rhs };
        }

        HalfFloat operator-(float rhs) const
        {
            return { AsFloat() - rhs };
        }

        HalfFloat operator*(float rhs) const
        {
            return { AsFloat() * rhs };
        }

        HalfFloat operator/(float rhs) const
        {
            return { AsFloat() / rhs };
        }

        HalfFloat operator+(const HalfFloat& rhs) const
        {
            return { AsFloat() + rhs.AsFloat() };
        }

        HalfFloat operator-(const HalfFloat& rhs) const
        {
            return { AsFloat() - rhs.AsFloat() };
        }

        HalfFloat operator*(const HalfFloat& rhs) const
        {
            return { AsFloat() * rhs.AsFloat() };
        }

        HalfFloat operator/(const HalfFloat& rhs) const
        {
            return { AsFloat() / rhs.AsFloat() };
        }

        HalfFloat& operator+=(float rhs)
        {
            Set(AsFloat() + rhs);
            return *this;
        }

        HalfFloat& operator-=(float rhs)
        {
            Set(AsFloat() - rhs);
            return *this;
        }

        HalfFloat& operator*=(float rhs)
        {
            Set(AsFloat() * rhs);
            return *this;
        }

        HalfFloat& operator/=(float rhs)
        {
            Set(AsFloat() / rhs);
            return *this;
        }

        HalfFloat& operator+=(const HalfFloat& rhs)
        {
            Set(AsFloat() + rhs.AsFloat());
            return *this;
        }

        HalfFloat& operator-=(const HalfFloat& rhs)
        {
            Set(AsFloat() - rhs.AsFloat());
            return *this;
        }

        HalfFloat& operator*=(const HalfFloat& rhs)
        {
            Set(AsFloat() * rhs.AsFloat());
            return *this;
        }

        HalfFloat& operator/=(const HalfFloat& rhs)
        {
            Set(AsFloat() / rhs.AsFloat());
            return *this;
        }
    };

    template <typename T>
    constexpr bool isHalfFloatingPointV = std::is_same_v<T, HalfFloat>;

    template <typename T>
    constexpr bool isFloatingPointV = std::is_floating_point_v<T> || isHalfFloatingPointV<T>;
}
