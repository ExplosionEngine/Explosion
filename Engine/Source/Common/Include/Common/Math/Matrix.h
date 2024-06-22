//
// Created by johnk on 2023/5/15.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>
#include <Common/Debug.h>
#include <Common/Utility.h>

namespace Common {
    enum class MatrixSetupType : uint8_t {
        rows,
        cols,
        max
    };

    // matrix stored in row-major
    template <typename T, uint8_t R, uint8_t C>
    requires (R >= 1) && (R <= 4) && (C >= 1) && (C <= 4)
    struct BaseMatrix {
        T data[R * C];
    };

    template <typename T, uint8_t R, uint8_t C>
    struct Matrix : BaseMatrix<T, R, C> {
        using Type = T;
        static constexpr uint8_t rows = R;
        static constexpr uint8_t cols = C;

        template <typename... IT>
        requires (IsAllSame<Vector<T, C>, IT...>::value) && (sizeof...(IT) == R)
        static Matrix FromRowVecs(IT&&... inVectors);

        template <typename... IT>
        requires (IsAllSame<Vector<T, R>, IT...>::value) && (sizeof...(IT) == C)
        static Matrix FromColVecs(IT&&... inVectors);

        Matrix();
        Matrix(T inValue); // NOLINT
        Matrix(const Matrix& other);
        Matrix(Matrix&& other) noexcept;
        Matrix& operator=(const Matrix& other);

        template <typename... IT>
        requires (sizeof...(IT) > 1)
        Matrix(IT&&... inValues); // NOLINT

        T& At(uint8_t row, uint8_t col);
        const T& At(uint8_t row, uint8_t col) const;

        T& operator[](uint32_t index);
        const T& operator[](uint32_t index) const;

        bool operator==(T rhs) const;
        bool operator==(const Matrix& rhs) const;
        bool operator!=(T rhs) const;
        bool operator!=(const Matrix& rhs) const;

        Matrix operator+(T rhs) const;
        Matrix operator-(T rhs) const;
        Matrix operator*(T rhs) const;
        Matrix operator/(T rhs) const;

        Matrix operator+(const Matrix& rhs) const;
        Matrix operator-(const Matrix& rhs) const;

        Matrix& operator+=(T rhs);
        Matrix& operator-=(T rhs);
        Matrix& operator*=(T rhs);
        Matrix& operator/=(T rhs);

        Matrix& operator+=(const Matrix& rhs);
        Matrix& operator-=(const Matrix& rhs);

        template <uint8_t IC>
        Matrix<T, R, IC> operator*(const Matrix<T, C, IC>& rhs) const;

        Vector<T, C> Row(uint8_t index) const;
        Vector<T, R> Col(uint8_t index) const;

        template <typename... IT>
        void SetValues(IT&&... inValues);

        template <typename... IT>
        void SetRows(IT&&... inVectors);

        template <typename... IT>
        void SetCols(IT&&... inVectors);

        template <typename... IT>
        void SetRow(uint8_t index, IT&&... inValues);

        template <typename... IT>
        void SetCol(uint8_t index, IT&&... inValues);

        template <typename IT>
        Matrix<IT, R, C> CastTo() const;

        Matrix<T, C, R> Transpose() const;
        bool CanInverse() const;
        Matrix Inverse() const;
        T Determinant() const;
    };

    template <typename T, uint8_t R, uint8_t C>
    requires (R >= 1) && (R <= 4) && (C >= 1) && (C <= 4)
    struct MatConsts {
        static const Matrix<T, R, C> zero;
    };

    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    struct MatConsts<T, L, L> {
        static const Matrix<T, L, L> zero;
        static const Matrix<T, L, L> identity;
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

    using IMat1x1 = Matrix<int32_t, 1, 1>;
    using IMat1x2 = Matrix<int32_t, 1, 2>;
    using IMat1x3 = Matrix<int32_t, 1, 3>;
    using IMat1x4 = Matrix<int32_t, 1, 4>;
    using IMat2x1 = Matrix<int32_t, 2, 1>;
    using IMat2x2 = Matrix<int32_t, 2, 2>;
    using IMat2x3 = Matrix<int32_t, 2, 3>;
    using IMat2x4 = Matrix<int32_t, 2, 4>;
    using IMat3x1 = Matrix<int32_t, 3, 1>;
    using IMat3x2 = Matrix<int32_t, 3, 2>;
    using IMat3x3 = Matrix<int32_t, 3, 3>;
    using IMat3x4 = Matrix<int32_t, 3, 4>;
    using IMat4x1 = Matrix<int32_t, 4, 1>;
    using IMat4x2 = Matrix<int32_t, 4, 2>;
    using IMat4x3 = Matrix<int32_t, 4, 3>;
    using IMat4x4 = Matrix<int32_t, 4, 4>;

    using HMat1x1 = Matrix<HFloat, 1, 1>;
    using HMat1x2 = Matrix<HFloat, 1, 2>;
    using HMat1x3 = Matrix<HFloat, 1, 3>;
    using HMat1x4 = Matrix<HFloat, 1, 4>;
    using HMat2x1 = Matrix<HFloat, 2, 1>;
    using HMat2x2 = Matrix<HFloat, 2, 2>;
    using HMat2x3 = Matrix<HFloat, 2, 3>;
    using HMat2x4 = Matrix<HFloat, 2, 4>;
    using HMat3x1 = Matrix<HFloat, 3, 1>;
    using HMat3x2 = Matrix<HFloat, 3, 2>;
    using HMat3x3 = Matrix<HFloat, 3, 3>;
    using HMat3x4 = Matrix<HFloat, 3, 4>;
    using HMat4x1 = Matrix<HFloat, 4, 1>;
    using HMat4x2 = Matrix<HFloat, 4, 2>;
    using HMat4x3 = Matrix<HFloat, 4, 3>;
    using HMat4x4 = Matrix<HFloat, 4, 4>;

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

    using BMat1x1Consts = MatConsts<bool, 1, 1>;
    using BMat1x2Consts = MatConsts<bool, 1, 2>;
    using BMat1x3Consts = MatConsts<bool, 1, 3>;
    using BMat1x4Consts = MatConsts<bool, 1, 4>;
    using BMat2x1Consts = MatConsts<bool, 2, 1>;
    using BMat2x2Consts = MatConsts<bool, 2, 2>;
    using BMat2x3Consts = MatConsts<bool, 2, 3>;
    using BMat2x4Consts = MatConsts<bool, 2, 4>;
    using BMat3x1Consts = MatConsts<bool, 3, 1>;
    using BMat3x2Consts = MatConsts<bool, 3, 2>;
    using BMat3x3Consts = MatConsts<bool, 3, 3>;
    using BMat3x4Consts = MatConsts<bool, 3, 4>;
    using BMat4x1Consts = MatConsts<bool, 4, 1>;
    using BMat4x2Consts = MatConsts<bool, 4, 2>;
    using BMat4x3Consts = MatConsts<bool, 4, 3>;
    using BMat4x4Consts = MatConsts<bool, 4, 4>;

    using IMat1x1Consts = MatConsts<int32_t, 1, 1>;
    using IMat1x2Consts = MatConsts<int32_t, 1, 2>;
    using IMat1x3Consts = MatConsts<int32_t, 1, 3>;
    using IMat1x4Consts = MatConsts<int32_t, 1, 4>;
    using IMat2x1Consts = MatConsts<int32_t, 2, 1>;
    using IMat2x2Consts = MatConsts<int32_t, 2, 2>;
    using IMat2x3Consts = MatConsts<int32_t, 2, 3>;
    using IMat2x4Consts = MatConsts<int32_t, 2, 4>;
    using IMat3x1Consts = MatConsts<int32_t, 3, 1>;
    using IMat3x2Consts = MatConsts<int32_t, 3, 2>;
    using IMat3x3Consts = MatConsts<int32_t, 3, 3>;
    using IMat3x4Consts = MatConsts<int32_t, 3, 4>;
    using IMat4x1Consts = MatConsts<int32_t, 4, 1>;
    using IMat4x2Consts = MatConsts<int32_t, 4, 2>;
    using IMat4x3Consts = MatConsts<int32_t, 4, 3>;
    using IMat4x4Consts = MatConsts<int32_t, 4, 4>;

    using HMat1x1Consts = MatConsts<HFloat, 1, 1>;
    using HMat1x2Consts = MatConsts<HFloat, 1, 2>;
    using HMat1x3Consts = MatConsts<HFloat, 1, 3>;
    using HMat1x4Consts = MatConsts<HFloat, 1, 4>;
    using HMat2x1Consts = MatConsts<HFloat, 2, 1>;
    using HMat2x2Consts = MatConsts<HFloat, 2, 2>;
    using HMat2x3Consts = MatConsts<HFloat, 2, 3>;
    using HMat2x4Consts = MatConsts<HFloat, 2, 4>;
    using HMat3x1Consts = MatConsts<HFloat, 3, 1>;
    using HMat3x2Consts = MatConsts<HFloat, 3, 2>;
    using HMat3x3Consts = MatConsts<HFloat, 3, 3>;
    using HMat3x4Consts = MatConsts<HFloat, 3, 4>;
    using HMat4x1Consts = MatConsts<HFloat, 4, 1>;
    using HMat4x2Consts = MatConsts<HFloat, 4, 2>;
    using HMat4x3Consts = MatConsts<HFloat, 4, 3>;
    using HMat4x4Consts = MatConsts<HFloat, 4, 4>;

    using FMat1x1Consts = MatConsts<float, 1, 1>;
    using FMat1x2Consts = MatConsts<float, 1, 2>;
    using FMat1x3Consts = MatConsts<float, 1, 3>;
    using FMat1x4Consts = MatConsts<float, 1, 4>;
    using FMat2x1Consts = MatConsts<float, 2, 1>;
    using FMat2x2Consts = MatConsts<float, 2, 2>;
    using FMat2x3Consts = MatConsts<float, 2, 3>;
    using FMat2x4Consts = MatConsts<float, 2, 4>;
    using FMat3x1Consts = MatConsts<float, 3, 1>;
    using FMat3x2Consts = MatConsts<float, 3, 2>;
    using FMat3x3Consts = MatConsts<float, 3, 3>;
    using FMat3x4Consts = MatConsts<float, 3, 4>;
    using FMat4x1Consts = MatConsts<float, 4, 1>;
    using FMat4x2Consts = MatConsts<float, 4, 2>;
    using FMat4x3Consts = MatConsts<float, 4, 3>;
    using FMat4x4Consts = MatConsts<float, 4, 4>;

    using DMat1x1Consts = MatConsts<double, 1, 1>;
    using DMat1x2Consts = MatConsts<double, 1, 2>;
    using DMat1x3Consts = MatConsts<double, 1, 3>;
    using DMat1x4Consts = MatConsts<double, 1, 4>;
    using DMat2x1Consts = MatConsts<double, 2, 1>;
    using DMat2x2Consts = MatConsts<double, 2, 2>;
    using DMat2x3Consts = MatConsts<double, 2, 3>;
    using DMat2x4Consts = MatConsts<double, 2, 4>;
    using DMat3x1Consts = MatConsts<double, 3, 1>;
    using DMat3x2Consts = MatConsts<double, 3, 2>;
    using DMat3x3Consts = MatConsts<double, 3, 3>;
    using DMat3x4Consts = MatConsts<double, 3, 4>;
    using DMat4x1Consts = MatConsts<double, 4, 1>;
    using DMat4x2Consts = MatConsts<double, 4, 2>;
    using DMat4x3Consts = MatConsts<double, 4, 3>;
    using DMat4x4Consts = MatConsts<double, 4, 4>;
}

namespace Common { // NOLINT
    template <typename T, uint8_t R, uint8_t C>
    struct Serializer<Matrix<T, R, C>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("Common::Matrix")
            + Serializer<T>::typeId + (R << 8) + C;

        static void Serialize(SerializeStream& stream, const Matrix<T, R, C>& value)
        {
            TypeIdSerializer<Matrix<T, R, C>>::Serialize(stream);

            for (auto i = 0; i < R * C; i++) {
                Serializer<T>::Serialize(stream, value.data[i]);
            }
        }

        static bool Deserialize(DeserializeStream& stream, Matrix<T, R, C>& value)
        {
            if (!TypeIdSerializer<Matrix<T, R, C>>::Deserialize(stream)) {
                return false;
            }

            for (auto i = 0; i < R * C; i++) {
                Serializer<T>::Deserialize(stream, value.data[i]);
            }
            return true;
        }
    };
}

namespace Common::Internal {
    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrix(Matrix<T, R, C>& matrix, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R * C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[VI] = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixRow(Matrix<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixCol(Matrix<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, size_t... VI>
    static void CopyVectorToMatrixRow(Matrix<T, R, C>& matrix, uint8_t index, const Vector<T, C>& inVector, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, size_t... VI>
    static void CopyVectorToMatrixCol(Matrix<T, R, C>& matrix, uint8_t index, const Vector<T, R>& inVector, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyRowVectorsToMatrix(Matrix<T, R, C>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vector<T, C>>);
            CopyVectorToMatrixRow(matrix, VI, inVectors, std::make_index_sequence<C> {});
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyColVectorsToMatrix(Matrix<T, R, C>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vector<T, R>>);
            CopyVectorToMatrixCol(matrix, VI, inVectors, std::make_index_sequence<R> {});
        }(), 0)... };
    }

    template <typename T, uint8_t L, size_t... VI>
    static void SetMatrixToIdentity(Matrix<T, L, L>& matrix, std::index_sequence<VI...>)
    {
        static_assert(L == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, VI) = 1;
        }(), 0)... };
    }

    template <typename T, uint8_t L>
    static Matrix<T, L, L> GetIdentityMatrix()
    {
        Matrix<T, L, L> result;
        SetMatrixToIdentity(result, std::make_index_sequence<L> {});
        return result;
    }
}

namespace Common {
    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    requires (IsAllSame<Vector<T, C>, IT...>::value) && (sizeof...(IT) == R)
    Matrix<T, R, C> Matrix<T, R, C>::FromRowVecs(IT&&... inVectors)
    {
        Matrix<T, R, C> result;
        result.SetRows(std::forward<IT>(inVectors)...);
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    requires (IsAllSame<Vector<T, R>, IT...>::value) && (sizeof...(IT) == C)
    Matrix<T, R, C> Matrix<T, R, C>::FromColVecs(IT&&... inVectors)
    {
        Matrix<T, R, C> result;
        result.SetCols(std::forward<IT>(inVectors)...);
        return result;
    }

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
    requires (sizeof...(IT) > 1)
    Matrix<T, R, C>::Matrix(IT&&... inValues)
    {
        static_assert(sizeof...(IT) == R || sizeof...(IT) == R * C);
        if constexpr (sizeof...(IT) == R * C) {
            SetValues(std::forward<IT>(inValues)...);
        } else {
            SetRows(std::forward<IT>(inValues)...);
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    T& Matrix<T, R, C>::At(uint8_t row, uint8_t col)
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C>
    const T& Matrix<T, R, C>::At(uint8_t row, uint8_t col) const
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C>
    T& Matrix<T, R, C>::operator[](uint32_t index)
    {
        Assert(index < R * C);
        return this->data[index];
    }

    template <typename T, uint8_t R, uint8_t C>
    const T& Matrix<T, R, C>::operator[](uint32_t index) const
    {
        Assert(index < R * C);
        return this->data[index];
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
    Matrix<T, R, C>& Matrix<T, R, C>::operator+=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator*=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] *= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator/=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] /= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+=(const Matrix& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-=(const Matrix& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <uint8_t IC>
    Matrix<T, R, IC> Matrix<T, R, C>::operator*(const Matrix<T, C, IC>& rhs) const
    {
        Matrix<T, R, IC> result;
        for (auto i = 0; i < R; i++) {
            for (auto j = 0; j < IC; j++) {
                result.At(i, j) = this->Row(i).Dot(rhs.Col(j));
            }
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Vector<T, C> Matrix<T, R, C>::Row(uint8_t index) const
    {
        Assert(index < R);
        Vector<T, C> result;
        for (auto i = 0; i < C; i++) {
            result[i] = At(index, i);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Vector<T, R> Matrix<T, R, C>::Col(uint8_t index) const
    {
        Assert(index < C);
        Vector<T, R> result;
        for (auto i = 0; i < R; i++) {
            result[i] = At(i, index);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetValues(IT&&... inValues)
    {
        Internal::CopyValuesToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetRows(IT&&... inVectors)
    {
        Internal::CopyRowVectorsToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetCols(IT&&... inVectors)
    {
        Internal::CopyColVectorsToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetRow(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vector<T, C>, IT...>::value) {
            Internal::CopyVectorToMatrixRow<T, R, C>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<C> {});
        } else {
            Internal::CopyValuesToMatrixRow<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Matrix<T, R, C>::SetCol(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vector<T, R>, IT...>::value) {
            Internal::CopyVectorToMatrixCol<T, R, C>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<R> {});
        } else {
            Internal::CopyValuesToMatrixCol<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
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
                result.At(j, i) = At(i, j);
            }
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Matrix<T, R, C>::CanInverse() const
    {
        return this->Determinant() != static_cast<T>(0);
    }

    template <typename T, uint8_t R, uint8_t C>
    Matrix<T, R, C> Matrix<T, R, C>::Inverse() const
    {
        static_assert( R == C && R > 1 && R < 5);
        T oneOverDet = static_cast<T>(1) / this->Determinant();

        Matrix<T, R, C> result;
        if constexpr (R == 2) {
            result.At(0, 0) = this->data[3] * oneOverDet;
            result.At(0, 1) = -this->data[1] * oneOverDet;
            result.At(1, 0) = -this->data[2] * oneOverDet;
            result.At(1, 1) = this->data[0] * oneOverDet;
        }

        if constexpr (R == 3) {
            result.At(0, 0) = (this->data[4] * this->data[8] - this->data[5] * this->data[7]) * oneOverDet;
            result.At(0, 1) = (this->data[2] * this->data[7] - this->data[1] * this->data[8]) * oneOverDet;
            result.At(0, 2) = (this->data[1] * this->data[5] - this->data[2] * this->data[4]) * oneOverDet;
            result.At(1, 0) = (this->data[5] * this->data[6] - this->data[3] * this->data[8]) * oneOverDet;
            result.At(1, 1) = (this->data[0] * this->data[8] - this->data[2] * this->data[6]) * oneOverDet;
            result.At(1, 2) = (this->data[2] * this->data[3] - this->data[0] * this->data[5]) * oneOverDet;
            result.At(2, 0) = (this->data[3] * this->data[7] - this->data[4] * this->data[6]) * oneOverDet;
            result.At(2, 1) = (this->data[1] * this->data[6] - this->data[7] * this->data[0]) * oneOverDet;
            result.At(2, 2) = (this->data[0] * this->data[4] - this->data[1] * this->data[3]) * oneOverDet;
        }

        if constexpr (R == 4) {
            T coef00 = this->data[10] * this->data[15] - this->data[11] * this->data[14];
            T coef02 = this->data[9] * this->data[15] - this->data[11] * this->data[13];
            T coef03 = this->data[9] * this->data[14] - this->data[10] * this->data[13];

            T coef04 = this->data[6] * this->data[15] - this->data[7] * this->data[14];
            T coef06 = this->data[5] * this->data[15] - this->data[7] * this->data[13];
            T coef07 = this->data[5] * this->data[14] - this->data[6] * this->data[13];

            T coef08 = this->data[6] * this->data[11] - this->data[7] * this->data[10];
            T coef10 = this->data[5] * this->data[11] - this->data[7] * this->data[9];
            T coef11 = this->data[5] * this->data[10] - this->data[6] * this->data[9];

            T coef12 = this->data[2] * this->data[15] - this->data[3] * this->data[14];
            T coef14 = this->data[1] * this->data[15] - this->data[3] * this->data[13];
            T coef15 = this->data[1] * this->data[14] - this->data[2] * this->data[13];

            T coef16 = this->data[2] * this->data[11] - this->data[3] * this->data[10];
            T coef18 = this->data[1] * this->data[11] - this->data[3] * this->data[9];
            T coef19 = this->data[1] * this->data[10] - this->data[2] * this->data[9];

            T coef20 = this->data[2] * this->data[7] - this->data[3] * this->data[6];
            T coef22 = this->data[1] * this->data[7] - this->data[3] * this->data[5];
            T coef23 = this->data[1] * this->data[6] - this->data[2] * this->data[5];

            Vector<T, 4> fac0(coef00, coef00, coef02, coef03);
            Vector<T, 4> fac1(coef04, coef04, coef06, coef07);
            Vector<T, 4> fac2(coef08, coef08, coef10, coef11);
            Vector<T, 4> fac3(coef12, coef12, coef14, coef15);
            Vector<T, 4> fac4(coef16, coef16, coef18, coef19);
            Vector<T, 4> fac5(coef20, coef20, coef22, coef23);

            Vector<T, 4> vec0(this->data[1], this->data[0], this->data[0], this->data[0]);
            Vector<T, 4> vec1(this->data[5], this->data[4], this->data[4], this->data[4]);
            Vector<T, 4> vec2(this->data[9], this->data[8], this->data[8], this->data[8]);
            Vector<T, 4> vec3(this->data[13], this->data[12], this->data[12], this->data[12]);

            Vector<T, 4> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
            Vector<T, 4> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
            Vector<T, 4> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
            Vector<T, 4> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

            Vector<T, 4> signA(+1, -1, +1, -1);
            Vector<T, 4> signB(-1, +1, -1, +1);

            Vector<T, 4> col0 = inv0 * signA;
            Vector<T, 4> col1 = inv1 * signB;
            Vector<T, 4> col2 = inv2 * signA;
            Vector<T, 4> col3 = inv3 * signB;

            result.SetCol(0, col0);
            result.SetCol(1, col1);
            result.SetCol(2, col2);
            result.SetCol(3, col3);

            result = result * oneOverDet;
        }

        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    T Matrix<T, R, C>::Determinant() const
    {
        static_assert( R == C && R > 1 && R < 5);
        T result = static_cast<T>(0);
        if constexpr (R == 2) {
            result = this->data[0] * this->data[3] - this->data[1] * this->data[2];
        }

        if constexpr (R == 3) {
            result =
                this->data[0] * (this->data[4] * this->data[8] - this->data[5] * this->data[7])
                - this->data[1] * (this->data[3] * this->data[8] - this->data[5] * this->data[6])
                + this->data[2] * (this->data[3] * this->data[7] - this->data[4] * this->data[6]);
        }

        if constexpr (R == 4) {
            T subFactor0 = this->data[10] * this->data[15] - this->data[11] * this->data[14];
            T subFactor1 = this->data[6] * this->data[15] - this->data[7] * this->data[14];
            T subFactor2 = this->data[6] * this->data[11] - this->data[7] * this->data[10];
            T subFactor3 = this->data[2] * this->data[15] - this->data[3] * this->data[14];
            T subFactor4 = this->data[2] * this->data[11] - this->data[3] * this->data[10];
            T subFactor5 = this->data[2] * this->data[7] - this->data[3] * this->data[6];

            T detCoef0 = this->data[5] * subFactor0 - this->data[9] * subFactor1 + this->data[13] * subFactor2;
            T detCoef1 = this->data[9] * subFactor3 - this->data[1] * subFactor0 - this->data[13] * subFactor4;
            T detCoef2 = this->data[1] * subFactor1 - this->data[5] * subFactor3 + this->data[13] * subFactor5;
            T detCoef3 = this->data[5] * subFactor4 - this->data[1] * subFactor2 - this->data[9] * subFactor5;

            result = this->data[0] * detCoef0 + this->data[4] * detCoef1 + this->data[8] * detCoef2 + this->data[12] * detCoef3;
        }

        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    requires (R >= 1) && (R <= 4) && (C >= 1) && (C <= 4)
    const Matrix<T, R, C> MatConsts<T, R, C>::zero = Matrix<T, R, C>(0);

    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    const Matrix<T, L, L> MatConsts<T, L, L>::zero = Matrix<T, L, L>(0);

    template <typename T, uint8_t L>
    requires (L >= 1) && (L <= 4)
    const Matrix<T, L, L> MatConsts<T, L, L>::identity = Internal::GetIdentityMatrix<T, L>();

    template <typename T, uint8_t R, uint8_t C>
    Vector<T, R> operator*(const Matrix<T, R, C>& mat, const Vector<T, C>& vec) {
        Vector<T, R> result;
        for (auto i = 0; i < R; i++) {
            result[i] = mat.Row(i).Dot(vec);
        }
        return result;
    }
}
