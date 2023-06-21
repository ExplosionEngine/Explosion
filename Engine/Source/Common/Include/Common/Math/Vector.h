//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <utility>

#include <Common/Math/Half.h>

namespace Common::Internal {
    template <typename T, uint8_t L>
    struct VecCrossResultTraits {
        using Type = T;
    };
}

namespace Common {
    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    struct BaseVector {};

    template <typename T, uint8_t L>
    struct Vector : public BaseVector<T, L> {
        using Type = T;
        static constexpr uint8_t dims = L;

        inline Vector();
        inline Vector(T inValue); // NOLINT
        inline Vector(const Vector& other);
        inline Vector(Vector&& other) noexcept;
        inline Vector& operator=(const Vector& other);

        template <typename... IT>
        inline Vector(IT&&... inValues); // NOLINT

        inline T& operator[](uint32_t i);
        inline T operator[](uint32_t i) const;

        inline bool operator==(T rhs) const;
        inline bool operator==(const Vector& rhs) const;
        inline bool operator!=(T rhs) const;
        inline bool operator!=(const Vector& rhs) const;

        inline Vector operator+(T rhs) const;
        inline Vector operator-(T rhs) const;
        inline Vector operator*(T rhs) const;
        inline Vector operator/(T rhs) const;

        inline Vector operator+(const Vector& rhs) const;
        inline Vector operator-(const Vector& rhs) const;
        inline Vector operator*(const Vector& rhs) const;
        inline Vector operator/(const Vector& rhs) const;

        inline Vector& operator+=(T rhs);
        inline Vector& operator-=(T rhs);
        inline Vector& operator*=(T rhs);
        inline Vector& operator/=(T rhs);

        inline Vector& operator+=(const Vector& rhs);
        inline Vector& operator-=(const Vector& rhs);
        inline Vector& operator*=(const Vector& rhs);
        inline Vector& operator/=(const Vector& rhs);

        template <typename IT>
        inline Vector<IT, L> CastTo() const;

        template <uint8_t... I>
        inline Vector<T, sizeof...(I)> SubVec() const;

        inline T Model() const;
        inline Vector Normalized() const;
        inline T Dot(const Vector& rhs) const;
        inline typename Internal::VecCrossResultTraits<T, L>::Type Cross(const Vector& rhs) const;
    };

    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    struct VecConsts {};

    template <typename T>
    struct VecConsts<T, 1> {
        static const Vector<T, 1> zero;
        static const Vector<T, 1> unit;
    };

    template <typename T>
    struct VecConsts<T, 2> {
        static const Vector<T, 2> zero;
        static const Vector<T, 2> unitX;
        static const Vector<T, 2> unitY;
        static const Vector<T, 2> unit;
    };

    template <typename T>
    struct VecConsts<T, 3> {
        static const Vector<T, 3> zero;
        static const Vector<T, 3> unitX;
        static const Vector<T, 3> unitY;
        static const Vector<T, 3> unitZ;
        static const Vector<T, 3> unit;
    };

    template <typename T>
    struct VecConsts<T, 4> {
        static const Vector<T, 4> zero;
        static const Vector<T, 4> unitX;
        static const Vector<T, 4> unitY;
        static const Vector<T, 4> unitZ;
        static const Vector<T, 4> unitW;
        static const Vector<T, 4> unit;
    };

    using BVec1 = Vector<bool, 1>;
    using BVec2 = Vector<bool, 2>;
    using BVec3 = Vector<bool, 3>;
    using BVec4 = Vector<bool, 4>;

    using IVec1 = Vector<int32_t, 1>;
    using IVec2 = Vector<int32_t, 2>;
    using IVec3 = Vector<int32_t, 3>;
    using IVec4 = Vector<int32_t, 4>;

    using HVec1 = Vector<HFloat, 1>;
    using HVec2 = Vector<HFloat, 2>;
    using HVec3 = Vector<HFloat, 3>;
    using HVec4 = Vector<HFloat, 4>;

    using FVec1 = Vector<float, 1>;
    using FVec2 = Vector<float, 2>;
    using FVec3 = Vector<float, 3>;
    using FVec4 = Vector<float, 4>;

    using DVec1 = Vector<double, 1>;
    using DVec2 = Vector<double, 2>;
    using DVec3 = Vector<double, 3>;
    using DVec4 = Vector<double, 4>;

    using BVec1Consts = VecConsts<bool, 1>;
    using BVec2Consts = VecConsts<bool, 2>;
    using BVec3Consts = VecConsts<bool, 3>;
    using BVec4Consts = VecConsts<bool, 4>;

    using IVec1Consts = VecConsts<int32_t, 1>;
    using IVec2Consts = VecConsts<int32_t, 2>;
    using IVec3Consts = VecConsts<int32_t, 3>;
    using IVec4Consts = VecConsts<int32_t, 4>;

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
        using Type = Vector<T, 3>;
    };
}

namespace Common {
    template <typename T>
    struct BaseVector<T, 1> {
        inline BaseVector();
        inline BaseVector(T inX); // NOLINT

        union {
            T data[1];
            struct {
                T x;
            };
        };
    };

    template <typename T>
    struct BaseVector<T, 2> {
        inline BaseVector();
        inline BaseVector(T inValue); // NOLINT
        inline BaseVector(T inX, T inY);

        union {
            T data[2];
            struct {
                T x;
                T y;
            };
        };
    };

    template <typename T>
    struct BaseVector<T, 3> {
        inline BaseVector();
        inline BaseVector(T inValue); // NOLINT
        inline BaseVector(T inX, T inY, T inZ);

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
    struct BaseVector<T, 4> {
        inline BaseVector();
        inline BaseVector(T inValue); // NOLINT
        inline BaseVector(T inX, T inY, T inZ, T inW);

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

    template<typename T>
    BaseVector<T, 1>::BaseVector()
        : x(0)
    {
    }

    template<typename T>
    BaseVector<T, 1>::BaseVector(T inX)
        : x(inX)
    {
    }

    template<typename T>
    BaseVector<T, 2>::BaseVector()
        : x(0)
    {
    }

    template<typename T>
    BaseVector<T, 2>::BaseVector(T inValue)
        : x(inValue), y(inValue)
    {
    }

    template<typename T>
    BaseVector<T, 2>::BaseVector(T inX, T inY)
        : x(inX), y(inY)
    {
    }

    template<typename T>
    BaseVector<T, 3>::BaseVector()
        : x(0)
    {
    }

    template<typename T>
    BaseVector<T, 3>::BaseVector(T inValue)
        : x(inValue), y(inValue), z(inValue)
    {
    }

    template<typename T>
    BaseVector<T, 3>::BaseVector(T inX, T inY, T inZ)
        : x(inX), y(inY), z(inZ)
    {
    }

    template<typename T>
    BaseVector<T, 4>::BaseVector()
        : x(0)
    {
    }

    template<typename T>
    BaseVector<T, 4>::BaseVector(T inValue)
        : x(inValue), y(inValue), z(inValue), w(inValue)
    {
    }

    template<typename T>
    BaseVector<T, 4>::BaseVector(T inX, T inY, T inZ, T inW)
        : x(inX), y(inY), z(inZ), w(inW)
    {
    }

    template <typename T>
    const Vector<T, 1> VecConsts<T, 1>::zero = Vector<T, 1>();

    template <typename T>
    const Vector<T, 1> VecConsts<T, 1>::unit = Vector<T, 1>(1);

    template <typename T>
    const Vector<T, 2> VecConsts<T, 2>::zero = Vector<T, 2>();

    template <typename T>
    const Vector<T, 2> VecConsts<T, 2>::unitX = Vector<T, 2>(1, 0);

    template <typename T>
    const Vector<T, 2> VecConsts<T, 2>::unitY = Vector<T, 2>(0, 1);

    template <typename T>
    const Vector<T, 2> VecConsts<T, 2>::unit = Vector<T, 2>(1, 1);

    template <typename T>
    const Vector<T, 3> VecConsts<T, 3>::zero = Vector<T, 3>();

    template <typename T>
    const Vector<T, 3> VecConsts<T, 3>::unitX = Vector<T, 3>(1, 0, 0);

    template <typename T>
    const Vector<T, 3> VecConsts<T, 3>::unitY = Vector<T, 3>(0, 1, 0);

    template <typename T>
    const Vector<T, 3> VecConsts<T, 3>::unitZ = Vector<T, 3>(0, 0, 1);

    template <typename T>
    const Vector<T, 3> VecConsts<T, 3>::unit = Vector<T, 3>(1, 1, 1);

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::zero = Vector<T, 4>();

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::unitX = Vector<T, 4>(1, 0, 0, 0);

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::unitY = Vector<T, 4>(0, 1, 0, 0);

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::unitZ = Vector<T, 4>(0, 0, 1, 0);

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::unitW = Vector<T, 4>(0, 0, 0, 1);

    template <typename T>
    const Vector<T, 4> VecConsts<T, 4>::unit = Vector<T, 4>(1, 1, 1, 1);

    template<typename T, uint8_t L>
    Vector<T, L>::Vector() : BaseVector<T, L>(0)
    {
    }

    template<typename T, uint8_t L>
    Vector<T, L>::Vector(T inValue) : BaseVector<T, L>(inValue)
    {
    }

    template <typename T, uint8_t L>
    Vector<T, L>::Vector(const Vector& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
    }

    template <typename T, uint8_t L>
    Vector<T, L>::Vector(Vector&& other) noexcept
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = std::move(other.data[i]);
        }
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator=(const Vector& other)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] = other.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    template <typename... IT>
    Vector<T, L>::Vector(IT&&... inValues) : BaseVector<T, L>(std::forward<IT>(inValues)...)
    {
    }

    template <typename T, uint8_t L>
    T& Vector<T, L>::operator[](uint32_t i)
    {
        return this->data[i];
    }

    template <typename T, uint8_t L>
    T Vector<T, L>::operator[](uint32_t i) const
    {
        return this->data[i];
    }

    template <typename T, uint8_t L>
    bool Vector<T, L>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t L>
    bool Vector<T, L>::operator==(const Vector& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L>
    bool Vector<T, L>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L>
    bool Vector<T, L>::operator!=(const Vector& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator+(T rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator-(T rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator*(T rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator/(T rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] / rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator+(const Vector& rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator-(const Vector& rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator*(const Vector& rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator/(const Vector& rhs) const
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] / rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator+=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] += rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator-=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] -= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator*=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] *= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator/=(T rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] /= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator+=(const Vector& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] += rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator-=(const Vector& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] -= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator*=(const Vector& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] *= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    Vector<T, L>& Vector<T, L>::operator/=(const Vector& rhs)
    {
        for (auto i = 0; i < L; i++) {
            this->data[i] /= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t L>
    template <typename IT>
    Vector<IT, L> Vector<T, L>::CastTo() const
    {
        Vector<IT, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t L>
    template <uint8_t... I>
    Vector<T, sizeof...(I)> Vector<T, L>::SubVec() const
    {
        Vector<T, sizeof...(I)> result;
        Internal::CopyValueToSubVec<Vector<T, L>, Vector<T, sizeof...(I)>, I...>(*this, result, std::make_index_sequence<sizeof...(I)> {});
        return result;
    }

    template <typename T, uint8_t L>
    T Vector<T, L>::Model() const
    {
        static_assert(isFloatingPointV<T>);
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += this->data[i] * this->data[i];
        }
        return std::sqrt(temp);
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::Normalized() const
    {
        return this->operator/(Model());
    }

    template <typename T, uint8_t L>
    T Vector<T, L>::Dot(const Vector& rhs) const
    {
        static_assert(isFloatingPointV<T>);
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += this->data[i] * rhs.data[i];
        }
        return temp;
    }

    template <typename T, uint8_t L>
    typename Internal::VecCrossResultTraits<T, L>::Type Vector<T, L>::Cross(const Vector& rhs) const
    {
        static_assert(isFloatingPointV<T> && L >= 2 && L <= 3);
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
