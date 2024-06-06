//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <cmath>
#include <bit>

#include <Common/Math/Common.h>
#include <Common/Serialization.h>

namespace Common::Internal {
    template <std::endian E>
    requires (E == std::endian::little) || (E == std::endian::big)
    struct FullFloatBase {};

    template <>
    struct FullFloatBase<std::endian::little> {
        union {
            float value;
            struct {
                uint32_t mantissa : 23;
                uint32_t exponent : 8;
                uint32_t sign : 1;
            };
        };

        explicit FullFloatBase(float inValue) : value(inValue) {}
    };

    template <>
    struct FullFloatBase<std::endian::big> {
        union {
            float value;
            struct {
                uint32_t sign : 1;
                uint32_t exponent : 8;
                uint32_t mantissa : 23;
            };
        };

        explicit FullFloatBase(float inValue) : value(inValue) {}
    };

    template <std::endian E>
    struct FullFloat : public FullFloatBase<E> {
        FullFloat() : FullFloatBase<E>(0) {}
        FullFloat(float inValue) : FullFloatBase<E>(inValue) {} // NOLINT
        FullFloat(const FullFloat& inValue) : FullFloatBase<E>(inValue.value) {}
        FullFloat(FullFloat&& inValue) noexcept : FullFloatBase<E>(inValue.value) {}
    };
}

namespace Common {
    template <std::endian E>
    requires (E == std::endian::little) || (E == std::endian::big)
    struct HalfFloatBase {};

    template <>
    struct HalfFloatBase<std::endian::little> {
        union {
            uint16_t value;
            struct {
                uint16_t mantissa : 10;
                uint16_t exponent : 5;
                uint16_t sign : 1;
            };
        };

        explicit HalfFloatBase(uint16_t inValue) : value(inValue) {}
    };

    template <>
    struct HalfFloatBase<std::endian::big> {
        union {
            uint16_t value;
            struct {
                uint16_t sign : 1;
                uint16_t exponent : 5;
                uint16_t mantissa : 10;
            };
        };

        explicit HalfFloatBase(uint16_t inValue) : value(inValue) {}
    };

    template <std::endian E>
    struct HalfFloat : HalfFloatBase<E> {
        HalfFloat() : HalfFloatBase<E>(0) {}
        HalfFloat(const HalfFloat& inValue) : HalfFloatBase<E>(inValue.value) {}
        HalfFloat(HalfFloat&& inValue) noexcept : HalfFloatBase<E>(inValue.value) {}

        HalfFloat(float inValue) : HalfFloatBase<E>(0) // NOLINT
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
            this->value = inValue.value;
            return *this;
        }

        void Set(float inValue)
        {
            Internal::FullFloat<E> full(inValue);

            this->sign = full.sign;
            if (full.exponent <= 112)
            {
                this->exponent = 0;
                this->mantissa = 0;

                const int32_t newExp = full.exponent - 127 + 15;

                if ((14 - newExp) <= 24)
                {
                    const uint32_t tMantissa = full.mantissa | 0x800000;
                    this->mantissa = static_cast<uint16_t>(tMantissa >> (14 - newExp));
                    if (((this->mantissa >> (13 - newExp)) & 1) != 0u)
                    {
                        this->value++;
                    }
                }
            }
            else if (full.exponent >= 143)
            {
                this->exponent = 30;
                this->mantissa = 1023;
            }
            else
            {
                this->exponent = static_cast<uint16_t>(static_cast<int32_t>(full.exponent) - 127 + 15);
                this->mantissa = static_cast<uint16_t>(full.mantissa >> 13);
            }
        }

        float AsFloat() const
        {
            Internal::FullFloat<E> result;

            result.sign = this->sign;
            if (this->exponent == 0)
            {
                uint32_t tMantissa = this->mantissa;
                if(tMantissa == 0)
                {
                    result.exponent = 0;
                    result.mantissa = 0;
                }
                else
                {
                    uint32_t mantissaShift = 10 - static_cast<uint32_t>(std::trunc(std::log2(static_cast<float>(tMantissa))));
                    result.exponent = 127 - (15 - 1) - mantissaShift;
                    result.mantissa = tMantissa << (mantissaShift + 23 - 10);
                }
            }
            else if (this->exponent == 31)
            {
                result.exponent = 142;
                result.mantissa = 8380416;
            }
            else
            {
                result.exponent = static_cast<int32_t>(this->exponent) - 15 + 127;
                result.mantissa = static_cast<uint32_t>(this->mantissa) << 13;
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

    using HFloat = HalfFloat<std::endian::native>;

    template <typename T>
    constexpr bool isHalfFloatingPointV = std::is_same_v<T, HFloat>;

    template <typename T>
    constexpr bool isFloatingPointV = std::is_floating_point_v<T> || isHalfFloatingPointV<T>;
}

namespace Common {
    template <std::endian E>
    struct Serializer<Internal::FullFloat<E>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = HashUtils::StrCrc32("Common::Internal::FullFloat");

        static void Serialize(SerializeStream& stream, const Internal::FullFloat<E>& value)
        {
            TypeIdSerializer<Internal::FullFloat<E>>::Serialize(stream);

            Serializer<float>::Serialize(stream, value.value);
        }

        static bool Deserialize(DeserializeStream& stream, Internal::FullFloat<E>& value)
        {
            if (!TypeIdSerializer<Internal::FullFloat<E>>::Deserialize(stream)) {
                return false;
            }

            Serializer<float>::Deserialize(stream, value.value);
            return true;
        }
    };

    template <std::endian E>
    struct Serializer<HalfFloat<E>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = HashUtils::StrCrc32("Common::Internal::HalfFloat");

        static void Serialize(SerializeStream& stream, const HalfFloat<E>& value)
        {
            TypeIdSerializer<HalfFloat<E>>::Serialize(stream);

            Serializer<uint16_t>::Serialize(stream, value.value);
        }

        static bool Deserialize(DeserializeStream& stream, HalfFloat<E>& value)
        {
            if (!TypeIdSerializer<HalfFloat<E>>::Deserialize(stream)) {
                return false;
            }

            Serializer<uint16_t>::Deserialize(stream, value.value);
            return true;
        }
    };
}
