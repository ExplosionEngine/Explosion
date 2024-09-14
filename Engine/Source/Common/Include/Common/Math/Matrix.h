//
// Created by johnk on 2023/5/15.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Common/Serialization.h>
#include <Common/Debug.h>
#include <Common/Utility.h>

namespace Common {
    template <uint8_t R> concept ValidMatRow = R >= 1 && R <= 4;
    template <uint8_t C> concept ValidMatCol = C >= 1 && C <= 4;
    template <uint8_t R, uint8_t C> concept ValidMatDims = ValidMatRow<R> && ValidMatCol<C>;
    template <uint8_t R, uint8_t MR> concept ValidSubMatRow = R >= 1 && R < MR;
    template <uint8_t C, uint8_t MC> concept ValidSubMatCol = C >= 1 && C < MC;
    template <uint8_t R, uint8_t C, uint8_t MR, uint8_t MC> concept ValidSubMatDims = ValidSubMatRow<R, MR> && ValidSubMatCol<C, MC>;

    enum class MatSetupType : uint8_t {
        rows,
        cols,
        max
    };

    // matrix stored in row-major
    template <typename T, uint8_t R, uint8_t C>
    requires ValidMatDims<R, C>
    struct BaseMat {
        T data[R * C];
    };

    template <typename T, uint8_t R, uint8_t C>
    struct Mat : BaseMat<T, R, C> {
        using Type = T;
        static constexpr uint8_t rows = R;
        static constexpr uint8_t cols = C;

        template <VecN<T, C>... IT>
        requires ArgsNumEqual<R, IT...>
        static Mat FromRowVecs(IT&&... inVectors);

        template <VecN<T, R>... IT>
        requires ArgsNumEqual<C, IT...>
        static Mat FromColVecs(IT&&... inVectors);

        Mat();
        Mat(T inValue); // NOLINT
        Mat(const Mat& other);
        Mat(Mat&& other) noexcept;
        Mat& operator=(const Mat& other);

        template <typename... IT>
        requires ArgsNumGreater<1, IT...>
        Mat(IT&&... inValues); // NOLINT

        T& At(uint8_t row, uint8_t col);
        const T& At(uint8_t row, uint8_t col) const;

        T& operator[](uint32_t index);
        const T& operator[](uint32_t index) const;

        bool operator==(T rhs) const;
        bool operator==(const Mat& rhs) const;
        bool operator!=(T rhs) const;
        bool operator!=(const Mat& rhs) const;

        Mat operator+(T rhs) const;
        Mat operator-(T rhs) const;
        Mat operator*(T rhs) const;
        Mat operator/(T rhs) const;

        Mat operator+(const Mat& rhs) const;
        Mat operator-(const Mat& rhs) const;

        Mat& operator+=(T rhs);
        Mat& operator-=(T rhs);
        Mat& operator*=(T rhs);
        Mat& operator/=(T rhs);

        Mat& operator+=(const Mat& rhs);
        Mat& operator-=(const Mat& rhs);

        template <uint8_t IC>
        Mat<T, R, IC> operator*(const Mat<T, C, IC>& rhs) const;

        Vec<T, C> Row(uint8_t index) const;
        Vec<T, R> Col(uint8_t index) const;

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
        Mat<IT, R, C> CastTo() const;

        Mat<T, C, R> Transpose() const;

        template<uint8_t DR, uint8_t DC>
        requires ValidSubMatDims<DR, DC, R, C>
        Mat<T, DR, DC> SubMatrix() const;

        bool CanInverse() const;
        Mat Inverse() const;
        T Determinant() const;
    };

    template <typename T, uint8_t R, uint8_t C>
    requires ValidMatDims<R, C>
    struct MatConsts {
        static const Mat<T, R, C> zero;
    };

    template <typename T, uint8_t L>
    requires ValidVecDim<L>
    struct MatConsts<T, L, L> {
        static const Mat<T, L, L> zero;
        static const Mat<T, L, L> identity;
    };

    using BMat1x1 = Mat<bool, 1, 1>;
    using BMat1x2 = Mat<bool, 1, 2>;
    using BMat1x3 = Mat<bool, 1, 3>;
    using BMat1x4 = Mat<bool, 1, 4>;
    using BMat2x1 = Mat<bool, 2, 1>;
    using BMat2x2 = Mat<bool, 2, 2>;
    using BMat2x3 = Mat<bool, 2, 3>;
    using BMat2x4 = Mat<bool, 2, 4>;
    using BMat3x1 = Mat<bool, 3, 1>;
    using BMat3x2 = Mat<bool, 3, 2>;
    using BMat3x3 = Mat<bool, 3, 3>;
    using BMat3x4 = Mat<bool, 3, 4>;
    using BMat4x1 = Mat<bool, 4, 1>;
    using BMat4x2 = Mat<bool, 4, 2>;
    using BMat4x3 = Mat<bool, 4, 3>;
    using BMat4x4 = Mat<bool, 4, 4>;

    using IMat1x1 = Mat<int32_t, 1, 1>;
    using IMat1x2 = Mat<int32_t, 1, 2>;
    using IMat1x3 = Mat<int32_t, 1, 3>;
    using IMat1x4 = Mat<int32_t, 1, 4>;
    using IMat2x1 = Mat<int32_t, 2, 1>;
    using IMat2x2 = Mat<int32_t, 2, 2>;
    using IMat2x3 = Mat<int32_t, 2, 3>;
    using IMat2x4 = Mat<int32_t, 2, 4>;
    using IMat3x1 = Mat<int32_t, 3, 1>;
    using IMat3x2 = Mat<int32_t, 3, 2>;
    using IMat3x3 = Mat<int32_t, 3, 3>;
    using IMat3x4 = Mat<int32_t, 3, 4>;
    using IMat4x1 = Mat<int32_t, 4, 1>;
    using IMat4x2 = Mat<int32_t, 4, 2>;
    using IMat4x3 = Mat<int32_t, 4, 3>;
    using IMat4x4 = Mat<int32_t, 4, 4>;

    using HMat1x1 = Mat<HFloat, 1, 1>;
    using HMat1x2 = Mat<HFloat, 1, 2>;
    using HMat1x3 = Mat<HFloat, 1, 3>;
    using HMat1x4 = Mat<HFloat, 1, 4>;
    using HMat2x1 = Mat<HFloat, 2, 1>;
    using HMat2x2 = Mat<HFloat, 2, 2>;
    using HMat2x3 = Mat<HFloat, 2, 3>;
    using HMat2x4 = Mat<HFloat, 2, 4>;
    using HMat3x1 = Mat<HFloat, 3, 1>;
    using HMat3x2 = Mat<HFloat, 3, 2>;
    using HMat3x3 = Mat<HFloat, 3, 3>;
    using HMat3x4 = Mat<HFloat, 3, 4>;
    using HMat4x1 = Mat<HFloat, 4, 1>;
    using HMat4x2 = Mat<HFloat, 4, 2>;
    using HMat4x3 = Mat<HFloat, 4, 3>;
    using HMat4x4 = Mat<HFloat, 4, 4>;

    using FMat1x1 = Mat<float, 1, 1>;
    using FMat1x2 = Mat<float, 1, 2>;
    using FMat1x3 = Mat<float, 1, 3>;
    using FMat1x4 = Mat<float, 1, 4>;
    using FMat2x1 = Mat<float, 2, 1>;
    using FMat2x2 = Mat<float, 2, 2>;
    using FMat2x3 = Mat<float, 2, 3>;
    using FMat2x4 = Mat<float, 2, 4>;
    using FMat3x1 = Mat<float, 3, 1>;
    using FMat3x2 = Mat<float, 3, 2>;
    using FMat3x3 = Mat<float, 3, 3>;
    using FMat3x4 = Mat<float, 3, 4>;
    using FMat4x1 = Mat<float, 4, 1>;
    using FMat4x2 = Mat<float, 4, 2>;
    using FMat4x3 = Mat<float, 4, 3>;
    using FMat4x4 = Mat<float, 4, 4>;

    using DMat1x1 = Mat<double, 1, 1>;
    using DMat1x2 = Mat<double, 1, 2>;
    using DMat1x3 = Mat<double, 1, 3>;
    using DMat1x4 = Mat<double, 1, 4>;
    using DMat2x1 = Mat<double, 2, 1>;
    using DMat2x2 = Mat<double, 2, 2>;
    using DMat2x3 = Mat<double, 2, 3>;
    using DMat2x4 = Mat<double, 2, 4>;
    using DMat3x1 = Mat<double, 3, 1>;
    using DMat3x2 = Mat<double, 3, 2>;
    using DMat3x3 = Mat<double, 3, 3>;
    using DMat3x4 = Mat<double, 3, 4>;
    using DMat4x1 = Mat<double, 4, 1>;
    using DMat4x2 = Mat<double, 4, 2>;
    using DMat4x3 = Mat<double, 4, 3>;
    using DMat4x4 = Mat<double, 4, 4>;

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
    struct Serializer<Mat<T, R, C>> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId
            = HashUtils::StrCrc32("Common::Matrix")
            + Serializer<T>::typeId + (R << 8) + C;

        static void Serialize(SerializeStream& stream, const Mat<T, R, C>& value)
        {
            TypeIdSerializer<Mat<T, R, C>>::Serialize(stream);

            for (auto i = 0; i < R * C; i++) {
                Serializer<T>::Serialize(stream, value.data[i]);
            }
        }

        static bool Deserialize(DeserializeStream& stream, Mat<T, R, C>& value)
        {
            if (!TypeIdSerializer<Mat<T, R, C>>::Deserialize(stream)) {
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
    static void CopyValuesToMatrix(Mat<T, R, C>& matrix, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R * C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[VI] = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixRow(Mat<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyValuesToMatrixCol(Mat<T, R, C>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, size_t... VI>
    static void CopyVectorToMatrixRow(Mat<T, R, C>& matrix, uint8_t index, const Vec<T, C>& inVector, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, size_t... VI>
    static void CopyVectorToMatrixCol(Mat<T, R, C>& matrix, uint8_t index, const Vec<T, R>& inVector, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyRowVectorsToMatrix(Mat<T, R, C>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vec<T, C>>);
            CopyVectorToMatrixRow(matrix, VI, inVectors, std::make_index_sequence<C> {});
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, typename... VT, size_t... VI>
    static void CopyColVectorsToMatrix(Mat<T, R, C>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vec<T, R>>);
            CopyVectorToMatrixCol(matrix, VI, inVectors, std::make_index_sequence<R> {});
        }(), 0)... };
    }

    template <typename T, uint8_t L, size_t... VI>
    static void SetMatrixToIdentity(Mat<T, L, L>& matrix, std::index_sequence<VI...>)
    {
        static_assert(L == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, VI) = 1;
        }(), 0)... };
    }

    template <typename T, uint8_t L>
    static Mat<T, L, L> GetIdentityMatrix()
    {
        Mat<T, L, L> result;
        SetMatrixToIdentity(result, std::make_index_sequence<L> {});
        return result;
    }
}

namespace Common {
    template <typename T, uint8_t R, uint8_t C>
    template <VecN<T, C>... IT>
    requires ArgsNumEqual<R, IT...>
    Mat<T, R, C> Mat<T, R, C>::FromRowVecs(IT&&... inVectors)
    {
        Mat<T, R, C> result;
        result.SetRows(std::forward<IT>(inVectors)...);
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <VecN<T, R>... IT>
    requires ArgsNumEqual<C, IT...>
    Mat<T, R, C> Mat<T, R, C>::FromColVecs(IT&&... inVectors)
    {
        Mat<T, R, C> result;
        result.SetCols(std::forward<IT>(inVectors)...);
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>::Mat()
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = 0;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>::Mat(T inValue)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = inValue;
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>::Mat(const Mat& other)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = other.data[i];
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>::Mat(Mat&& other) noexcept
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = std::move(other.data[i]);
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator=(const Mat& other)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = other.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    requires ArgsNumGreater<1, IT...>
    Mat<T, R, C>::Mat(IT&&... inValues)
    {
        static_assert(sizeof...(IT) == R || sizeof...(IT) == R * C);
        if constexpr (sizeof...(IT) == R * C) {
            SetValues(std::forward<IT>(inValues)...);
        } else {
            SetRows(std::forward<IT>(inValues)...);
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    T& Mat<T, R, C>::At(uint8_t row, uint8_t col)
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C>
    const T& Mat<T, R, C>::At(uint8_t row, uint8_t col) const
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C>
    T& Mat<T, R, C>::operator[](uint32_t index)
    {
        Assert(index < R * C);
        return this->data[index];
    }

    template <typename T, uint8_t R, uint8_t C>
    const T& Mat<T, R, C>::operator[](uint32_t index) const
    {
        Assert(index < R * C);
        return this->data[index];
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Mat<T, R, C>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Mat<T, R, C>::operator==(const Mat& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Mat<T, R, C>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C>
    bool Mat<T, R, C>::operator!=(const Mat& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator+(T rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] + rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator-(T rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] - rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator*(T rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] * rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator/(T rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] / rhs;
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator+(const Mat& rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::operator-(const Mat& rhs) const
    {
        Mat<T, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator+=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator-=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator*=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] *= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator/=(T rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] /= rhs;
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator+=(const Mat& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] += rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C>& Mat<T, R, C>::operator-=(const Mat& rhs)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] -= rhs.data[i];
        }
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <uint8_t IC>
    Mat<T, R, IC> Mat<T, R, C>::operator*(const Mat<T, C, IC>& rhs) const
    {
        Mat<T, R, IC> result;
        for (auto i = 0; i < R; i++) {
            for (auto j = 0; j < IC; j++) {
                result.At(i, j) = this->Row(i).Dot(rhs.Col(j));
            }
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Vec<T, C> Mat<T, R, C>::Row(uint8_t index) const
    {
        Assert(index < R);
        Vec<T, C> result;
        for (auto i = 0; i < C; i++) {
            result[i] = At(index, i);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Vec<T, R> Mat<T, R, C>::Col(uint8_t index) const
    {
        Assert(index < C);
        Vec<T, R> result;
        for (auto i = 0; i < R; i++) {
            result[i] = At(i, index);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Mat<T, R, C>::SetValues(IT&&... inValues)
    {
        Internal::CopyValuesToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Mat<T, R, C>::SetRows(IT&&... inVectors)
    {
        Internal::CopyRowVectorsToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Mat<T, R, C>::SetCols(IT&&... inVectors)
    {
        Internal::CopyColVectorsToMatrix<T, R, C, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Mat<T, R, C>::SetRow(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vec<T, C>, IT...>::value) {
            Internal::CopyVectorToMatrixRow<T, R, C>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<C> {});
        } else {
            Internal::CopyValuesToMatrixRow<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename... IT>
    void Mat<T, R, C>::SetCol(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vec<T, R>, IT...>::value) {
            Internal::CopyVectorToMatrixCol<T, R, C>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<R> {});
        } else {
            Internal::CopyValuesToMatrixCol<T, R, C, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C>
    template <typename IT>
    Mat<IT, R, C> Mat<T, R, C>::CastTo() const
    {
        Mat<IT, R, C> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, C, R> Mat<T, R, C>::Transpose() const
    {
        Mat<T, C, R> result;
        for (auto i = 0; i < R; i++) {
            for (auto j = 0; j < C; j++) {
                result.At(j, i) = At(i, j);
            }
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    template <uint8_t DR, uint8_t DC>
    requires ValidSubMatDims<DR, DC, R, C>
    Mat<T, DR, DC> Mat<T, R, C>::SubMatrix() const
    {
        Mat<T, DR, DC> result;
        for (auto i = 0; i < DR; i++) {
            for (auto j = 0; j < DC; j++) {
                result.At(i, j) = At(i, j);
            }
        }
        return result;
    }


    template <typename T, uint8_t R, uint8_t C>
    bool Mat<T, R, C>::CanInverse() const
    {
        return this->Determinant() != static_cast<T>(0);
    }

    template <typename T, uint8_t R, uint8_t C>
    Mat<T, R, C> Mat<T, R, C>::Inverse() const
    {
        static_assert( R == C && R > 1 && R < 5);
        T oneOverDet = static_cast<T>(1) / this->Determinant();

        Mat<T, R, C> result;
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

            Vec<T, 4> fac0(coef00, coef00, coef02, coef03);
            Vec<T, 4> fac1(coef04, coef04, coef06, coef07);
            Vec<T, 4> fac2(coef08, coef08, coef10, coef11);
            Vec<T, 4> fac3(coef12, coef12, coef14, coef15);
            Vec<T, 4> fac4(coef16, coef16, coef18, coef19);
            Vec<T, 4> fac5(coef20, coef20, coef22, coef23);

            Vec<T, 4> vec0(this->data[1], this->data[0], this->data[0], this->data[0]);
            Vec<T, 4> vec1(this->data[5], this->data[4], this->data[4], this->data[4]);
            Vec<T, 4> vec2(this->data[9], this->data[8], this->data[8], this->data[8]);
            Vec<T, 4> vec3(this->data[13], this->data[12], this->data[12], this->data[12]);

            Vec<T, 4> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
            Vec<T, 4> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
            Vec<T, 4> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
            Vec<T, 4> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

            Vec<T, 4> signA(+1, -1, +1, -1);
            Vec<T, 4> signB(-1, +1, -1, +1);

            Vec<T, 4> col0 = inv0 * signA;
            Vec<T, 4> col1 = inv1 * signB;
            Vec<T, 4> col2 = inv2 * signA;
            Vec<T, 4> col3 = inv3 * signB;

            result.SetCol(0, col0);
            result.SetCol(1, col1);
            result.SetCol(2, col2);
            result.SetCol(3, col3);

            result = result * oneOverDet;
        }

        return result;
    }

    template <typename T, uint8_t R, uint8_t C>
    T Mat<T, R, C>::Determinant() const
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
    requires ValidMatDims<R, C>
    const Mat<T, R, C> MatConsts<T, R, C>::zero = Mat<T, R, C>(0);

    template <typename T, uint8_t L>
    requires ValidVecDim<L>
    const Mat<T, L, L> MatConsts<T, L, L>::zero = Mat<T, L, L>(0);

    template <typename T, uint8_t L>
    requires ValidVecDim<L>
    const Mat<T, L, L> MatConsts<T, L, L>::identity = Internal::GetIdentityMatrix<T, L>();

    template <typename T, uint8_t R, uint8_t C>
    Vec<T, R> operator*(const Mat<T, R, C>& mat, const Vec<T, C>& vec) {
        Vec<T, R> result;
        for (auto i = 0; i < R; i++) {
            result[i] = mat.Row(i).Dot(vec);
        }
        return result;
    }
}
