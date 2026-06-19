//
// Created by johnk on 2023/5/15.
//

#pragma once

#include <Common/Math/Simd.h>
#include <Common/Math/Vector.h>
#include <Common/Serialization.h>
#include <Common/String.h>
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

    template <typename T, MathBackend B>
    struct Quaternion;

    // matrix stored in row-major
    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    requires ValidMatDims<R, C>
    struct BaseMat {
        T data[R * C];
    };

    template <typename T, uint8_t R, uint8_t C, MathBackend B = MathBackend::defaultBackend>
    struct Mat : BaseMat<T, R, C, B> {
        using Type = T;
        static constexpr uint8_t rows = R;
        static constexpr uint8_t cols = C;
        static constexpr MathBackend backend = B;

        template <VecN<T, C, B>... IT>
        requires ArgsNumEqual<R, IT...>
        static Mat FromRowVecs(IT&&... inVectors);

        template <VecN<T, R, B>... IT>
        requires ArgsNumEqual<C, IT...>
        static Mat FromColVecs(IT&&... inVectors);

        Mat();
        Mat(T inValue); // NOLINT
        Mat(const Mat& other) = default;
        Mat(Mat&& other) noexcept = default;
        Mat& operator=(const Mat& other) = default;

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
        Mat<T, R, IC, B> operator*(const Mat<T, C, IC, B>& rhs) const;

        Vec<T, C, B> Row(uint8_t index) const;
        Vec<T, R, B> Col(uint8_t index) const;

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
        Mat<IT, R, C, B> CastTo() const;

        Mat<T, C, R, B> Transpose() const;

        template<uint8_t DR, uint8_t DC>
        requires ValidSubMatDims<DR, DC, R, C>
        Mat<T, DR, DC, B> SubMatrix() const;

        bool CanInverse() const;
        Mat Inverse() const;
        T Determinant() const;

        Vec<T, 3, B> ExtractTranslation() const;
        Vec<T, 3, B> ExtractScale() const;
        Quaternion<T, B> ExtractRotation() const;
    };

    template <typename T, uint8_t R, uint8_t C, MathBackend B = MathBackend::defaultBackend>
    requires ValidMatDims<R, C>
    struct MatConsts {
        static const Mat<T, R, C, B> zero;
    };

    template <typename T, uint8_t L, MathBackend B>
    requires ValidVecDim<L>
    struct MatConsts<T, L, L, B> {
        static const Mat<T, L, L, B> zero;
        static const Mat<T, L, L, B> identity;
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
    template <Serializable T, uint8_t R, uint8_t C, MathBackend B>
    struct Serializer<Mat<T, R, C, B>> {
        static constexpr size_t typeId
            = HashUtils::StrCrc32("Common::Matrix")
            + Serializer<T>::typeId + (R << 8) + C;

        static size_t Serialize(BinarySerializeStream& stream, const Mat<T, R, C, B>& value)
        {
            auto serialized = 0;
            for (auto i = 0; i < R * C; i++) {
                serialized += Serializer<T>::Serialize(stream, value.data[i]);
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, Mat<T, R, C, B>& value)
        {
            auto deserialized = 0;
            for (auto i = 0; i < R * C; i++) {
                deserialized += Serializer<T>::Deserialize(stream, value.data[i]);
            }
            return deserialized;
        }
    };

    template <StringConvertible T, uint8_t R, uint8_t C, MathBackend B>
    struct StringConverter<Mat<T, R, C, B>> {
        static std::string ToString(const Mat<T, R, C, B>& inValue)
        {
            std::stringstream stream;
            stream << "(";
            for (auto i = 0; i < R; i++) {
                for (auto j = 0; j < C; j++) {
                    stream << StringConverter<T>::ToString(inValue.At(i, j));
                    if (i * C + j != R * C - 1) {
                        stream << ", ";
                    }
                }
            }
            stream << ")";
            return stream.str();
        }
    };

    template <StringConvertible T, uint8_t R, uint8_t C, MathBackend B>
    struct JsonSerializer<Mat<T, R, C, B>> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mat<T, R, C, B>& inValue)
        {
            outJsonValue.SetArray();
            outJsonValue.Reserve(R * C, inAllocator);
            for (auto i = 0; i < R; i++) {
                for (auto j = 0; j < C; j++) {
                    rapidjson::Value jsonElement;
                    JsonSerializer<T>::JsonSerialize(jsonElement, inAllocator, inValue.At(i, j));
                    outJsonValue.PushBack(jsonElement, inAllocator);
                }
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, Mat<T, R, C, B>& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != R * C) {
                return;
            }
            for (auto i = 0; i < inJsonValue.Size(); i++) {
                auto row = i / C;
                auto col = i % C;

                T element;
                JsonSerializer<T>::JsonDeserialize(inJsonValue[i], element);
                outValue.At(row, col) = std::move(element);
            }
        }
    };
}

namespace Common::Internal {
    template <typename T, uint8_t R, uint8_t C, MathBackend B, typename... VT, size_t... VI>
    static void CopyValuesToMatrix(Mat<T, R, C, B>& matrix, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R * C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.data[VI] = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, typename... VT, size_t... VI>
    static void CopyValuesToMatrixRow(Mat<T, R, C, B>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, typename... VT, size_t... VI>
    static void CopyValuesToMatrixCol(Mat<T, R, C, B>& matrix, uint8_t index, VT&&... inValue, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inValue;
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, size_t... VI>
    static void CopyVectorToMatrixRow(Mat<T, R, C, B>& matrix, uint8_t index, const Vec<T, C, B>& inVector, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(index, VI) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, size_t... VI>
    static void CopyVectorToMatrixCol(Mat<T, R, C, B>& matrix, uint8_t index, const Vec<T, R, B>& inVector, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, index) = inVector[VI];
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, typename... VT, size_t... VI>
    static void CopyRowVectorsToMatrix(Mat<T, R, C, B>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(R == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vec<T, C, B>>);
            CopyVectorToMatrixRow(matrix, VI, inVectors, std::make_index_sequence<C> {});
        }(), 0)... };
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B, typename... VT, size_t... VI>
    static void CopyColVectorsToMatrix(Mat<T, R, C, B>& matrix, VT&&... inVectors, std::index_sequence<VI...>)
    {
        static_assert(C == sizeof...(VT) && sizeof...(VT) == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            static_assert(std::is_same_v<VT, Vec<T, R, B>>);
            CopyVectorToMatrixCol(matrix, VI, inVectors, std::make_index_sequence<R> {});
        }(), 0)... };
    }

    template <typename T, uint8_t L, MathBackend B, size_t... VI>
    static void SetMatrixToIdentity(Mat<T, L, L, B>& matrix, std::index_sequence<VI...>)
    {
        static_assert(L == sizeof...(VI));
        (void) std::initializer_list<int> { ([&]() -> void {
            matrix.At(VI, VI) = 1;
        }(), 0)... };
    }

    template <typename T, uint8_t L, MathBackend B>
    static Mat<T, L, L, B> GetIdentityMatrix()
    {
        Mat<T, L, L, B> result;
        SetMatrixToIdentity(result, std::make_index_sequence<L> {});
        return result;
    }

}

namespace Common::Internal {
    // Single source of truth for the scalar matrix kernels. The primary MatOps and the SIMD specializations both route
    // their non-accelerated operations here, so the generic implementation lives exactly once and Mat's members stay
    // pure delegations to MatOps.
    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, C, R, B> MatTransposeScalar(const Mat<T, R, C, B>& m)
    {
        Mat<T, C, R, B> result;
        for (auto i = 0; i < R; i++) {
            for (auto j = 0; j < C; j++) {
                result.At(j, i) = m.At(i, j);
            }
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, R, B> MatMulVecScalar(const Mat<T, R, C, B>& mat, const Vec<T, C, B>& vec)
    {
        Vec<T, R, B> result;
        for (auto i = 0; i < R; i++) {
            result[i] = mat.Row(i).Dot(vec);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    T MatDeterminantScalar(const Mat<T, R, C, B>& m)
    {
        static_assert(R == C && R > 1 && R < 5);
        T result = static_cast<T>(0);
        if constexpr (R == 2) {
            result = m.data[0] * m.data[3] - m.data[1] * m.data[2];
        }

        if constexpr (R == 3) {
            result =
                m.data[0] * (m.data[4] * m.data[8] - m.data[5] * m.data[7])
                - m.data[1] * (m.data[3] * m.data[8] - m.data[5] * m.data[6])
                + m.data[2] * (m.data[3] * m.data[7] - m.data[4] * m.data[6]);
        }

        if constexpr (R == 4) {
            T subFactor0 = m.data[10] * m.data[15] - m.data[11] * m.data[14];
            T subFactor1 = m.data[6] * m.data[15] - m.data[7] * m.data[14];
            T subFactor2 = m.data[6] * m.data[11] - m.data[7] * m.data[10];
            T subFactor3 = m.data[2] * m.data[15] - m.data[3] * m.data[14];
            T subFactor4 = m.data[2] * m.data[11] - m.data[3] * m.data[10];
            T subFactor5 = m.data[2] * m.data[7] - m.data[3] * m.data[6];

            T detCoef0 = m.data[5] * subFactor0 - m.data[9] * subFactor1 + m.data[13] * subFactor2;
            T detCoef1 = m.data[9] * subFactor3 - m.data[1] * subFactor0 - m.data[13] * subFactor4;
            T detCoef2 = m.data[1] * subFactor1 - m.data[5] * subFactor3 + m.data[13] * subFactor5;
            T detCoef3 = m.data[5] * subFactor4 - m.data[1] * subFactor2 - m.data[9] * subFactor5;

            result = m.data[0] * detCoef0 + m.data[4] * detCoef1 + m.data[8] * detCoef2 + m.data[12] * detCoef3;
        }

        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> MatInverseScalar(const Mat<T, R, C, B>& m)
    {
        static_assert(R == C && R > 1 && R < 5);
        T oneOverDet = static_cast<T>(1) / MatDeterminantScalar(m);

        Mat<T, R, C, B> result;
        if constexpr (R == 2) {
            result.At(0, 0) = m.data[3] * oneOverDet;
            result.At(0, 1) = -m.data[1] * oneOverDet;
            result.At(1, 0) = -m.data[2] * oneOverDet;
            result.At(1, 1) = m.data[0] * oneOverDet;
        }

        if constexpr (R == 3) {
            result.At(0, 0) = (m.data[4] * m.data[8] - m.data[5] * m.data[7]) * oneOverDet;
            result.At(0, 1) = (m.data[2] * m.data[7] - m.data[1] * m.data[8]) * oneOverDet;
            result.At(0, 2) = (m.data[1] * m.data[5] - m.data[2] * m.data[4]) * oneOverDet;
            result.At(1, 0) = (m.data[5] * m.data[6] - m.data[3] * m.data[8]) * oneOverDet;
            result.At(1, 1) = (m.data[0] * m.data[8] - m.data[2] * m.data[6]) * oneOverDet;
            result.At(1, 2) = (m.data[2] * m.data[3] - m.data[0] * m.data[5]) * oneOverDet;
            result.At(2, 0) = (m.data[3] * m.data[7] - m.data[4] * m.data[6]) * oneOverDet;
            result.At(2, 1) = (m.data[1] * m.data[6] - m.data[7] * m.data[0]) * oneOverDet;
            result.At(2, 2) = (m.data[0] * m.data[4] - m.data[1] * m.data[3]) * oneOverDet;
        }

        if constexpr (R == 4) {
            T coef00 = m.data[10] * m.data[15] - m.data[11] * m.data[14];
            T coef02 = m.data[9] * m.data[15] - m.data[11] * m.data[13];
            T coef03 = m.data[9] * m.data[14] - m.data[10] * m.data[13];

            T coef04 = m.data[6] * m.data[15] - m.data[7] * m.data[14];
            T coef06 = m.data[5] * m.data[15] - m.data[7] * m.data[13];
            T coef07 = m.data[5] * m.data[14] - m.data[6] * m.data[13];

            T coef08 = m.data[6] * m.data[11] - m.data[7] * m.data[10];
            T coef10 = m.data[5] * m.data[11] - m.data[7] * m.data[9];
            T coef11 = m.data[5] * m.data[10] - m.data[6] * m.data[9];

            T coef12 = m.data[2] * m.data[15] - m.data[3] * m.data[14];
            T coef14 = m.data[1] * m.data[15] - m.data[3] * m.data[13];
            T coef15 = m.data[1] * m.data[14] - m.data[2] * m.data[13];

            T coef16 = m.data[2] * m.data[11] - m.data[3] * m.data[10];
            T coef18 = m.data[1] * m.data[11] - m.data[3] * m.data[9];
            T coef19 = m.data[1] * m.data[10] - m.data[2] * m.data[9];

            T coef20 = m.data[2] * m.data[7] - m.data[3] * m.data[6];
            T coef22 = m.data[1] * m.data[7] - m.data[3] * m.data[5];
            T coef23 = m.data[1] * m.data[6] - m.data[2] * m.data[5];

            Vec<T, 4, B> fac0(coef00, coef00, coef02, coef03);
            Vec<T, 4, B> fac1(coef04, coef04, coef06, coef07);
            Vec<T, 4, B> fac2(coef08, coef08, coef10, coef11);
            Vec<T, 4, B> fac3(coef12, coef12, coef14, coef15);
            Vec<T, 4, B> fac4(coef16, coef16, coef18, coef19);
            Vec<T, 4, B> fac5(coef20, coef20, coef22, coef23);

            Vec<T, 4, B> vec0(m.data[1], m.data[0], m.data[0], m.data[0]);
            Vec<T, 4, B> vec1(m.data[5], m.data[4], m.data[4], m.data[4]);
            Vec<T, 4, B> vec2(m.data[9], m.data[8], m.data[8], m.data[8]);
            Vec<T, 4, B> vec3(m.data[13], m.data[12], m.data[12], m.data[12]);

            Vec<T, 4, B> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
            Vec<T, 4, B> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
            Vec<T, 4, B> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
            Vec<T, 4, B> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

            Vec<T, 4, B> signA(+1, -1, +1, -1);
            Vec<T, 4, B> signB(-1, +1, -1, +1);

            Vec<T, 4, B> col0 = inv0 * signA;
            Vec<T, 4, B> col1 = inv1 * signB;
            Vec<T, 4, B> col2 = inv2 * signA;
            Vec<T, 4, B> col3 = inv3 * signB;

            result.SetCol(0, col0);
            result.SetCol(1, col1);
            result.SetCol(2, col2);
            result.SetCol(3, col3);

            result = result * oneOverDet;
        }

        return result;
    }

    // Per-backend dispatch for matrix operations. The primary template forwards every operation to the scalar kernels
    // above; the SIMD specializations override the ones that benefit and forward the rest, so each Mat member is a
    // single MatOps call regardless of backend.
    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    struct MatOps {
        static Mat<T, R, C, B> Add(const Mat<T, R, C, B>& a, const Mat<T, R, C, B>& b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] + b.data[i]; }
            return result;
        }

        static Mat<T, R, C, B> Sub(const Mat<T, R, C, B>& a, const Mat<T, R, C, B>& b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] - b.data[i]; }
            return result;
        }

        static Mat<T, R, C, B> AddScalar(const Mat<T, R, C, B>& a, T b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] + b; }
            return result;
        }

        static Mat<T, R, C, B> SubScalar(const Mat<T, R, C, B>& a, T b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] - b; }
            return result;
        }

        static Mat<T, R, C, B> MulScalar(const Mat<T, R, C, B>& a, T b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] * b; }
            return result;
        }

        static Mat<T, R, C, B> DivScalar(const Mat<T, R, C, B>& a, T b)
        {
            Mat<T, R, C, B> result;
            for (auto i = 0; i < R * C; i++) { result.data[i] = a.data[i] / b; }
            return result;
        }

        static Mat<T, C, R, B> Transpose(const Mat<T, R, C, B>& m) { return MatTransposeScalar(m); }
        static Vec<T, R, B> MulVec(const Mat<T, R, C, B>& m, const Vec<T, C, B>& v) { return MatMulVecScalar(m, v); }
        static T Determinant(const Mat<T, R, C, B>& m) { return MatDeterminantScalar(m); }
        static Mat<T, R, C, B> Inverse(const Mat<T, R, C, B>& m) { return MatInverseScalar(m); }
    };

    // Row-major 4x4 float matrix, backed by float[16] (four contiguous rows of 16 bytes each), so each row maps to an
    // unaligned 128-bit load/store. Besides the element-wise ops shared with the primary template, this also offers a
    // SIMD matrix product (Mul) that Mat::operator* dispatches to for the 4x4 float case.
    template <>
    struct MatOps<float, 4, 4, MathBackend::simd> {
        using M = Mat<float, 4, 4, MathBackend::simd>;
        using V = Vec<float, 4, MathBackend::simd>;

        static M Add(const M& a, const M& b) { M r; Simd::MapBinary<16>(r.data, a.data, b.data, Simd::AddOp {}); return r; }
        static M Sub(const M& a, const M& b) { M r; Simd::MapBinary<16>(r.data, a.data, b.data, Simd::SubOp {}); return r; }

        static M AddScalar(const M& a, float b) { M r; Simd::MapScalar<16>(r.data, a.data, b, Simd::AddOp {}); return r; }
        static M SubScalar(const M& a, float b) { M r; Simd::MapScalar<16>(r.data, a.data, b, Simd::SubOp {}); return r; }
        static M MulScalar(const M& a, float b) { M r; Simd::MapScalar<16>(r.data, a.data, b, Simd::MulOp {}); return r; }
        static M DivScalar(const M& a, float b) { M r; Simd::MapScalar<16>(r.data, a.data, b, Simd::DivOp {}); return r; }

        // C = A * B, row-major. Each output row is a linear combination of B's rows weighted by one row of A:
        // C_row_i = A[i][0]*B_row0 + A[i][1]*B_row1 + A[i][2]*B_row2 + A[i][3]*B_row3.
        static M Mul(const M& a, const M& b)
        {
            const Simd::F32x4 bRow0 = Simd::LoadU(&b.data[0]);
            const Simd::F32x4 bRow1 = Simd::LoadU(&b.data[4]);
            const Simd::F32x4 bRow2 = Simd::LoadU(&b.data[8]);
            const Simd::F32x4 bRow3 = Simd::LoadU(&b.data[12]);

            M result;
            for (auto i = 0; i < 4; i++) {
                const Simd::F32x4 row = Simd::Add(
                    Simd::Add(
                        Simd::Mul(Simd::Set1(a.data[i * 4 + 0]), bRow0),
                        Simd::Mul(Simd::Set1(a.data[i * 4 + 1]), bRow1)),
                    Simd::Add(
                        Simd::Mul(Simd::Set1(a.data[i * 4 + 2]), bRow2),
                        Simd::Mul(Simd::Set1(a.data[i * 4 + 3]), bRow3)));
                Simd::StoreU(&result.data[i * 4], row);
            }
            return result;
        }

        // result = M * v, with v a column vector: result[i] = dot(row_i, v). Loading v once and reducing each row with
        // the SIMD horizontal sum avoids the temporary row Vec the scalar path builds. The reduction order matches
        // VecOps<float, 4, simd>::Dot, so the result agrees with the scalar backend within float tolerance.
        static V MulVec(const M& m, const V& v)
        {
            const Simd::F32x4 vv = Simd::LoadU(v.data);
            V result;
            result.data[0] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[0]), vv));
            result.data[1] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[4]), vv));
            result.data[2] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[8]), vv));
            result.data[3] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[12]), vv));
            return result;
        }

        static M Transpose(const M& m)
        {
            Simd::F32x4 r0 = Simd::LoadU(&m.data[0]);
            Simd::F32x4 r1 = Simd::LoadU(&m.data[4]);
            Simd::F32x4 r2 = Simd::LoadU(&m.data[8]);
            Simd::F32x4 r3 = Simd::LoadU(&m.data[12]);
            Simd::Transpose4(r0, r1, r2, r3);

            M result;
            Simd::StoreU(&result.data[0], r0);
            Simd::StoreU(&result.data[4], r1);
            Simd::StoreU(&result.data[8], r2);
            Simd::StoreU(&result.data[12], r3);
            return result;
        }

        // Cofactor-expansion inverse, mirroring the scalar Mat4 path but building every 2x2 cofactor vector (fac) and
        // the row-broadcast vectors (vec) with single-row shuffles instead of scalar gathers. fac for the row pair
        // (p, q) is (p2*q3 - p3*q2, same, p1*q3 - p3*q1, p1*q2 - p2*q1). The cofactor columns are transposed into rows,
        // and the determinant is recovered as row0 . (first row of the cofactor matrix), so there is no second
        // Determinant pass.
        static M Inverse(const M& m)
        {
            const Simd::F32x4 r0 = Simd::LoadU(&m.data[0]);
            const Simd::F32x4 r1 = Simd::LoadU(&m.data[4]);
            const Simd::F32x4 r2 = Simd::LoadU(&m.data[8]);
            const Simd::F32x4 r3 = Simd::LoadU(&m.data[12]);

            const auto makeFac = [](const Simd::F32x4 p, const Simd::F32x4 q) {
                return Simd::Sub(
                    Simd::Mul(Simd::Shuffle<2, 2, 1, 1>(p), Simd::Shuffle<3, 3, 3, 2>(q)),
                    Simd::Mul(Simd::Shuffle<3, 3, 3, 2>(p), Simd::Shuffle<2, 2, 1, 1>(q)));
            };

            const Simd::F32x4 fac0 = makeFac(r2, r3);
            const Simd::F32x4 fac1 = makeFac(r1, r3);
            const Simd::F32x4 fac2 = makeFac(r1, r2);
            const Simd::F32x4 fac3 = makeFac(r0, r3);
            const Simd::F32x4 fac4 = makeFac(r0, r2);
            const Simd::F32x4 fac5 = makeFac(r0, r1);

            const Simd::F32x4 vec0 = Simd::Shuffle<1, 0, 0, 0>(r0);
            const Simd::F32x4 vec1 = Simd::Shuffle<1, 0, 0, 0>(r1);
            const Simd::F32x4 vec2 = Simd::Shuffle<1, 0, 0, 0>(r2);
            const Simd::F32x4 vec3 = Simd::Shuffle<1, 0, 0, 0>(r3);

            const Simd::F32x4 inv0 = Simd::Add(Simd::Sub(Simd::Mul(vec1, fac0), Simd::Mul(vec2, fac1)), Simd::Mul(vec3, fac2));
            const Simd::F32x4 inv1 = Simd::Add(Simd::Sub(Simd::Mul(vec0, fac0), Simd::Mul(vec2, fac3)), Simd::Mul(vec3, fac4));
            const Simd::F32x4 inv2 = Simd::Add(Simd::Sub(Simd::Mul(vec0, fac1), Simd::Mul(vec1, fac3)), Simd::Mul(vec3, fac5));
            const Simd::F32x4 inv3 = Simd::Add(Simd::Sub(Simd::Mul(vec0, fac2), Simd::Mul(vec1, fac4)), Simd::Mul(vec2, fac5));

            const Simd::F32x4 signA = Simd::Set(1.0f, -1.0f, 1.0f, -1.0f);
            const Simd::F32x4 signB = Simd::Set(-1.0f, 1.0f, -1.0f, 1.0f);

            Simd::F32x4 col0 = Simd::Mul(inv0, signA);
            Simd::F32x4 col1 = Simd::Mul(inv1, signB);
            Simd::F32x4 col2 = Simd::Mul(inv2, signA);
            Simd::F32x4 col3 = Simd::Mul(inv3, signB);

            // det = row0 . (column 0 of the cofactor matrix). That column is the col0 register as-is, so compute the
            // determinant before Transpose4 turns col0 into the cofactor matrix's first row.
            const float det = Simd::Sum(Simd::Mul(r0, col0));
            const Simd::F32x4 oneOverDet = Simd::Set1(1.0f / det);

            Simd::Transpose4(col0, col1, col2, col3);

            M result;
            Simd::StoreU(&result.data[0], Simd::Mul(col0, oneOverDet));
            Simd::StoreU(&result.data[4], Simd::Mul(col1, oneOverDet));
            Simd::StoreU(&result.data[8], Simd::Mul(col2, oneOverDet));
            Simd::StoreU(&result.data[12], Simd::Mul(col3, oneOverDet));
            return result;
        }

        static float Determinant(const M& m) { return MatDeterminantScalar(m); }
    };

    // Row-major 3x3 float matrix, backed by a tight float[9] (no padding, so the layout stays GPU/serialization
    // friendly). The first eight elements are covered by two safe 128-bit loads (data[0..3], data[4..7]) with data[8]
    // handled by a scalar tail; matrix-product rows and the transpose use Load3/Store3 to avoid over-running the
    // float[9] on the last row. The 4th lane is always discarded on store, so the garbage it may carry is harmless.
    template <>
    struct MatOps<float, 3, 3, MathBackend::simd> {
        using M = Mat<float, 3, 3, MathBackend::simd>;
        using V = Vec<float, 3, MathBackend::simd>;

        // MapBinary/MapScalar<9> cover data[0..7] with two safe 128-bit loads (the second, at index 4, reads data[4..7])
        // and finish data[8] in the scalar tail, so the float[9] is never over-run.
        static M Add(const M& a, const M& b) { M r; Simd::MapBinary<9>(r.data, a.data, b.data, Simd::AddOp {}); return r; }
        static M Sub(const M& a, const M& b) { M r; Simd::MapBinary<9>(r.data, a.data, b.data, Simd::SubOp {}); return r; }

        static M AddScalar(const M& a, float b) { M r; Simd::MapScalar<9>(r.data, a.data, b, Simd::AddOp {}); return r; }
        static M SubScalar(const M& a, float b) { M r; Simd::MapScalar<9>(r.data, a.data, b, Simd::SubOp {}); return r; }
        static M MulScalar(const M& a, float b) { M r; Simd::MapScalar<9>(r.data, a.data, b, Simd::MulOp {}); return r; }
        static M DivScalar(const M& a, float b) { M r; Simd::MapScalar<9>(r.data, a.data, b, Simd::DivOp {}); return r; }

        // C_row_i = A[i][0]*B_row0 + A[i][1]*B_row1 + A[i][2]*B_row2. B_row0/B_row1 come from safe full loads (their 4th
        // lane is the next row's first element, unused); B_row2 uses Load3 to stay in bounds.
        static M Mul(const M& a, const M& b)
        {
            const Simd::F32x4 bRow0 = Simd::LoadU(&b.data[0]);
            const Simd::F32x4 bRow1 = Simd::LoadU(&b.data[3]);
            const Simd::F32x4 bRow2 = Simd::Load3(&b.data[6]);

            M result;
            for (auto i = 0; i < 3; i++) {
                const Simd::F32x4 row = Simd::Add(
                    Simd::Add(
                        Simd::Mul(Simd::Set1(a.data[i * 3 + 0]), bRow0),
                        Simd::Mul(Simd::Set1(a.data[i * 3 + 1]), bRow1)),
                    Simd::Mul(Simd::Set1(a.data[i * 3 + 2]), bRow2));
                Simd::Store3(&result.data[i * 3], row);
            }
            return result;
        }

        // result[i] = dot(row_i, v). v is loaded with Load3 so its 4th lane is 0, which zeroes the unused 4th lane the
        // full row loads carry, leaving the 4-wide horizontal sum equal to the 3-component dot.
        static V MulVec(const M& m, const V& v)
        {
            const Simd::F32x4 vv = Simd::Load3(v.data);
            V result;
            result.data[0] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[0]), vv));
            result.data[1] = Simd::Sum(Simd::Mul(Simd::LoadU(&m.data[3]), vv));
            result.data[2] = Simd::Sum(Simd::Mul(Simd::Load3(&m.data[6]), vv));
            return result;
        }

        // 3x3 transpose via the 4x4 primitive with a zero 4th row: the garbage in the loaded rows' 4th lanes only lands
        // in the discarded 4th output row, so the three Store3'd rows are the exact transpose.
        static M Transpose(const M& m)
        {
            Simd::F32x4 r0 = Simd::LoadU(&m.data[0]);
            Simd::F32x4 r1 = Simd::LoadU(&m.data[3]);
            Simd::F32x4 r2 = Simd::Load3(&m.data[6]);
            Simd::F32x4 r3 = Simd::Set1(0.0f);
            Simd::Transpose4(r0, r1, r2, r3);

            M result;
            Simd::Store3(&result.data[0], r0);
            Simd::Store3(&result.data[3], r1);
            Simd::Store3(&result.data[6], r2);
            return result;
        }

        static M Inverse(const M& m) { return MatInverseScalar(m); }
        static float Determinant(const M& m) { return MatDeterminantScalar(m); }
    };
}

namespace Common {
    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <VecN<T, C, B>... IT>
    requires ArgsNumEqual<R, IT...>
    Mat<T, R, C, B> Mat<T, R, C, B>::FromRowVecs(IT&&... inVectors)
    {
        Mat<T, R, C, B> result;
        result.SetRows(std::forward<IT>(inVectors)...);
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <VecN<T, R, B>... IT>
    requires ArgsNumEqual<C, IT...>
    Mat<T, R, C, B> Mat<T, R, C, B>::FromColVecs(IT&&... inVectors)
    {
        Mat<T, R, C, B> result;
        result.SetCols(std::forward<IT>(inVectors)...);
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>::Mat()
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = 0;
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>::Mat(T inValue)
    {
        for (auto i = 0; i < R * C; i++) {
            this->data[i] = inValue;
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    requires ArgsNumGreater<1, IT...>
    Mat<T, R, C, B>::Mat(IT&&... inValues)
    {
        static_assert(sizeof...(IT) == R || sizeof...(IT) == R * C);
        if constexpr (sizeof...(IT) == R * C) {
            SetValues(std::forward<IT>(inValues)...);
        } else {
            SetRows(std::forward<IT>(inValues)...);
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    T& Mat<T, R, C, B>::At(uint8_t row, uint8_t col)
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    const T& Mat<T, R, C, B>::At(uint8_t row, uint8_t col) const
    {
        Assert(row < R && col < C);
        return this->data[row * C + col];
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    T& Mat<T, R, C, B>::operator[](uint32_t index)
    {
        Assert(index < R * C);
        return this->data[index];
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    const T& Mat<T, R, C, B>::operator[](uint32_t index) const
    {
        Assert(index < R * C);
        return this->data[index];
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    bool Mat<T, R, C, B>::operator==(T rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    bool Mat<T, R, C, B>::operator==(const Mat& rhs) const
    {
        bool result = true;
        for (auto i = 0; i < R * C; i++) {
            result = result && CompareNumber(this->data[i], rhs.data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    bool Mat<T, R, C, B>::operator!=(T rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    bool Mat<T, R, C, B>::operator!=(const Mat& rhs) const
    {
        return !this->operator==(rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator+(T rhs) const
    {
        return Internal::MatOps<T, R, C, B>::AddScalar(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator-(T rhs) const
    {
        return Internal::MatOps<T, R, C, B>::SubScalar(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator*(T rhs) const
    {
        return Internal::MatOps<T, R, C, B>::MulScalar(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator/(T rhs) const
    {
        return Internal::MatOps<T, R, C, B>::DivScalar(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator+(const Mat& rhs) const
    {
        return Internal::MatOps<T, R, C, B>::Add(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::operator-(const Mat& rhs) const
    {
        return Internal::MatOps<T, R, C, B>::Sub(*this, rhs);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator+=(T rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::AddScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator-=(T rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::SubScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator*=(T rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::MulScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator/=(T rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::DivScalar(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator+=(const Mat& rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::Add(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B>& Mat<T, R, C, B>::operator-=(const Mat& rhs)
    {
        *this = Internal::MatOps<T, R, C, B>::Sub(*this, rhs);
        return *this;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <uint8_t IC>
    Mat<T, R, IC, B> Mat<T, R, C, B>::operator*(const Mat<T, C, IC, B>& rhs) const
    {
        if constexpr (B == MathBackend::simd && std::is_same_v<T, float> && R == 4 && C == 4 && IC == 4) {
            return Internal::MatOps<float, 4, 4, MathBackend::simd>::Mul(*this, rhs);
        } else if constexpr (B == MathBackend::simd && std::is_same_v<T, float> && R == 3 && C == 3 && IC == 3) {
            return Internal::MatOps<float, 3, 3, MathBackend::simd>::Mul(*this, rhs);
        } else {
            // Row-linear-combination order (ikj): each result row is sum_k A[i][k] * B_row_k. Unlike the textbook
            // Row(i).Dot(Col(j)) form it builds no temporary vectors and does not gather B's columns with a stride. The
            // row base pointers are hoisted so the inner j loop is plain contiguous accesses over rhs and result with no
            // per-element bounds checks, which lets the compiler auto-vectorize it. The k=0 term seeds the row so no
            // separate zeroing pass is needed; accumulation stays in ascending k order, so the numerical result matches
            // the scalar dot-product and the SIMD paths.
            Mat<T, R, IC, B> result;
            for (auto i = 0; i < R; i++) {
                T* resultRow = &result.data[i * IC];
                const T* aRow = &this->data[i * C];
                for (auto j = 0; j < IC; j++) {
                    resultRow[j] = aRow[0] * rhs.data[j];
                }
                for (auto k = 1; k < C; k++) {
                    const T aik = aRow[k];
                    const T* bRow = &rhs.data[k * IC];
                    for (auto j = 0; j < IC; j++) {
                        resultRow[j] += aik * bRow[j];
                    }
                }
            }
            return result;
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, C, B> Mat<T, R, C, B>::Row(uint8_t index) const
    {
        Assert(index < R);
        Vec<T, C, B> result;
        for (auto i = 0; i < C; i++) {
            result[i] = At(index, i);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, R, B> Mat<T, R, C, B>::Col(uint8_t index) const
    {
        Assert(index < C);
        Vec<T, R, B> result;
        for (auto i = 0; i < R; i++) {
            result[i] = At(i, index);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    void Mat<T, R, C, B>::SetValues(IT&&... inValues)
    {
        Internal::CopyValuesToMatrix<T, R, C, B, IT...>(*this, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    void Mat<T, R, C, B>::SetRows(IT&&... inVectors)
    {
        Internal::CopyRowVectorsToMatrix<T, R, C, B, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    void Mat<T, R, C, B>::SetCols(IT&&... inVectors)
    {
        Internal::CopyColVectorsToMatrix<T, R, C, B, IT...>(*this, std::forward<IT>(inVectors)..., std::make_index_sequence<sizeof...(IT)> {});
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    void Mat<T, R, C, B>::SetRow(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vec<T, C, B>, IT...>::value) {
            Internal::CopyVectorToMatrixRow<T, R, C, B>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<C> {});
        } else {
            Internal::CopyValuesToMatrixRow<T, R, C, B, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename... IT>
    void Mat<T, R, C, B>::SetCol(uint8_t index, IT&&... inValues)
    {
        if constexpr (sizeof...(IT) == 1 && IsAllSame<Vec<T, R, B>, IT...>::value) {
            Internal::CopyVectorToMatrixCol<T, R, C, B>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<R> {});
        } else {
            Internal::CopyValuesToMatrixCol<T, R, C, B, IT...>(*this, index, std::forward<IT>(inValues)..., std::make_index_sequence<sizeof...(IT)> {});
        }
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <typename IT>
    Mat<IT, R, C, B> Mat<T, R, C, B>::CastTo() const
    {
        Mat<IT, R, C, B> result;
        for (auto i = 0; i < R * C; i++) {
            result.data[i] = static_cast<IT>(this->data[i]);
        }
        return result;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, C, R, B> Mat<T, R, C, B>::Transpose() const
    {
        return Internal::MatOps<T, R, C, B>::Transpose(*this);
    }

    template<typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, 3, B> Mat<T, R, C, B>::ExtractTranslation() const
    {
        static_assert( R == 4 && C == 4);
        Vec<T, 3, B> ret = Vec<T, 3, B>(this->data[3], this->data[7], this->data[11]);
        return ret;
    }

    template<typename T, uint8_t R, uint8_t C, MathBackend B>
    Quaternion<T, B> Mat<T, R, C, B>::ExtractRotation() const
    {
        static_assert( R == 4 && C == 4);
        Quaternion<T, B> ret = Quaternion<T, B>(1, 0, 0, 0);

        T sx = Vec<T, 3, B>(this->data[0], this->data[4], this->data[8]).Model();
        T sy = Vec<T, 3, B>(this->data[1], this->data[5], this->data[9]).Model();
        T sz = Vec<T, 3, B>(this->data[2], this->data[6], this->data[10]).Model();
        T det = this->Determinant();
        if (det < 0) {
            sx = -sx;
        }

        T m11 = this->data[0] / sx, m21 = this->data[4] / sx, m31 = this->data[8] / sx;
        T m12 = this->data[1] / sy, m22 = this->data[5] / sy, m32 = this->data[9] / sy;
        T m13 = this->data[2] / sz, m23 = this->data[6] / sz, m33 = this->data[10] / sz;

        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
        // assumes the upper 3x3 of m is a pure rotation matrix (i.e, unscaled)
        T trace = m11 + m22 + m33;
        if (trace > 0) {
            T s = 0.5 / std::sqrt(trace + 1.0);
            ret.w = 0.25 / s;
            ret.x = (m32 - m23) * s;
            ret.y = (m13 - m31) * s;
            ret.z = (m21 - m12) * s;
        } else if (m11 > m22 && m11 > m33) {
            T s = 2.0 * std::sqrt(1.0 + m11 - m22 - m33);
            ret.w = ( m32 - m23 ) / s;
            ret.x = 0.25 * s;
            ret.y = ( m12 + m21 ) / s;
            ret.z = ( m13 + m31 ) / s;
        } else if (m22 > m33) {
            T s = 2.0 * std::sqrt( 1.0 + m22 - m11 - m33 );
            ret.w = ( m13 - m31 ) / s;
            ret.x = ( m12 + m21 ) / s;
            ret.y = 0.25 * s;
            ret.z = ( m23 + m32 ) / s;
        } else {
            T s = 2.0 * std::sqrt( 1.0 + m33 - m11 - m22 );
            ret.w = ( m21 - m12 ) / s;
            ret.x = ( m13 + m31 ) / s;
            ret.y = ( m23 + m32 ) / s;
            ret.z = 0.25 * s;
        }

        return ret;
    }

    template<typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, 3, B> Mat<T, R, C, B>::ExtractScale() const
    {
        static_assert( R == 4 && C == 4);

        T sx = Vec<T, 3, B>(this->data[0], this->data[4], this->data[8]).Model();
        T sy = Vec<T, 3, B>(this->data[1], this->data[5], this->data[9]).Model();
        T sz = Vec<T, 3, B>(this->data[2], this->data[6], this->data[10]).Model();

        T det = this->Determinant();
        if (det < 0) {
            sx = -sx;
        }
        Vec<T, 3, B> ret = Vec<T, 3, B>(sx, sy, sz);

        return ret;
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    template <uint8_t DR, uint8_t DC>
    requires ValidSubMatDims<DR, DC, R, C>
    Mat<T, DR, DC, B> Mat<T, R, C, B>::SubMatrix() const
    {
        Mat<T, DR, DC, B> result;
        for (auto i = 0; i < DR; i++) {
            for (auto j = 0; j < DC; j++) {
                result.At(i, j) = At(i, j);
            }
        }
        return result;
    }


    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    bool Mat<T, R, C, B>::CanInverse() const
    {
        return this->Determinant() != static_cast<T>(0);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Mat<T, R, C, B> Mat<T, R, C, B>::Inverse() const
    {
        return Internal::MatOps<T, R, C, B>::Inverse(*this);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    T Mat<T, R, C, B>::Determinant() const
    {
        return Internal::MatOps<T, R, C, B>::Determinant(*this);
    }

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    requires ValidMatDims<R, C>
    const Mat<T, R, C, B> MatConsts<T, R, C, B>::zero = Mat<T, R, C, B>(0);

    template <typename T, uint8_t L, MathBackend B>
    requires ValidVecDim<L>
    const Mat<T, L, L, B> MatConsts<T, L, L, B>::zero = Mat<T, L, L, B>(0);

    template <typename T, uint8_t L, MathBackend B>
    requires ValidVecDim<L>
    const Mat<T, L, L, B> MatConsts<T, L, L, B>::identity = Internal::GetIdentityMatrix<T, L, B>();

    template <typename T, uint8_t R, uint8_t C, MathBackend B>
    Vec<T, R, B> operator*(const Mat<T, R, C, B>& mat, const Vec<T, C, B>& vec) {
        return Internal::MatOps<T, R, C, B>::MulVec(mat, vec);
    }
}
