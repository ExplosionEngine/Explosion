//
// Created by johnk on 2023/5/15.
//

#pragma once

#include <Common/Math/Vector.h>

namespace Common {
    // Matrix saved in row-major
    template <typename T, uint8_t R, uint8_t C>
    requires (R >= 1) && (R <= 4) && (C >= 1) && (C <= 4)
    struct BaseMatrix {
        T data[R * C];
    };

    template <typename T, uint8_t R, uint8_t C>
    struct Matrix : public BaseMatrix<T, R, C> {
        using Type = T;
        static constexpr uint8_t rows = R;
        static constexpr uint8_t cols = C;

        inline Matrix();
        inline Matrix(T inValue); // NOLINT
        inline Matrix(const Matrix& other);
        inline Matrix(Matrix&& other) noexcept;
        inline Matrix& operator=(const Matrix& other);

        template <typename... IT>
        inline Matrix(IT&&... inValues); // NOLINT

        T* operator[](uint32_t index);
        inline const T* operator[](uint32_t index) const;

        inline bool operator==(T rhs) const;
        inline bool operator==(const Matrix& rhs) const;
        inline bool operator!=(T rhs) const;
        inline bool operator!=(const Matrix& rhs) const;

        inline Matrix operator+(T rhs) const;
        inline Matrix operator-(T rhs) const;
        inline Matrix operator*(T rhs) const;
        inline Matrix operator/(T rhs) const;

        inline Matrix operator+(const Matrix& rhs) const;
        inline Matrix operator-(const Matrix& rhs) const;

        template <uint8_t IC>
        inline Matrix<T, R, IC> operator*(const Matrix<T, C, IC>& rhs);

        inline Matrix& operator+=(T rhs);
        inline Matrix& operator-=(T rhs);
        inline Matrix& operator*=(T rhs);
        inline Matrix& operator/=(T rhs);

        inline Matrix& operator+=(const Matrix& rhs);
        inline Matrix& operator-=(const Matrix& rhs);

        inline Vector<T, C> Row(uint8_t index) const;
        inline Vector<T, R> Col(uint8_t index) const;
        inline void SetRow(uint8_t index, const Vector<T, C>& inValue);
        inline void SetCol(uint8_t index, const Vector<T, R>& inValue);

        template <typename... IT>
        inline void SetRow(uint8_t index, IT&&... inValues);

        template <typename... IT>
        inline void SetCol(uint8_t index, IT&&... inValues);

        template <typename IT>
        Matrix<IT, R, C> CastTo() const;

        inline Matrix<T, C, R> Transpose() const;
    };

    using BMat1x1 = Matrix<bool, 1, 1>;
    using BMat1x2 = Matrix<bool, 1, 2>;
    using BMat1x3 = Matrix<bool, 1, 3>;
    using BMat1x4 = Matrix<bool, 1, 4>;
    using BMat2x1 = Matrix<bool, 2, 1>;
    using BMat2x2 = Matrix<bool, 2, 2>;
    using BMat2x3 = Matrix<bool, 2, 3>;
    using BMat2x4 = Matrix<bool, 2, 4>;
    using BMat3x1 = Matrix<bool, 3, 1>;
    using BMat3x2 = Matrix<bool, 3, 2>;
    using BMat3x3 = Matrix<bool, 3, 3>;
    using BMat3x4 = Matrix<bool, 3, 4>;
    using BMat4x1 = Matrix<bool, 4, 1>;
    using BMat4x2 = Matrix<bool, 4, 2>;
    using BMat4x3 = Matrix<bool, 4, 3>;
    using BMat4x4 = Matrix<bool, 4, 4>;

    using IMat1x1 = Matrix<uint32_t, 1, 1>;
    using IMat1x2 = Matrix<uint32_t, 1, 2>;
    using IMat1x3 = Matrix<uint32_t, 1, 3>;
    using IMat1x4 = Matrix<uint32_t, 1, 4>;
    using IMat2x1 = Matrix<uint32_t, 2, 1>;
    using IMat2x2 = Matrix<uint32_t, 2, 2>;
    using IMat2x3 = Matrix<uint32_t, 2, 3>;
    using IMat2x4 = Matrix<uint32_t, 2, 4>;
    using IMat3x1 = Matrix<uint32_t, 3, 1>;
    using IMat3x2 = Matrix<uint32_t, 3, 2>;
    using IMat3x3 = Matrix<uint32_t, 3, 3>;
    using IMat3x4 = Matrix<uint32_t, 3, 4>;
    using IMat4x1 = Matrix<uint32_t, 4, 1>;
    using IMat4x2 = Matrix<uint32_t, 4, 2>;
    using IMat4x3 = Matrix<uint32_t, 4, 3>;
    using IMat4x4 = Matrix<uint32_t, 4, 4>;

    using HMat1x1 = Matrix<HalfFloat, 1, 1>;
    using HMat1x2 = Matrix<HalfFloat, 1, 2>;
    using HMat1x3 = Matrix<HalfFloat, 1, 3>;
    using HMat1x4 = Matrix<HalfFloat, 1, 4>;
    using HMat2x1 = Matrix<HalfFloat, 2, 1>;
    using HMat2x2 = Matrix<HalfFloat, 2, 2>;
    using HMat2x3 = Matrix<HalfFloat, 2, 3>;
    using HMat2x4 = Matrix<HalfFloat, 2, 4>;
    using HMat3x1 = Matrix<HalfFloat, 3, 1>;
    using HMat3x2 = Matrix<HalfFloat, 3, 2>;
    using HMat3x3 = Matrix<HalfFloat, 3, 3>;
    using HMat3x4 = Matrix<HalfFloat, 3, 4>;
    using HMat4x1 = Matrix<HalfFloat, 4, 1>;
    using HMat4x2 = Matrix<HalfFloat, 4, 2>;
    using HMat4x3 = Matrix<HalfFloat, 4, 3>;
    using HMat4x4 = Matrix<HalfFloat, 4, 4>;

    using FMat1x1 = Matrix<float, 1, 1>;
    using FMat1x2 = Matrix<float, 1, 2>;
    using FMat1x3 = Matrix<float, 1, 3>;
    using FMat1x4 = Matrix<float, 1, 4>;
    using FMat2x1 = Matrix<float, 2, 1>;
    using FMat2x2 = Matrix<float, 2, 2>;
    using FMat2x3 = Matrix<float, 2, 3>;
    using FMat2x4 = Matrix<float, 2, 4>;
    using FMat3x1 = Matrix<float, 3, 1>;
    using FMat3x2 = Matrix<float, 3, 2>;
    using FMat3x3 = Matrix<float, 3, 3>;
    using FMat3x4 = Matrix<float, 3, 4>;
    using FMat4x1 = Matrix<float, 4, 1>;
    using FMat4x2 = Matrix<float, 4, 2>;
    using FMat4x3 = Matrix<float, 4, 3>;
    using FMat4x4 = Matrix<float, 4, 4>;

    using DMat1x1 = Matrix<double, 1, 1>;
    using DMat1x2 = Matrix<double, 1, 2>;
    using DMat1x3 = Matrix<double, 1, 3>;
    using DMat1x4 = Matrix<double, 1, 4>;
    using DMat2x1 = Matrix<double, 2, 1>;
    using DMat2x2 = Matrix<double, 2, 2>;
    using DMat2x3 = Matrix<double, 2, 3>;
    using DMat2x4 = Matrix<double, 2, 4>;
    using DMat3x1 = Matrix<double, 3, 1>;
    using DMat3x2 = Matrix<double, 3, 2>;
    using DMat3x3 = Matrix<double, 3, 3>;
    using DMat3x4 = Matrix<double, 3, 4>;
    using DMat4x1 = Matrix<double, 4, 1>;
    using DMat4x2 = Matrix<double, 4, 2>;
    using DMat4x3 = Matrix<double, 4, 3>;
    using DMat4x4 = Matrix<double, 4, 4>;
}

namespace Common::Internal {
    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrix(Matrix<T, R, C>& matrix, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R * C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[VI] = inValue;
        }, 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixRow(Matrix<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[index * C + VI] = inValue;
        }, 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixCol(Matrix<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[VI * C + index] = inValue;
        }, 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyRowVectorsToMatrix(Matrix<T, R, C>& matrix, uint8_t index, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vector<T, C>>);
            for (auto i = 0; i < C; i++) {
                matrix.data[VI * C + i] = inVectors[i];
            }
        }, 0)... };
    }
}

namespace Common {
    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>::Matrix()
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = 0;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>::Matrix(T inValue)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = inValue;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>::Matrix(const Matrix& other)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = other.data[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>::Matrix(Matrix&& other) noexcept
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = std::move(other.data[i]);
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator=(const Matrix& other)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = other.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    Matrix<T, R, C>::Matrix(IT&&... inValues)
    {
        static_assert(sizeof...(IT) == R || sizeof...(IT) == R * C);
        if constexpr (sizeof...(IT) == R * C) {
            Internal::CopyValuesToMatrix(*this, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        } else {
            Internal::CopyRowVectorsToMatrix(*this, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    T* Matrix<T, R, C>::operator[](uint32_t index)
    {
        return this->data + index;
    }

    template <typename T, uint8_t R, uint8_t C>
    const T* Matrix<T, R, C>::operator[](uint32_t index) const
    {
        return this->data + index;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Matrix<T, R, C>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Matrix<T, R, C>::operator==(const Matrix& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Matrix<T, R, C>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Matrix<T, R, C>::operator!=(const Matrix& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+(T rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] + rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator-(T rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] - rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator*(T rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] * rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/(T rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] / rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+(const Matrix& rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::operator-(const Matrix& rhs) const
    {
        Matrix<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <uint8_t IC>
    Matrix<T, R, IC> Matrix<T, R, C>::operator*(const Matrix<T, C, IC>& rhs)
    {
        Matrix<T, R, IC> result;
        // TODO
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator*=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] *= rhs;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator/=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] /= rhs;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+=(const Matrix& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs.data[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-=(const Matrix& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs.data[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Vector<T, C> Matrix<T, R, C>::Row(uint8_t index) const
    {
        Vector<T, C> result;
        for (auto i = 0; i < C; i++) {
            result[i] = this->data[index * C + i];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Vector<T, R> Matrix<T, R, C>::Col(uint8_t index) const
    {
        Vector<T, R> result;
        for (auto i = 0; i < R; i++) {
            result[i] = this->data[i * C + index];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    void Matrix<T, R, C>::SetRow(uint8_t index, const Vector<T, C>& inValue)
    {
        for (auto i = 0; i < C; i++) {
            this->data[index * C + i] = inValue[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    void Matrix<T, R, C>::SetCol(uint8_t index, const Vector<T, R>& inValue)
    {
        for (auto i = 0; i < R; i++) {
            this->data[i * C + index] = inValue[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetRow(uint8_t index, IT&&... inValues)
    {
        Internal::CopyValuesToMatrixRow<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetCol(uint8_t index, IT&&... inValues)
    {
        Internal::CopyValuesToMatrixCol<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename IT>
    Matrix<IT, R, C> Matrix<T, R, C>::CastTo() const
    {
        Matrix<IT, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, C, R> Matrix<T, R, C>::Transpose() const
    {
        Matrix<T, C, R> result;
        for (auto i = 0; i < R; i++) {
            for (auto j = 0; j < C; j++) {
                result.data[j * R + i] = this->data[i * C + j];
            }
        }
        return result;
    }
}
