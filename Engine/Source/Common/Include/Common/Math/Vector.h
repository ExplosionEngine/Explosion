//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <utility>

#include <Common/Math/Half.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common::Internal {
    template <typename T, uint8_t L>
    struct VecCrossResultTraits {
        using Type = T;
    };
}

namespace Common {
    template <typename T, uint8_t L> struct Vec;

    template <uint8_t L> concept ValidVecDim = L >= 1 && L <= 4;
    template <typename T, typename VT, uint8_t L> concept VecN = std::is_same_v<T, Vec<VT, L>>;

    template <typename T, uint8_t L>
    requires ValidVecDim<L>
    struct BaseVec {};

    template <typename T, uint8_t L>
    struct Vec : BaseVec<T, L> {
        using Type = T;
        static constexpr uint8_t dims = L;

        Vec();
        Vec(T inValue); // NOLINT
        Vec(const Vec& other);
        Vec(Vec&& other) noexcept;
        Vec& operator=(const Vec& other);

        template <typename... IT>
        Vec(IT&&... inValues); // NOLINT

        T& operator[](uint32_t i);
        T operator[](uint32_t i) const;

        bool operator==(T rhs) const;
        bool operator==(const Vec& rhs) const;
        bool operator!=(T rhs) const;
        bool operator!=(const Vec& rhs) const;

        Vec operator+(T rhs) const;
        Vec operator-(T rhs) const;
        Vec operator*(T rhs) const;
        Vec operator/(T rhs) const;

        Vec operator+(const Vec& rhs) const;
        Vec operator-(const Vec& rhs) const;
        Vec operator*(const Vec& rhs) const;
        Vec operator/(const Vec& rhs) const;

        Vec& operator+=(T rhs);
        Vec& operator-=(T rhs);
        Vec& operator*=(T rhs);
        Vec& operator/=(T rhs);

        Vec& operator+=(const Vec& rhs);
        Vec& operator-=(const Vec& rhs);
        Vec& operator*=(const Vec& rhs);
        Vec& operator/=(const Vec& rhs);

        template <typename IT>
        Vec<IT, L> CastTo() const;

        template <uint8_t... I>
        Vec<T, sizeof...(I)> SubVec() const;

        T Model() const;
        Vec Normalized() const;
        void Normalize();
        T Dot(const Vec& rhs) const;
        typename Internal::VecCrossResultTraits<T, L>::Type Cross(const Vec& rhs) const;
    };

    template <typename T, uint8_t L>
    requires ValidVecDim<L>
    struct VecConsts {};

    template <typename T>
    struct VecConsts<T, 1> {
        static const Vec<T, 1> zero;
        static const Vec<T, 1> unit;
        static const Vec<T, 1> negaUnit;
    };

    template <typename T>
    struct VecConsts<T, 2> {
        static const Vec<T, 2> zero;
        static const Vec<T, 2> unitX;
        static const Vec<T, 2> unitY;
        static const Vec<T, 2> unit;
        static const Vec<T, 2> negaUnitX;
        static const Vec<T, 2> negaUnitY;
        static const Vec<T, 2> negaUnit;
    };

    template <typename T>
    struct VecConsts<T, 3> {
        static const Vec<T, 3> zero;
        static const Vec<T, 3> unitX;
        static const Vec<T, 3> unitY;
        static const Vec<T, 3> unitZ;
        static const Vec<T, 3> unit;
        static const Vec<T, 3> negaUnitX;
        static const Vec<T, 3> negaUnitY;
        static const Vec<T, 3> negaUnitZ;
        static const Vec<T, 3> negaUnit;
    };

    template <typename T>
    struct VecConsts<T, 4> {
        static const Vec<T, 4> zero;
        static const Vec<T, 4> unitX;
        static const Vec<T, 4> unitY;
        static const Vec<T, 4> unitZ;
        static const Vec<T, 4> unitW;
        static const Vec<T, 4> unit;
        static const Vec<T, 4> negaUnitX;
        static const Vec<T, 4> negaUnitY;
        static const Vec<T, 4> negaUintZ;
        static const Vec<T, 4> negaUnitW;
        static const Vec<T, 4> negaUnit;
    };

    using BVec1 = Vec<bool, 1>;
    using BVec2 = Vec<bool, 2>;
    using BVec3 = Vec<bool, 3>;
    using BVec4 = Vec<bool, 4>;

    using IVec1 = Vec<int32_t, 1>;
    using IVec2 = Vec<int32_t, 2>;
    using IVec3 = Vec<int32_t, 3>;
    using IVec4 = Vec<int32_t, 4>;

    using UVec1 = Vec<uint32_t, 1>;
    using UVec2 = Vec<uint32_t, 2>;
    using UVec3 = Vec<uint32_t, 3>;
    using UVec4 = Vec<uint32_t, 4>;

    using HVec1 = Vec<HFloat, 1>;
    using HVec2 = Vec<HFloat, 2>;
    using HVec3 = Vec<HFloat, 3>;
    using HVec4 = Vec<HFloat, 4>;

    using FVec1 = Vec<float, 1>;
    using FVec2 = Vec<float, 2>;
    using FVec3 = Vec<float, 3>;
    using FVec4 = Vec<float, 4>;

    using DVec1 = Vec<double, 1>;
    using DVec2 = Vec<double, 2>;
    using DVec3 = Vec<double, 3>;
    using DVec4 = Vec<double, 4>;

    using BVec1Consts = VecConsts<bool, 1>;
    using BVec2Consts = VecConsts<bool, 2>;
    using BVec3Consts = VecConsts<bool, 3>;
    using BVec4Consts = VecConsts<bool, 4>;

    using IVec1Consts = VecConsts<int32_t, 1>;
    using IVec2Consts = VecConsts<int32_t, 2>;
    using IVec3Consts = VecConsts<int32_t, 3>;
    using IVec4Consts = VecConsts<int32_t, 4>;

    using UVec1Consts = VecConsts<uint32_t, 1>;
    using UVec2Consts = VecConsts<uint32_t, 2>;
    using UVec3Consts = VecConsts<uint32_t, 3>;
    using UVec4Consts = VecConsts<uint32_t, 4>;

    using HVec1Consts = VecConsts<HFloat, 1>;
    using HVec2Consts = VecConsts<HFloat, 2>;
    using HVec3Consts = VecConsts<HFloat, 3>;
    using HVec4Consts = VecConsts<HFloat, 4>;

    using FVec1Consts = VecConsts<float, 1>;
    using FVec2Consts = VecConsts<float, 2>;
    using FVec3Consts = VecConsts<float, 3>;
    using FVec4Consts = VecConsts<float, 4>;

    using DVec1Consts = VecConsts<double, 1>;
    using DVec2Consts = VecConsts<double, 2>;
    using DVec3Consts = VecConsts<double, 3>;
    using DVec4Consts = VecConsts<double, 4>;
}

namespace Common::Internal {
    template <typename VecT, typename SubVecT, uint8_t... VecIndex, size_t... SubVecIndex>
    static void CopyValueToSubVec(const VecT& vec, SubVecT& subVec, std::index_sequence<SubVecIndex...>)
    {
        static_assert(sizeof...(SubVecIndex) == sizeof...(VecIndex));
        (void) std::initializer_list<int> { ([&]() -> void { subVec.data[SubVecIndex] = vec.data[VecIndex]; }(), 0)... };
    }

    template <typename T>
    struct VecCrossResultTraits<T, 2> {
        using Type = T;
    };

    template <typename T>
    struct VecCrossResultTraits<T, 3> {
        using Type = Vec<T, 3>;
    };
}

namespace Common {
    template <Serializable T, uint8_t L>
    struct Serializer<Vec<T, L>> {
        static constexpr uint32_t typeId
            = Common::HashUtils::StrCrc32("Common::Vector")
            + Serializer<T>::typeId
            + L;

        static void Serialize(SerializeStream& stream, const Vec<T, L>& value)
        {
            TypeIdSerializer<Vec<T, L>>::Serialize(stream);

            for (auto i = 0; i < L; i++) {
                Serializer<T>::Serialize(stream, value.data[i]);
            }
        }

        static bool Deserialize(DeserializeStream& stream, Vec<T, L>& value)
        {
            if (!TypeIdSerializer<Vec<T, L>>::Deserialize(stream)) {
                return false;
            }

            for (auto i = 0; i < L; i++) {
                Serializer<T>::Deserialize(stream, value.data[i]);
            }
            return true;
        }
    };

    template <StringConvertible T, uint8_t L>
    struct StringConverter<Vec<T, L>> {
        static std::string ToString(const Vec<T, L>& inValue)
        {
            std::stringstream stream;
            stream << "(";
            for (auto i = 0; i < L; i++) {
                stream << inValue.data[i];
                if (i != L - 1) {
                    stream << ", ";
                }
            }
            stream << ")";
            return stream.str();
        }
    };

    template <JsonSerializable T, uint8_t L>
    struct JsonSerializer<Vec<T, L>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Vec<T, L>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(L, inAllocator);
            for (auto i = 0; i < L; i++) {
                rapidjson::Value elementJson;
                JsonSerializer<T>::JsonSerialize(elementJson, inAllocator, inValue[i]);
                outJsonValue.PushBack(elementJson, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Vec<T, L>& outValue)
        {
            for (auto i = 0; i < L; i++) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], outValue[i]);
            }
        }
    };
}

namespace Common {
    template <typename T>
    struct BaseVec<T, 1> {
        BaseVec();
        BaseVec(T inX); // NOLINT

        union {
            T data[1];
            struct {
                T x;
            };
        };
    };

    template <typename T>
    struct BaseVec<T, 2> {
        BaseVec();
        BaseVec(T inValue); // NOLINT
        BaseVec(T inX, T inY);

        union {
            T data[2];
            struct {
                T x;
                T y;
            };
        };
    };

    template <typename T>
    struct BaseVec<T, 3> {
        BaseVec();
        BaseVec(T inValue); // NOLINT
        BaseVec(T inX, T inY, T inZ);

        union {
            T data[3];
            struct {
                T x;
                T y;
                T z;
            };
        };
    };

    template <typename T>
    struct BaseVec<T, 4> {
        BaseVec();
        BaseVec(T inValue); // NOLINT
        BaseVec(T inX, T inY, T inZ, T inW);

        union {
            T data[4];
            struct {
                T x;
                T y;
                T z;
                T w;
            };
        };
    };

    template <typename T>
    BaseVec<T, 1>::BaseVec()
        : x(0)
    {
    }

    template <typename T>
    BaseVec<T, 1>::BaseVec(T inX)
        : x(inX)
    {
    }

    template <typename T>
    BaseVec<T, 2>::BaseVec()
        : x(0)
    {
    }

    template <typename T>
    BaseVec<T, 2>::BaseVec(T inValue)
        : x(inValue), y(inValue)
    {
    }

    template <typename T>
    BaseVec<T, 2>::BaseVec(T inX, T inY)
        : x(inX), y(inY)
    {
    }

    template <typename T>
    BaseVec<T, 3>::BaseVec()
        : x(0)
    {
    }

    template <typename T>
    BaseVec<T, 3>::BaseVec(T inValue)
        : x(inValue), y(inValue), z(inValue)
    {
    }

    template <typename T>
    BaseVec<T, 3>::BaseVec(T inX, T inY, T inZ)
        : x(inX), y(inY), z(inZ)
    {
    }

    template <typename T>
    BaseVec<T, 4>::BaseVec()
        : x(0)
    {
    }

    template <typename T>
    BaseVec<T, 4>::BaseVec(T inValue)
        : x(inValue), y(inValue), z(inValue), w(inValue)
    {
    }

    template <typename T>
    BaseVec<T, 4>::BaseVec(T inX, T inY, T inZ, T inW)
        : x(inX), y(inY), z(inZ), w(inW)
    {
    }

    template <typename T>
    const Vec<T, 1> VecConsts<T, 1>::zero = Vec<T, 1>();

    template <typename T>
    const Vec<T, 1> VecConsts<T, 1>::unit = Vec<T, 1>(1);

    template <typename T>
    const Vec<T, 1> VecConsts<T, 1>::negaUnit = Vec<T, 1>(-1);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::zero = Vec<T, 2>();

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::unitX = Vec<T, 2>(1, 0);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::unitY = Vec<T, 2>(0, 1);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::unit = Vec<T, 2>(1, 1);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::negaUnitX = Vec<T, 2>(-1, 0);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::negaUnitY = Vec<T, 2>(0, -1);

    template <typename T>
    const Vec<T, 2> VecConsts<T, 2>::negaUnit = Vec<T, 2>(-1, -1);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::zero = Vec<T, 3>();

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::unitX = Vec<T, 3>(1, 0, 0);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::unitY = Vec<T, 3>(0, 1, 0);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::unitZ = Vec<T, 3>(0, 0, 1);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::unit = Vec<T, 3>(1, 1, 1);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::negaUnitX = Vec<T, 3>(-1, 0, 0);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::negaUnitY = Vec<T, 3>(0, -1, 0);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::negaUnitZ = Vec<T, 3>(0, 0, -1);

    template <typename T>
    const Vec<T, 3> VecConsts<T, 3>::negaUnit = Vec<T, 3>(-1, -1, -1);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::zero = Vec<T, 4>();

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::unitX = Vec<T, 4>(1, 0, 0, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::unitY = Vec<T, 4>(0, 1, 0, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::unitZ = Vec<T, 4>(0, 0, 1, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::unitW = Vec<T, 4>(0, 0, 0, 1);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::unit = Vec<T, 4>(1, 1, 1, 1);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::negaUnitX = Vec<T, 4>(-1, 0, 0, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::negaUnitY = Vec<T, 4>(0, -1, 0, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::negaUintZ = Vec<T, 4>(0, 0, -1, 0);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::negaUnitW = Vec<T, 4>(0, 0, 0, -1);

    template <typename T>
    const Vec<T, 4> VecConsts<T, 4>::negaUnit = Vec<T, 4>(-1, -1, -1, -1);

    template<typename T, uint8_t L>
    Vec<T, L>::Vec() : BaseVec<T, L>(0)
    {
    }

    template<typename T, uint8_t L>
    Vec<T, L>::Vec(T inValue) : BaseVec<T, L>(inValue)
    {
    }

    template <typename T, uint8_t L>
    Vec<T, L>::Vec(const Vec& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
    }

    template <typename T, uint8_t L>
    Vec<T, L>::Vec(Vec&& other) noexcept
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = std::move(other.data[i]);
        }
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator=(const Vec& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    template <typename... IT>
    Vec<T, L>::Vec(IT&&... inValues) : BaseVec<T, L>(std::forward<IT>(inValues)...)
    {
    }

    template <typename T, uint8_t L>
    T& Vec<T, L>::operator[](uint32_t i)
    {
        return this->data[i];
    }

    template <typename T, uint8_t L>
    T Vec<T, L>::operator[](uint32_t i) const
    {
        return this->data[i];
    }

    template <typename T, uint8_t L>
    bool Vec<T, L>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t L>
    bool Vec<T, L>::operator==(const Vec& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L>
    bool Vec<T, L>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L>
    bool Vec<T, L>::operator!=(const Vec& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator+(T rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator-(T rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator*(T rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator/(T rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] / rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator+(const Vec& rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator-(const Vec& rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator*(const Vec& rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::operator/(const Vec& rhs) const
    {
        Vec<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] / rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator+=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] += rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator-=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] -= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator*=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] *= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator/=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] /= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator+=(const Vec& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] += rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator-=(const Vec& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] -= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator*=(const Vec& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] *= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vec<T, L>& Vec<T, L>::operator/=(const Vec& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] /= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    template <typename IT>
    Vec<IT, L> Vec<T, L>::CastTo() const
    {
        Vec<IT, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L>
    template <uint8_t... I>
    Vec<T, sizeof...(I)> Vec<T, L>::SubVec() const
    {
        Vec<T, sizeof...(I)> result;
        Internal::CopyValueToSubVec<Vec<T, L>, Vec<T, sizeof...(I)>, I...>(*this, result, std::make_index_sequence<sizeof...(I)> {});
        return result;
    }

    template <typename T, uint8_t L>
    T Vec<T, L>::Model() const
    {
        static_assert(FloatingPoint<T>);
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += this->data[i] * this->data[i];
        }
        return std::sqrt(temp);
    }

    template <typename T, uint8_t L>
    Vec<T, L> Vec<T, L>::Normalized() const
    {
        return this->operator/(Model());
    }

    template <typename T, uint8_t L>
    void Vec<T, L>::Normalize()
    {
        T oneOverModel = static_cast<T>(1.0) / Model();
        for (auto i = 0; i < L; i++) {
            this->data[i] *= oneOverModel;
        }
    }

    template <typename T, uint8_t L>
    T Vec<T, L>::Dot(const Vec& rhs) const
    {
        static_assert(FloatingPoint<T>);
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += this->data[i] * rhs.data[i];
        }
        return temp;
    }

    template <typename T, uint8_t L>
    typename Internal::VecCrossResultTraits<T, L>::Type Vec<T, L>::Cross(const Vec& rhs) const
    {
        static_assert(FloatingPoint<T> && L >= 2 && L <= 3);
        typename Internal::VecCrossResultTraits<T, L>::Type result;
        if constexpr (L == 2) {
            result = this->x * rhs.y - this->y * rhs.x;
        } else {
            result.x = this->y * rhs.z - this->z * rhs.y;
            result.y = this->z * rhs.x - this->x * rhs.z;
            result.z = this->x * rhs.y - this->y * rhs.x;
        }
        return result;
    }
}
