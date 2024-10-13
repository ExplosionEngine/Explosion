//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <cmath>
#include <bit>

#include <Common/Math/Common.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <std::endian E> concept ValidEndian = E == std::endian::little || E == std::endian::big;
}

namespace Common::Internal {
    template <std::endian E>
    requires ValidEndian<E>
    struct FullFloatBase {};

    template <std::endian E>
    struct FullFloat : FullFloatBase<E> {
        FullFloat();
        FullFloat(float inValue); // NOLINT
        FullFloat(const FullFloat& inValue);
        FullFloat(FullFloat&& inValue) noexcept;
    };
}

namespace Common {
    template <std::endian E>
    requires ValidEndian<E>
    struct HalfFloatBase {};

    template <std::endian E>
    struct HalfFloat : HalfFloatBase<E> {
        HalfFloat();
        HalfFloat(const HalfFloat& inValue);
        HalfFloat(HalfFloat&& inValue) noexcept;
        HalfFloat(float inValue); // NOLINT

        HalfFloat& operator=(float inValue);
        HalfFloat& operator=(const HalfFloat& inValue);

        void Set(float inValue);
        float AsFloat() const;
        operator float() const; // NOLINT

        bool operator==(float rhs) const;
        bool operator!=(float rhs) const;
        bool operator==(const HalfFloat& rhs) const;
        bool operator!=(const HalfFloat& rhs) const;
        bool operator>(const HalfFloat& rhs) const;
        bool operator<(const HalfFloat& rhs) const;
        bool operator>=(const HalfFloat& rhs) const;
        bool operator<=(const HalfFloat& rhs) const;
        HalfFloat operator+(float rhs) const;
        HalfFloat operator-(float rhs) const;
        HalfFloat operator*(float rhs) const;
        HalfFloat operator/(float rhs) const;
        HalfFloat operator+(const HalfFloat& rhs) const;
        HalfFloat operator-(const HalfFloat& rhs) const;
        HalfFloat operator*(const HalfFloat& rhs) const;
        HalfFloat operator/(const HalfFloat& rhs) const;
        HalfFloat& operator+=(float rhs);
        HalfFloat& operator-=(float rhs);
        HalfFloat& operator*=(float rhs);
        HalfFloat& operator/=(float rhs);
        HalfFloat& operator+=(const HalfFloat& rhs);
        HalfFloat& operator-=(const HalfFloat& rhs);
        HalfFloat& operator*=(const HalfFloat& rhs);
        HalfFloat& operator/=(const HalfFloat& rhs);
    };

    using HFloat = HalfFloat<std::endian::native>;

    template <typename T> concept HalfFloatingPoint = std::is_same_v<T, HFloat>;
    template <typename T> concept FloatingPoint = std::is_floating_point_v<T> || std::is_same_v<T, HFloat>;
}

namespace Common {
    template <std::endian E>
    struct Serializer<Internal::FullFloat<E>> {
        static constexpr size_t typeId = HashUtils::StrCrc32("Common::Internal::FullFloat");

        static size_t Serialize(BinarySerializeStream& stream, const Internal::FullFloat<E>& value)
        {
            return Serializer<float>::Serialize(stream, value.value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Internal::FullFloat<E>& value)
        {
            return Serializer<float>::Deserialize(stream, value.value);
        }
    };

    template <std::endian E>
    struct Serializer<HalfFloat<E>> {
        static constexpr size_t typeId = HashUtils::StrCrc32("Common::Internal::HalfFloat");

        static size_t Serialize(BinarySerializeStream& stream, const HalfFloat<E>& value)
        {
            return Serializer<uint16_t>::Serialize(stream, value.value);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, HalfFloat<E>& value)
        {
            return Serializer<uint16_t>::Deserialize(stream, value.value);
        }
    };

    template <std::endian E>
    struct StringConverter<Internal::FullFloat<E>> {
        static std::string ToString(const Internal::FullFloat<E>& inValue)
        {
            return StringConverter<float>::ToString(inValue.value);
        }
    };

    template <std::endian E>
    struct StringConverter<HalfFloat<E>> {
        static std::string ToString(const HalfFloat<E>& inValue)
        {
            return StringConverter<float>::ToString(inValue.AsFloat());
        }
    };

    template <std::endian E>
    struct JsonSerializer<Internal::FullFloat<E>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Internal::FullFloat<E>& inValue)
        {
            JsonSerializer<float>::JsonSerialize(outJsonValue, inAllocator, inValue.value);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Internal::FullFloat<E>& outValue)
        {
            JsonSerializer<float>::JsonDeserialize(inJsonValue, outValue.value);
        }
    };

    template <std::endian E>
    struct JsonSerializer<HalfFloat<E>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const HalfFloat<E>& inValue)
        {
            JsonSerializer<float>::JsonSerialize(outJsonValue, inAllocator, inValue.AsFloat());
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, HalfFloat<E>& outValue)
        {
            float fltValue;
            JsonSerializer<float>::JsonDeserialize(inJsonValue, fltValue);
            outValue = fltValue;
        }
    };
}

namespace Common::Internal {
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

        inline explicit FullFloatBase(float inValue);
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

        inline explicit FullFloatBase(float inValue);
    };

    FullFloatBase<std::endian::little>::FullFloatBase(const float inValue)
        : value(inValue)
    {
    }

    FullFloatBase<std::endian::big>::FullFloatBase(const float inValue)
        : value(inValue)
    {
    }

    template <std::endian E>
    FullFloat<E>::FullFloat()
        : FullFloatBase<E>(0)
    {
    }

    template <std::endian E>
    FullFloat<E>::FullFloat(float inValue)
        : FullFloatBase<E>(inValue)
    {
    }

    template <std::endian E>
    FullFloat<E>::FullFloat(const FullFloat& inValue)
        : FullFloatBase<E>(inValue.value)
    {
    }

    template <std::endian E>
    FullFloat<E>::FullFloat(FullFloat&& inValue) noexcept
        : FullFloatBase<E>(inValue.value)
    {
    }
}

namespace Common {
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

        inline explicit HalfFloatBase(uint16_t inValue);
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

        inline explicit HalfFloatBase(uint16_t inValue);
    };

    HalfFloatBase<std::endian::little>::HalfFloatBase(const uint16_t inValue)
        : value(inValue)
    {
    }

    HalfFloatBase<std::endian::big>::HalfFloatBase(const uint16_t inValue)
        : value(inValue)
    {
    }

    template <std::endian E>
    HalfFloat<E>::HalfFloat()
        : HalfFloatBase<E>(0)
    {
    }

    template <std::endian E>
    HalfFloat<E>::HalfFloat(const HalfFloat& inValue)
        : HalfFloatBase<E>(inValue.value)
    {
    }

    template <std::endian E>
    HalfFloat<E>::HalfFloat(HalfFloat&& inValue) noexcept
        : HalfFloatBase<E>(inValue.value)
    {
    }

    template <std::endian E>
    HalfFloat<E>::HalfFloat(const float inValue)
        : HalfFloatBase<E>(0)
    {
        Set(inValue);
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator=(const float inValue)
    {
        Set(inValue);
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator=(const HalfFloat& inValue)
    {
        this->value = inValue.value;
        return *this;
    }

    template <std::endian E>
    void HalfFloat<E>::Set(float inValue)
    {
        Internal::FullFloat<E> full(inValue);

        this->sign = full.sign;
        if (full.exponent <= 112)
        {
            this->exponent = 0;
            this->mantissa = 0;

            if (const int32_t newExp = full.exponent - 127 + 15;
                14 - newExp <= 24)
            {
                const uint32_t tMantissa = full.mantissa | 0x800000;
                this->mantissa = static_cast<uint16_t>(tMantissa >> (14 - newExp));
                if (((this->mantissa >> (13 - newExp)) & 1) != 0u)
                {
                    ++this->value;
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

    template <std::endian E>
    float HalfFloat<E>::AsFloat() const
    {
        Internal::FullFloat<E> result;

        result.sign = this->sign;
        if (this->exponent == 0)
        {
            if (const uint32_t tMantissa = this->mantissa;
                tMantissa == 0)
            {
                result.exponent = 0;
                result.mantissa = 0;
            }
            else
            {
                const uint32_t mantissaShift = 10 - static_cast<uint32_t>(std::trunc(std::log2(static_cast<float>(tMantissa))));
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

    template <std::endian E>
    HalfFloat<E>::operator float() const
    {
        return AsFloat();
    }

    template <std::endian E>
    bool HalfFloat<E>::operator==(const float rhs) const
    {
        return std::abs(AsFloat() - rhs) < halfEpsilon;
    }

    template <std::endian E>
    bool HalfFloat<E>::operator!=(const float rhs) const
    {
        return std::abs(AsFloat() - rhs) >= halfEpsilon;
    }

    template <std::endian E>
    bool HalfFloat<E>::operator==(const HalfFloat& rhs) const
    {
        return std::abs(AsFloat() - rhs.AsFloat()) < halfEpsilon;
    }

    template <std::endian E>
    bool HalfFloat<E>::operator!=(const HalfFloat& rhs) const
    {
        return std::abs(AsFloat() - rhs.AsFloat()) >= halfEpsilon;
    }

    template <std::endian E>
    bool HalfFloat<E>::operator>(const HalfFloat& rhs) const
    {
        return AsFloat() > rhs.AsFloat();
    }

    template <std::endian E>
    bool HalfFloat<E>::operator<(const HalfFloat& rhs) const
    {
        return AsFloat() < rhs.AsFloat();
    }

    template <std::endian E>
    bool HalfFloat<E>::operator>=(const HalfFloat& rhs) const
    {
        return AsFloat() >= rhs.AsFloat();
    }

    template <std::endian E>
    bool HalfFloat<E>::operator<=(const HalfFloat& rhs) const
    {
        return AsFloat() <= rhs.AsFloat();
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator+(const float rhs) const
    {
        return { AsFloat() + rhs };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator-(const float rhs) const
    {
        return { AsFloat() - rhs };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator*(const float rhs) const
    {
        return { AsFloat() * rhs };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator/(const float rhs) const
    {
        return { AsFloat() / rhs };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator+(const HalfFloat& rhs) const
    {
        return { AsFloat() + rhs.AsFloat() };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator-(const HalfFloat& rhs) const
    {
        return { AsFloat() - rhs.AsFloat() };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator*(const HalfFloat& rhs) const
    {
        return { AsFloat() * rhs.AsFloat() };
    }

    template <std::endian E>
    HalfFloat<E> HalfFloat<E>::operator/(const HalfFloat& rhs) const
    {
        return { AsFloat() / rhs.AsFloat() };
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator+=(const float rhs)
    {
        Set(AsFloat() + rhs);
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator-=(const float rhs)
    {
        Set(AsFloat() - rhs);
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator*=(const float rhs)
    {
        Set(AsFloat() * rhs);
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator/=(const float rhs)
    {
        Set(AsFloat() / rhs);
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator+=(const HalfFloat& rhs)
    {
        Set(AsFloat() + rhs.AsFloat());
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator-=(const HalfFloat& rhs)
    {
        Set(AsFloat() - rhs.AsFloat());
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator*=(const HalfFloat& rhs)
    {
        Set(AsFloat() * rhs.AsFloat());
        return *this;
    }

    template <std::endian E>
    HalfFloat<E>& HalfFloat<E>::operator/=(const HalfFloat& rhs)
    {
        Set(AsFloat() / rhs.AsFloat());
        return *this;
    }
}
