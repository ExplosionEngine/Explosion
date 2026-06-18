//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <utility>

#include <Common/Math/Simd.h>
#include <Common/Math/Half.h>
#include <Common/Serialization.h>
#include <Common/String.h>

namespace Common {
    template <typename T, uint8_t L, MathBackend B = MathBackend::defaultBackend> struct Vec;
}

namespace Common::Internal {
    template <typename T, uint8_t L, MathBackend B>
    struct VecCrossResultTraits {
        using Type = T;
    };
}

namespace Common {
    template <uint8_t L> concept ValidVecDim = L >= 1 && L <= 4;
    template <typename CheckT, typename VT, uint8_t L, MathBackend B> concept VecN = std::is_same_v<CheckT, Vec<VT, L, B>>;

    template <typename T, uint8_t L, MathBackend B>
    requires ValidVecDim<L>
    struct BaseVec {};

    template <typename T, uint8_t L, MathBackend B>
    struct Vec : BaseVec<T, L, B> {
        using Type = T;
        static constexpr uint8_t dims = L;
        static constexpr MathBackend backend = B;

        Vec();
        Vec(T inValue); // NOLINT
        // Not defaulted: BaseVec aliases data via an anonymous union, and a union's copy/move members are implicitly
        // deleted when a variant member (e.g. HFloat) has a non-trivial copy, so these stay element-wise.
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
        Vec<IT, L, B> CastTo() const;

        template <uint8_t... I>
        Vec<T, sizeof...(I), B> SubVec() const;

        T Model() const;
        Vec Normalized() const;
        void Normalize();
        T Dot(const Vec& rhs) const;
        typename Internal::VecCrossResultTraits<T, L, B>::Type Cross(const Vec& rhs) const;
    };

    template <typename T, uint8_t L, MathBackend B = MathBackend::defaultBackend>
    requires ValidVecDim<L>
    struct VecConsts {};

    template <typename T, MathBackend B>
    struct VecConsts<T, 1, B> {
        static const Vec<T, 1, B> zero;
        static const Vec<T, 1, B> unit;
        static const Vec<T, 1, B> negaUnit;
    };

    template <typename T, MathBackend B>
    struct VecConsts<T, 2, B> {
        static const Vec<T, 2, B> zero;
        static const Vec<T, 2, B> unitX;
        static const Vec<T, 2, B> unitY;
        static const Vec<T, 2, B> unit;
        static const Vec<T, 2, B> negaUnitX;
        static const Vec<T, 2, B> negaUnitY;
        static const Vec<T, 2, B> negaUnit;
    };

    template <typename T, MathBackend B>
    struct VecConsts<T, 3, B> {
        static const Vec<T, 3, B> zero;
        static const Vec<T, 3, B> unitX;
        static const Vec<T, 3, B> unitY;
        static const Vec<T, 3, B> unitZ;
        static const Vec<T, 3, B> unit;
        static const Vec<T, 3, B> negaUnitX;
        static const Vec<T, 3, B> negaUnitY;
        static const Vec<T, 3, B> negaUnitZ;
        static const Vec<T, 3, B> negaUnit;
    };

    template <typename T, MathBackend B>
    struct VecConsts<T, 4, B> {
        static const Vec<T, 4, B> zero;
        static const Vec<T, 4, B> unitX;
        static const Vec<T, 4, B> unitY;
        static const Vec<T, 4, B> unitZ;
        static const Vec<T, 4, B> unitW;
        static const Vec<T, 4, B> unit;
        static const Vec<T, 4, B> negaUnitX;
        static const Vec<T, 4, B> negaUnitY;
        static const Vec<T, 4, B> negaUnitZ;
        static const Vec<T, 4, B> negaUnitW;
        static const Vec<T, 4, B> negaUnit;
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

    template <typename T, MathBackend B>
    struct VecCrossResultTraits<T, 2, B> {
        using Type = T;
    };

    template <typename T, MathBackend B>
    struct VecCrossResultTraits<T, 3, B> {
        using Type = Vec<T, 3, B>;
    };
}

namespace Common {
    template <Serializable T, uint8_t L, MathBackend B>
    struct Serializer<Vec<T, L, B>> {
        static constexpr size_t typeId
            = Common::HashUtils::StrCrc32("Common::Vector")
            + Serializer<T>::typeId
            + L;

        static size_t Serialize(BinarySerializeStream& stream, const Vec<T, L, B>& value)
        {
            size_t serialized = 0;
            for (auto i = 0; i < L; i++) {
                serialized += Serializer<T>::Serialize(stream, value.data[i]);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Vec<T, L, B>& value)
        {
            size_t deserialized = 0;
            for (auto i = 0; i < L; i++) {
                deserialized += Serializer<T>::Deserialize(stream, value.data[i]);
            }
            return deserialized;
        }
    };

    template <StringConvertible T, uint8_t L, MathBackend B>
    struct StringConverter<Vec<T, L, B>> {
        static std::string ToString(const Vec<T, L, B>& inValue)
        {
            std::stringstream stream;
            stream << "(";
            for (auto i = 0; i < L; i++) {
                stream << StringConverter<T>::ToString(inValue.data[i]);
                if (i != L - 1) {
                    stream << ", ";
                }
            }
            stream << ")";
            return stream.str();
        }
    };

    template <JsonSerializable T, uint8_t L, MathBackend B>
    struct JsonSerializer<Vec<T, L, B>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Vec<T, L, B>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(L, inAllocator);
            for (auto i = 0; i < L; i++) {
                rapidjson::Value elementJson;
                JsonSerializer<T>::JsonSerialize(elementJson, inAllocator, inValue[i]);
                outJsonValue.PushBack(elementJson, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Vec<T, L, B>& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != L) {
                return;
            }
            for (auto i = 0; i < L; i++) {
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], outValue[i]);
            }
        }
    };
}

namespace Common {
    template <typename T, MathBackend B>
    struct BaseVec<T, 1, B> {
        BaseVec();
        BaseVec(T inX); // NOLINT

        union {
            T data[1];
            struct {
                T x;
            };
        };
    };

    template <typename T, MathBackend B>
    struct BaseVec<T, 2, B> {
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

    template <typename T, MathBackend B>
    struct BaseVec<T, 3, B> {
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

    template <typename T, MathBackend B>
    struct BaseVec<T, 4, B> {
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

    template <typename T, MathBackend B>
    BaseVec<T, 1, B>::BaseVec()
        : x(0)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 1, B>::BaseVec(T inX)
        : x(inX)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 2, B>::BaseVec()
        : x(0)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 2, B>::BaseVec(T inValue)
        : x(inValue), y(inValue)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 2, B>::BaseVec(T inX, T inY)
        : x(inX), y(inY)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 3, B>::BaseVec()
        : x(0)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 3, B>::BaseVec(T inValue)
        : x(inValue), y(inValue), z(inValue)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 3, B>::BaseVec(T inX, T inY, T inZ)
        : x(inX), y(inY), z(inZ)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 4, B>::BaseVec()
        : x(0)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 4, B>::BaseVec(T inValue)
        : x(inValue), y(inValue), z(inValue), w(inValue)
    {
    }

    template <typename T, MathBackend B>
    BaseVec<T, 4, B>::BaseVec(T inX, T inY, T inZ, T inW)
        : x(inX), y(inY), z(inZ), w(inW)
    {
    }

    template <typename T, MathBackend B>
    const Vec<T, 1, B> VecConsts<T, 1, B>::zero = Vec<T, 1, B>();

    template <typename T, MathBackend B>
    const Vec<T, 1, B> VecConsts<T, 1, B>::unit = Vec<T, 1, B>(1);

    template <typename T, MathBackend B>
    const Vec<T, 1, B> VecConsts<T, 1, B>::negaUnit = Vec<T, 1, B>(-1);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::zero = Vec<T, 2, B>();

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::unitX = Vec<T, 2, B>(1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::unitY = Vec<T, 2, B>(0, 1);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::unit = Vec<T, 2, B>(1, 1);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::negaUnitX = Vec<T, 2, B>(-1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::negaUnitY = Vec<T, 2, B>(0, -1);

    template <typename T, MathBackend B>
    const Vec<T, 2, B> VecConsts<T, 2, B>::negaUnit = Vec<T, 2, B>(-1, -1);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::zero = Vec<T, 3, B>();

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::unitX = Vec<T, 3, B>(1, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::unitY = Vec<T, 3, B>(0, 1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::unitZ = Vec<T, 3, B>(0, 0, 1);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::unit = Vec<T, 3, B>(1, 1, 1);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::negaUnitX = Vec<T, 3, B>(-1, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::negaUnitY = Vec<T, 3, B>(0, -1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::negaUnitZ = Vec<T, 3, B>(0, 0, -1);

    template <typename T, MathBackend B>
    const Vec<T, 3, B> VecConsts<T, 3, B>::negaUnit = Vec<T, 3, B>(-1, -1, -1);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::zero = Vec<T, 4, B>();

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::unitX = Vec<T, 4, B>(1, 0, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::unitY = Vec<T, 4, B>(0, 1, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::unitZ = Vec<T, 4, B>(0, 0, 1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::unitW = Vec<T, 4, B>(0, 0, 0, 1);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::unit = Vec<T, 4, B>(1, 1, 1, 1);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::negaUnitX = Vec<T, 4, B>(-1, 0, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::negaUnitY = Vec<T, 4, B>(0, -1, 0, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::negaUnitZ = Vec<T, 4, B>(0, 0, -1, 0);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::negaUnitW = Vec<T, 4, B>(0, 0, 0, -1);

    template <typename T, MathBackend B>
    const Vec<T, 4, B> VecConsts<T, 4, B>::negaUnit = Vec<T, 4, B>(-1, -1, -1, -1);
}

namespace Common::Internal {
    // Per-backend dispatch for the hot element-wise arithmetic. The primary template is the original scalar loop, so
    // any (T, L, B) without a dedicated specialization degrades gracefully to scalar (this includes B == simd for
    // types/dims that have no SIMD path). This block sits after the BaseVec<T, 4, B> specialization because the SIMD
    // specialization below is a full (non-template) specialization whose member bodies eagerly need Vec<float, 4, B>'s
    // `data`.
    template <typename T, uint8_t L, MathBackend B>
    struct VecOps {
        static Vec<T, L, B> Add(const Vec<T, L, B>& a, const Vec<T, L, B>& b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] + b.data[i]; }
            return result;
        }

        static Vec<T, L, B> Sub(const Vec<T, L, B>& a, const Vec<T, L, B>& b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] - b.data[i]; }
            return result;
        }

        static Vec<T, L, B> Mul(const Vec<T, L, B>& a, const Vec<T, L, B>& b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] * b.data[i]; }
            return result;
        }

        static Vec<T, L, B> Div(const Vec<T, L, B>& a, const Vec<T, L, B>& b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] / b.data[i]; }
            return result;
        }

        static Vec<T, L, B> AddScalar(const Vec<T, L, B>& a, T b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] + b; }
            return result;
        }

        static Vec<T, L, B> SubScalar(const Vec<T, L, B>& a, T b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] - b; }
            return result;
        }

        static Vec<T, L, B> MulScalar(const Vec<T, L, B>& a, T b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] * b; }
            return result;
        }

        static Vec<T, L, B> DivScalar(const Vec<T, L, B>& a, T b)
        {
            Vec<T, L, B> result;
            for (auto i = 0; i < L; i++) { result.data[i] = a.data[i] / b; }
            return result;
        }

        static T Dot(const Vec<T, L, B>& a, const Vec<T, L, B>& b)
        {
            T result = 0;
            for (auto i = 0; i < L; i++) { result += a.data[i] * b.data[i]; }
            return result;
        }
    };

    // Vec<float, 4, simd> is backed by float[4] (16 bytes), so unaligned 128-bit loads/stores stay in bounds. Vec3 is
    // intentionally left to the scalar primary template: its float[3] storage cannot be loaded with a 128-bit load
    // without reading out of bounds, and a masked load would not beat the scalar loop.
    template <>
    struct VecOps<float, 4, MathBackend::simd> {
        using V = Vec<float, 4, MathBackend::simd>;

        static V Add(const V& a, const V& b) { V r; Simd::MapBinary<4>(r.data, a.data, b.data, Simd::AddOp {}); return r; }
        static V Sub(const V& a, const V& b) { V r; Simd::MapBinary<4>(r.data, a.data, b.data, Simd::SubOp {}); return r; }
        static V Mul(const V& a, const V& b) { V r; Simd::MapBinary<4>(r.data, a.data, b.data, Simd::MulOp {}); return r; }
        static V Div(const V& a, const V& b) { V r; Simd::MapBinary<4>(r.data, a.data, b.data, Simd::DivOp {}); return r; }

        static V AddScalar(const V& a, float b) { V r; Simd::MapScalar<4>(r.data, a.data, b, Simd::AddOp {}); return r; }
        static V SubScalar(const V& a, float b) { V r; Simd::MapScalar<4>(r.data, a.data, b, Simd::SubOp {}); return r; }
        static V MulScalar(const V& a, float b) { V r; Simd::MapScalar<4>(r.data, a.data, b, Simd::MulOp {}); return r; }
        static V DivScalar(const V& a, float b) { V r; Simd::MapScalar<4>(r.data, a.data, b, Simd::DivOp {}); return r; }

        static float Dot(const V& a, const V& b)
        {
            return Simd::Sum(Simd::Mul(Simd::LoadU(a.data), Simd::LoadU(b.data)));
        }
    };
}

namespace Common {
    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>::Vec() : BaseVec<T, L, B>(0)
    {
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>::Vec(T inValue) : BaseVec<T, L, B>(inValue)
    {
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>::Vec(const Vec& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>::Vec(Vec&& other) noexcept
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = std::move(other.data[i]);
        }
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator=(const Vec& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    template <typename... IT>
    Vec<T, L, B>::Vec(IT&&... inValues) : BaseVec<T, L, B>(std::forward<IT>(inValues)...)
    {
    }

    template <typename T, uint8_t L, MathBackend B>
    T& Vec<T, L, B>::operator[](uint32_t i)
    {
        return this->data[i];
    }

    template <typename T, uint8_t L, MathBackend B>
    T Vec<T, L, B>::operator[](uint32_t i) const
    {
        return this->data[i];
    }

    template <typename T, uint8_t L, MathBackend B>
    bool Vec<T, L, B>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t L, MathBackend B>
    bool Vec<T, L, B>::operator==(const Vec& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L, MathBackend B>
    bool Vec<T, L, B>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    bool Vec<T, L, B>::operator!=(const Vec& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator+(T rhs) const
    {
        return Internal::VecOps<T, L, B>::AddScalar(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator-(T rhs) const
    {
        return Internal::VecOps<T, L, B>::SubScalar(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator*(T rhs) const
    {
        return Internal::VecOps<T, L, B>::MulScalar(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator/(T rhs) const
    {
        return Internal::VecOps<T, L, B>::DivScalar(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator+(const Vec& rhs) const
    {
        return Internal::VecOps<T, L, B>::Add(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator-(const Vec& rhs) const
    {
        return Internal::VecOps<T, L, B>::Sub(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator*(const Vec& rhs) const
    {
        return Internal::VecOps<T, L, B>::Mul(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::operator/(const Vec& rhs) const
    {
        return Internal::VecOps<T, L, B>::Div(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator+=(T rhs)
    {
        *this = Internal::VecOps<T, L, B>::AddScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator-=(T rhs)
    {
        *this = Internal::VecOps<T, L, B>::SubScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator*=(T rhs)
    {
        *this = Internal::VecOps<T, L, B>::MulScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator/=(T rhs)
    {
        *this = Internal::VecOps<T, L, B>::DivScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator+=(const Vec& rhs)
    {
        *this = Internal::VecOps<T, L, B>::Add(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator-=(const Vec& rhs)
    {
        *this = Internal::VecOps<T, L, B>::Sub(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator*=(const Vec& rhs)
    {
        *this = Internal::VecOps<T, L, B>::Mul(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B>& Vec<T, L, B>::operator/=(const Vec& rhs)
    {
        *this = Internal::VecOps<T, L, B>::Div(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t L, MathBackend B>
    template <typename IT>
    Vec<IT, L, B> Vec<T, L, B>::CastTo() const
    {
        Vec<IT, L, B> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L, MathBackend B>
    template <uint8_t... I>
    Vec<T, sizeof...(I), B> Vec<T, L, B>::SubVec() const
    {
        Vec<T, sizeof...(I), B> result;
        Internal::CopyValueToSubVec<Vec<T, L, B>, Vec<T, sizeof...(I), B>, I...>(*this, result, std::make_index_sequence<sizeof...(I)> {});
        return result;
    }

    template <typename T, uint8_t L, MathBackend B>
    T Vec<T, L, B>::Model() const
    {
        static_assert(FloatingPoint<T>);
        return std::sqrt(Internal::VecOps<T, L, B>::Dot(*this, *this));
    }

    template <typename T, uint8_t L, MathBackend B>
    Vec<T, L, B> Vec<T, L, B>::Normalized() const
    {
        return this->operator/(Model());
    }

    template <typename T, uint8_t L, MathBackend B>
    void Vec<T, L, B>::Normalize()
    {
        T oneOverModel = static_cast<T>(1.0) / Model();
        *this = Internal::VecOps<T, L, B>::MulScalar(*this, oneOverModel);
    }

    template <typename T, uint8_t L, MathBackend B>
    T Vec<T, L, B>::Dot(const Vec& rhs) const
    {
        static_assert(FloatingPoint<T>);
        return Internal::VecOps<T, L, B>::Dot(*this, rhs);
    }

    template <typename T, uint8_t L, MathBackend B>
    typename Internal::VecCrossResultTraits<T, L, B>::Type Vec<T, L, B>::Cross(const Vec& rhs) const
    {
        static_assert(FloatingPoint<T> && L >= 2 && L <= 3);
        typename Internal::VecCrossResultTraits<T, L, B>::Type result;
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
