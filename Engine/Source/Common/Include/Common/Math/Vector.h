//
// Created by johnk on 2023/5/9.
//

#pragma once

#include <cstdint>
#include <utility>

#include <Common/Math/Half.h>

namespace Common {
    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    struct BaseVector {};

    template <typename T>
    struct BaseVector<T, 1> {
        inline BaseVector();
        inline explicit BaseVector(T inX);

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
        inline explicit BaseVector(T inValue);
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
        inline explicit BaseVector(T inValue);
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
        inline explicit BaseVector(T inValue);
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

    template <typename T, uint8_t L>
    struct Vector : public BaseVector<T, L> {
        inline Vector();
        inline explicit Vector(T inValue);

        template <typename... IT>
        inline explicit Vector(IT&&... inValues);

        inline T& operator[](uint32_t i);
        inline T operator[](uint32_t i) const;

        inline bool operator==(T rhs) const;
        inline bool operator==(const Vector& rhs) const;

        inline Vector operator+(T rhs);
        inline Vector operator-(T rhs);
        inline Vector operator*(T rhs);
        inline Vector operator/(T rhs);

        inline Vector operator+(const Vector& rhs);
        inline Vector operator-(const Vector& rhs);
        inline Vector operator*(const Vector& rhs);
        inline Vector operator/(const Vector& rhs);

        inline Vector& operator+=(T rhs);
        inline Vector& operator-=(T rhs);
        inline Vector& operator*=(T rhs);
        inline Vector& operator/=(T rhs);

        inline Vector& operator+=(const Vector& rhs);
        inline Vector& operator-=(const Vector& rhs);
        inline Vector& operator*=(const Vector& rhs);
        inline Vector& operator/=(const Vector& rhs);

        template <uint8_t... I>
        Vector<T, sizeof...(I)> SubVec();
    };

    using BVec1 = Vector<bool, 1>;
    using BVec2 = Vector<bool, 2>;
    using BVec3 = Vector<bool, 3>;
    using BVec4 = Vector<bool, 4>;

    using IVec1 = Vector<int32_t, 1>;
    using IVec2 = Vector<int32_t, 2>;
    using IVec3 = Vector<int32_t, 3>;
    using IVec4 = Vector<int32_t, 4>;

    using HVec1 = Vector<HalfFloat, 1>;
    using HVec2 = Vector<HalfFloat, 2>;
    using HVec3 = Vector<HalfFloat, 3>;
    using HVec4 = Vector<HalfFloat, 4>;

    using FVec1 = Vector<float, 1>;
    using FVec2 = Vector<float, 2>;
    using FVec3 = Vector<float, 3>;
    using FVec4 = Vector<float, 4>;

    using DVec1 = Vector<double, 1>;
    using DVec2 = Vector<double, 2>;
    using DVec3 = Vector<double, 3>;
    using DVec4 = Vector<double, 4>;

    template <typename T, uint8_t L>
    requires isFloatingPointV<T>
    T Length(const Vector<T, L>& vec);

    template <typename T, uint8_t L>
    requires isFloatingPointV<T>
    T Dot(const Vector<T, L>& lhs, const Vector<T, L>& rhs);

    template <typename T>
    requires isFloatingPointV<T>
    T Cross(const Vector<T, 2>& lhs, const Vector<T, 2>& rhs);

    template <typename T>
    requires isFloatingPointV<T>
    Vector<T, 3> Cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs);
}

namespace Common::Internal {
    template <typename VecT, typename SubVecT, uint8_t... VecIndex, size_t... SubVecIndex>
    void CopyValueToSubVec(const VecT& vec, SubVecT& subVec, std::index_sequence<SubVecIndex...>)
    {
        static_assert(sizeof...(SubVecIndex) == sizeof...(VecIndex));
        std::initializer_list<int> { ([&]() -> void { subVec.data[SubVecIndex] == vec.data[VecIndex]; }(), 0)... };
    }
}

namespace Common {
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

    template<typename T, uint8_t L>
    Vector<T, L>::Vector() : BaseVector<T, L>(0)
    {
    }

    template<typename T, uint8_t L>
    Vector<T, L>::Vector(T inValue) : BaseVector<T, L>(inValue)
    {
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
            if constexpr (std::is_floating_point_v<T>) {
                result = result && (std::abs(this->data[i] - rhs) < epsilon);
            } else {
                result = result && (this->data[i] == rhs);
            }
        }
        return result;
    }

    template <typename T, uint8_t L>
    bool Vector<T, L>::operator==(const Vector& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < L; i++) {
            if constexpr (std::is_floating_point_v<T>) {
                result = result && (std::abs(this->data[i] - rhs.data[i]) < epsilon);
            } else {
                result = result && (this->data[i] == rhs.data[i]);
            }
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator+(T rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator-(T rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator*(T rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator/(T rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] / rhs;
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator+(const Vector& rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator-(const Vector& rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator*(const Vector& rhs)
    {
        Vector<T, L> result;
        for (auto i = 0; i < L; i++) {
            result.data[i] = this->data[i] * rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t L>
    Vector<T, L> Vector<T, L>::operator/(const Vector& rhs)
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
    template <uint8_t... I>
    Vector<T, sizeof...(I)> Vector<T, L>::SubVec()
    {
        Vector<T, sizeof...(I)> result;
        Internal::CopyValueToSubVec<T, Vector<T, sizeof...(I)>, I...>(*this, result, std::make_index_sequence<sizeof...(I)> {});
        return result;
    }

    template <typename T, uint8_t L>
    requires isFloatingPointV<T>
    T Length(const Vector<T, L>& vec)
    {
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += vec.data[i] * vec.data[i];
        }
        return std::sqrt(temp);
    }

    template <typename T, uint8_t L>
    requires isFloatingPointV<T>
    T Dot(const Vector<T, L>& lhs, const Vector<T, L>& rhs)
    {
        T temp = 0;
        for (auto i = 0; i < L; i++) {
            temp += lhs.data[i] * rhs.data[i];
        }
        return temp;
    }

    template <typename T>
    requires isFloatingPointV<T>
    T Cross(const Vector<T, 2>& lhs, const Vector<T, 2>& rhs)
    {
        return lhs.x * rhs.y - lhs.y * rhs.x;
    }

    template <typename T>
    requires isFloatingPointV<T>
    Vector<T, 3> Cross(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
    {
        Vector<T, 3> result;
        result.x = lhs.y * rhs.z - lhs.z * rhs.y;
        result.y = lhs.z * rhs.x - lhs.x * rhs.z;
        result.z = lhs.x * rhs.y - lhs.y * rhs.x;
        return result;
    }
}
