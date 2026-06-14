//
// Created by Zach Lee on 2022/9/12.
//

#include <Test/Test.h>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>
#include <Common/Math/Transform.h>
#include <Common/Math/Rect.h>
#include <Common/Math/Box.h>
#include <Common/Math/Sphere.h>
#include <Common/Math/Color.h>
#include <Common/Math/View.h>
#include <Common/Math/Half.h>
#include <Common/Math/Projection.h>
#include <SerializationTest.h>

using namespace Common;

// ==================================== Common ====================================

TEST(MathTest, CommonTest)
{
    ASSERT_EQ(DivideAndRoundUp(7, 3), 3);
    ASSERT_EQ(DivideAndRoundUp(6, 2), 3);
    ASSERT_EQ(DivideAndRoundUp(70, 9), 8);
}

TEST(MathTest, CompareNumberTest)
{
    ASSERT_TRUE(CompareNumber(1.0f, 1.0f + epsilon / 2.0f));
    ASSERT_FALSE(CompareNumber(1.0f, 1.1f));
    ASSERT_TRUE(CompareNumber(5, 5));
    ASSERT_FALSE(CompareNumber(5, 6));
}

// ==================================== Half ====================================

TEST(MathTest, HFloatTest) // NOLINT
{
    const HFloat v0 = 1.0f;
    ASSERT_FLOAT_EQ(v0, 1.0f);
    ASSERT_TRUE(v0 == 1.0f);
    ASSERT_TRUE(v0 != 2.0f);

    const HFloat v1 = v0 + 2.0f;
    ASSERT_FLOAT_EQ(v1, 3.0f);

    const HFloat v2 = v1 / 2.0f;
    ASSERT_FLOAT_EQ(v2.AsFloat(), 1.5f);

    HFloat v3 = 1.0f;
    v3 /= v2;
    ASSERT_TRUE(v3 == (1.0f / 1.5f));

    HFloat v4 = 2.0f;
    v4 -= v2;
    ASSERT_TRUE(v4 == HFloat(0.5f));

    const HFloat v5 = 3.0f;
    ASSERT_TRUE(HFloat(9.0f) = HFloat(std::pow(v5, 2.0f)));

    const HFloat v6 = 9.0f;
    ASSERT_TRUE(v5 == HFloat(std::sqrt(v6)));
}

TEST(MathTest, HFloatComparisonTest) // NOLINT
{
    const HFloat a = 1.0f;
    const HFloat b = 2.0f;
    ASSERT_TRUE(a < b);
    ASSERT_TRUE(b > a);
    ASSERT_TRUE(a <= HFloat(1.0f));
    ASSERT_TRUE(a >= HFloat(1.0f));
    ASSERT_FALSE(a > b);
    ASSERT_TRUE(a != b);

    ASSERT_TRUE((HFloat(1.0f) + HFloat(2.0f)) == 3.0f);
    ASSERT_TRUE((HFloat(6.0f) - HFloat(2.0f)) == 4.0f);
    ASSERT_TRUE((HFloat(2.0f) * HFloat(3.0f)) == 6.0f);
    ASSERT_TRUE((HFloat(6.0f) / HFloat(2.0f)) == 3.0f);

    HFloat c = 1.0f;
    c += HFloat(2.0f);
    c -= 1.0f;
    c *= HFloat(3.0f);
    ASSERT_TRUE(c == 6.0f);
    c /= 2.0f;
    ASSERT_TRUE(c == 3.0f);
}

TEST(MathTest, HFloatEdgeCaseTest) // NOLINT
{
    ASSERT_TRUE(HFloat(0.0f) == 0.0f);
    ASSERT_TRUE(HFloat(-2.5f) == -2.5f);
    ASSERT_TRUE(HFloat(-2.5f) < HFloat(0.0f));
    ASSERT_TRUE(HFloat(0.5f) == 0.5f);
    ASSERT_TRUE(HFloat(0.25f) == 0.25f);

    // values beyond the half range are clamped to the largest representable finite half (~65504)
    const float large = static_cast<float>(HFloat(100000.0f));
    ASSERT_GT(large, 60000.0f);
    ASSERT_LT(large, 70000.0f);
}

// ==================================== Vector ====================================

TEST(MathTest, FVec1Test)
{
    const FVec1 v0;
    ASSERT_FLOAT_EQ(v0.x, 0.0f);

    const FVec1 v1(1);
    ASSERT_FLOAT_EQ(v1.x, 1.0f);
    ASSERT_TRUE(v1 == FVec1(1.0f));

    const FVec1 v2 = v0 + v1;
    ASSERT_FLOAT_EQ(v2.x, 1.0f);

    const FVec1 v3 = v2 * 4 - 1;
    ASSERT_FLOAT_EQ(v3.x, 3.0f);

    const FVec1 v4 = (v3 - v2) * v2;
    ASSERT_FLOAT_EQ(v4.x, 2.0f);

    const FVec1 v5 = FVec1(6.0f) / v4;
    ASSERT_FLOAT_EQ(v5.x, 3.0f);
}

TEST(MathTest, FVec2Test)
{
    const FVec2 v0;
    ASSERT_FLOAT_EQ(v0.x, 0.0f);
    ASSERT_FLOAT_EQ(v0.y, 0.0f);

    const FVec2 v1(3.0f, 4.0f);
    ASSERT_FLOAT_EQ(v1[0], 3.0f);
    ASSERT_FLOAT_EQ(v1[1], 4.0f);

    const FVec2 v2 = v0 + v1 + 1;
    ASSERT_FLOAT_EQ(v2.x, 4.0f);
    ASSERT_FLOAT_EQ(v2.y, 5.0f);

    const FVec2 v3 = v2 / 2.0f;
    ASSERT_TRUE(v3 == FVec2(2.0f, 2.5f));

    const FVec2 v4 = v3 / FVec2(2.0f, 5.0f);
    ASSERT_FLOAT_EQ(v4.x, 1.0f);
    ASSERT_FLOAT_EQ(v4.y, 0.5f);
}

TEST(MathTest, FVec3Test)
{
    const FVec3 v0(4, 5, 6);
    ASSERT_FLOAT_EQ(v0.x, 4.0f);
    ASSERT_FLOAT_EQ(v0.y, 5.0f);
    ASSERT_FLOAT_EQ(v0.z, 6.0f);

    const FVec3 v1 = v0 * 2.0f - FVec3(1, 2, 3);
    ASSERT_FLOAT_EQ(v1.x, 7.0f);
    ASSERT_FLOAT_EQ(v1.y, 8.0f);
    ASSERT_FLOAT_EQ(v1.z, 9.0f);

    const FVec3 v2 = v1 / FVec3(2, 2, 2);
    ASSERT_TRUE(v2 == FVec3(3.5f, 4.0f, 4.5f));
}

TEST(MathTest, FVec4Test)
{
    const auto v0 = FVec4(4, 5, 6, 7);
    ASSERT_FLOAT_EQ(v0.x, 4.0f);
    ASSERT_FLOAT_EQ(v0.y, 5.0f);
    ASSERT_FLOAT_EQ(v0.z, 6.0f);
    ASSERT_FLOAT_EQ(v0.w, 7.0f);

    const FVec4 v1 = v0 * 3.0f + FVec4(1, 1, 2, 2) - FVec4(4, 4, 3, 3);
    ASSERT_FLOAT_EQ(v1.x, 9.0f);
    ASSERT_FLOAT_EQ(v1.y, 12.0f);
    ASSERT_FLOAT_EQ(v1.z, 17.0f);
    ASSERT_FLOAT_EQ(v1.w, 20.0f);
}

TEST(MathTest, IVec1Test)
{
    const IVec1 v0 = 1;
    ASSERT_EQ(v0.x, 1);

    const IVec1 v1 = (v0 + 3) / 2;
    ASSERT_EQ(v1.x, 2);

    const IVec1 v2 = v1 - IVec1(3);
    ASSERT_EQ(v2.x, -1);
}

TEST(MathTest, IVec2Test)
{
    const IVec2 v0(-1, -2);
    ASSERT_EQ(v0.x, -1);
    ASSERT_EQ(v0.y, -2);

    const IVec2 v1 { 3, 4 };
    ASSERT_EQ(v1.x, 3);
    ASSERT_EQ(v1.y, 4);

    const IVec2 v2 = v0 + v1 - 1;
    ASSERT_TRUE(v2 == IVec2(1, 1));

    const IVec2 v3 = v2 * 3 / IVec2(2);
    ASSERT_EQ(v3.x, 1);
    ASSERT_EQ(v3.y, 1);
}

TEST(MathTest, IVec3Test)
{
    const auto v0 = IVec3(3, 4, 5);
    ASSERT_TRUE(v0 == IVec3(3, 4, 5));
    ASSERT_TRUE(v0 != IVec3(1));

    IVec3 v1 = 1;
    v1 += v0;
    ASSERT_TRUE(v1 == IVec3(4, 5, 6));

    IVec3 v2 = v1 * 3;
    v2 /= 4;
    ASSERT_EQ(v2.x, 3);
    ASSERT_EQ(v2.y, 3);
    ASSERT_EQ(v2.z, 4);
}

TEST(MathTest, IVec4Test)
{
    const auto v0 = IVec4(-1, -2, -3, -4);
    ASSERT_EQ(v0.x, -1);
    ASSERT_EQ(v0.y, -2);
    ASSERT_EQ(v0.z, -3);
    ASSERT_EQ(v0.w, -4);

    IVec4 v1;
    v1 += v0 * 2;
    ASSERT_TRUE(v1 == IVec4(-2, -4, -6, -8));

    const IVec4 v2 = (v1 + 10) / IVec4(1, 2, 3, 4);
    ASSERT_TRUE(v2 == IVec4(8, 3, 1, 0));
}

TEST(MathTest, HVec1Test)
{
    const HVec1 v0 = 1.0f;
    ASSERT_FLOAT_EQ(v0.x, 1.0f);

    HVec1 v1;
    v1 += v0;
    v1 /= HFloat(2.0f);
    v1 -= HFloat(3.0f);
    ASSERT_TRUE(v1.x == HFloat(-2.5f));
}

TEST(MathTest, HVec2Test)
{
    const auto v0 = HVec2(1.0f, 2.0f);
    ASSERT_TRUE(v0.x == HFloat(1.0f));
    ASSERT_TRUE(v0.y == 2.0f);

    HVec2 v1 = v0 + HFloat(2.0f) - HVec2(3.0f, 1.0f);
    ASSERT_TRUE(v1.x == HFloat(0.0f));
    ASSERT_TRUE(v1.y == 3.0f);

    v1 /= HFloat(2.0f);
    ASSERT_TRUE(v1.x == 0.0f);
    ASSERT_TRUE(v1.y == HFloat(1.5f));
}

TEST(MathTest, HVec3Test)
{
    const HVec3 v0 { HFloat(1.0f), HFloat(2.0f), HFloat(3.0f) };
    ASSERT_TRUE(v0.x == 1.0f);
    ASSERT_TRUE(v0.y == HFloat(2.0f));
    ASSERT_TRUE(v0.z == 3.0f);
    ASSERT_FALSE(v0.z == 2.0f);

    const HVec3 v1 = v0 + HFloat(3.0f) / HFloat(2.0f) - HFloat(1.0f);
    ASSERT_TRUE(v1 == HVec3(1.5f, 2.5f, 3.5f));
}

TEST(MathTest, HVec4Test)
{
    const HVec4 v0 = 2.0f;
    ASSERT_FALSE(v0.x == 3.0f);
    ASSERT_TRUE(v0.w == 2.0f);

    const HVec4 v1 = v0 - HVec4(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_TRUE(v1 == HVec4(1.0f, 0.0f, -1.0f, -2.0f));
}

TEST(MathTest, VecScalarCompareTest)
{
    const FVec3 v0(2.0f);
    ASSERT_TRUE(v0 == 2.0f);
    ASSERT_TRUE(v0 != 3.0f);
    ASSERT_FALSE(FVec3(1, 2, 3) == 1.0f);
}

TEST(MathTest, VecCastToTest)
{
    const FVec3 v0(1.5f, 2.5f, 3.5f);
    const IVec3 v1 = v0.CastTo<int32_t>();
    ASSERT_TRUE(v1 == IVec3(1, 2, 3));

    const IVec2 v2(1, 2);
    const FVec2 v3 = v2.CastTo<float>();
    ASSERT_TRUE(v3 == FVec2(1.0f, 2.0f));
}

TEST(MathTest, SubVecTest)
{
    const auto v0 = FVec4(1, 2, 3, 4);
    const FVec3 v1 = v0.SubVec<0, 1, 2>();
    ASSERT_TRUE(v1 == FVec3(1, 2, 3));

    const FVec2 v2 = v0.SubVec<1, 3>();
    ASSERT_TRUE(v2 == FVec2(2, 4));
}

TEST(MathTest, VecModelTest)
{
    const auto v0 = FVec4(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_FLOAT_EQ(v0.Model(), std::sqrt(30.0f));

    const FVec2 v1 { 2.0f, 3.0f };
    ASSERT_FLOAT_EQ(v1.Model(), std::sqrt(13.0f));
}

TEST(MathTest, VecDotTest)
{
    const auto v0 = FVec4(1.0f, 2.0f, 3.0f, 4.0f);
    const auto v1 = FVec4(2.0f, 3.0f, 4.0f, 5.0f);
    ASSERT_FLOAT_EQ(v0.Dot(v1), 40.0f);

    const FVec2 v2 { 2.0f, 3.0f };
    const FVec2 v3 { 3.0f, 4.0f };
    ASSERT_FLOAT_EQ(v2.Dot(v3), 18.0f);
}

TEST(MathTest, VecCrossTest)
{
    const FVec2 v0 { 1.0f, 2.0f };
    const FVec2 v1 { 3.0f, 4.0f };
    ASSERT_FLOAT_EQ(v0.Cross(v1), -2.0f);

    const auto v2 = FVec3(1.0f, 2.0f, 3.0f);
    const auto v3 = FVec3(2.0f, 3.0f, 4.0f);
    const FVec3 v4 = v2.Cross(v3);
    ASSERT_FLOAT_EQ(v4.x, -1.0f);
    ASSERT_FLOAT_EQ(v4.y, 2.0f);
    ASSERT_FLOAT_EQ(v4.z, -1.0f);
}

TEST(MathTest, VecNormalizeTest)
{
    FVec2 v0 { 3.0f, 4.0f };
    const FVec2 n0 { 0.6f, 0.8f };
    ASSERT_TRUE(v0.Normalized() == n0);

    v0.Normalize();
    ASSERT_TRUE(v0 == n0);
}

TEST(MathTest, VecConstsTest)
{
    ASSERT_TRUE(FVec2(1, 0) == FVec2Consts::unitX);
    ASSERT_TRUE(IVec3(0, 0, 1) == IVec3Consts::unitZ);
}

TEST(MathTest, VecConstsFullTest)
{
    ASSERT_TRUE(FVec4Consts::negaUnitZ == FVec4(0, 0, -1, 0));
    ASSERT_TRUE(FVec4Consts::unitW == FVec4(0, 0, 0, 1));
    ASSERT_TRUE(FVec4Consts::negaUnit == FVec4(-1, -1, -1, -1));
    ASSERT_TRUE(FVec3Consts::unit == FVec3(1, 1, 1));
    ASSERT_TRUE(FVec2Consts::negaUnitX == FVec2(-1, 0));
    ASSERT_TRUE(IVec1Consts::negaUnit == IVec1(-1));
    ASSERT_TRUE(IVec1Consts::unit == IVec1(1));
}

// ==================================== Matrix ====================================

TEST(MathTest, IMat2x3Test)
{
    IMat2x3 v0(1, 2, 3, 4, 5, 6);
    for (auto i = 0; i < 6; i++) {
        ASSERT_EQ(v0[i], i + 1);
        ASSERT_EQ(v0.At(i / 3, i % 3), i + 1);
    }

    IMat2x3 v1 = (v0 + 3 - 2) / 2;
    for (auto i = 0; i < 6; i++) {
        ASSERT_EQ(v1.At(i / 3, i % 3), (i + 2) / 2);
    }
}

TEST(MathTest, FMat3x4Test)
{
    FMat3x4 v0(
        FVec4(1, 2, 3, 4),
        FVec4(5, 6, 7, 8),
        FVec4(9, 10, 11, 12)
    );
    for (auto i = 0; i < 12; i++) {
        ASSERT_EQ(v0.At(i / 4, i % 4), i + 1);
    }

    const FMat3x4 v1 = (v0 - 3.0f) * 4.0f;
    FMat3x4 v2;
    v2.SetRows(
        FVec4(-8.0f, -4.0f, 0.0f, 4.0f),
        FVec4(8.0f, 12.0f, 16.0f, 20.0f),
        FVec4(24.0f, 28.0f, 32.0f, 36.0f)
    );
    ASSERT_TRUE(v1 == v2);
}

TEST(MathTest, SubMatrixTest)
{
    const FMat4x4 m0(
        FVec4(1, 2, 3, 4),
        FVec4(4, 5, 6, 7),
        FVec4(7, 8, 9, 10),
        FVec4(12, 11, 9, 10)
    );

    const auto m1 = m0.SubMatrix<3, 3>();
    ASSERT_TRUE(m1.Row(0) == FVec3(1, 2, 3));
    ASSERT_TRUE(m1.Row(1) == FVec3(4, 5, 6));
    ASSERT_TRUE(m1.Row(2) == FVec3(7, 8, 9));
}

TEST(MathTest, MatViewTest)
{
    const FMat3x3 v0(
        FVec3(1, 2, 3),
        FVec3(4, 5, 6),
        FVec3(7, 8, 9)
    );
    ASSERT_TRUE(v0.Row(0) == FVec3(1, 2, 3));
    ASSERT_TRUE(v0.Row(1) == FVec3(4, 5, 6));
    ASSERT_TRUE(v0.Row(2) == FVec3(7, 8, 9));
    ASSERT_TRUE(v0.Col(0) == FVec3(1, 4, 7));
    ASSERT_TRUE(v0.Col(1) == FVec3(2, 5, 8));
    ASSERT_TRUE(v0.Col(2) == FVec3(3, 6, 9));
}

TEST(MathTest, MatConstsTest)
{
    const auto v0 = FMat2x4Consts::zero;
    ASSERT_TRUE(v0.Row(1) == FVec4Consts::zero);

    const auto v1 = FMat3x3Consts::zero;
    ASSERT_TRUE(v1.Col(2) == FVec3Consts::zero);

    const auto v2 = FMat3x3Consts::identity;
    ASSERT_TRUE(v2.Row(0) == FVec3(1, 0, 0));
    ASSERT_TRUE(v2.Row(1) == FVec3(0, 1, 0));
    ASSERT_TRUE(v2.Row(2) == FVec3(0, 0, 1));
}

TEST(MathTest, MatSetTest) // NOLINT
{
    FMat2x4 v0;
    v0.SetRow(0, 1, 2, 3, 4);
    v0.SetCol(0, FVec2(2, 3));
    ASSERT_TRUE(v0.Row(0) == FVec4(2, 2, 3, 4));

    v0.SetRows(
        FVec4(5, 6, 7, 8),
        FVec4(9, 10, 11, 12)
    );
    ASSERT_TRUE(v0.Row(0) == FVec4(5, 6, 7, 8));
    ASSERT_TRUE(v0.Row(1) == FVec4(9, 10, 11, 12));

    v0.SetCols(
        FVec2(1, 2),
        FVec2(3, 4),
        FVec2(5, 6),
        FVec2(7, 8)
    );
    ASSERT_TRUE(v0.Row(0) == FVec4(1, 3, 5, 7));
    ASSERT_TRUE(v0.Row(1) == FVec4(2, 4, 6, 8));

    v0.SetValues(
        2, 3, 4, 5,
        6, 7, 8, 9
    );
    ASSERT_TRUE(v0.Row(0) == FVec4(2, 3, 4, 5));
    ASSERT_TRUE(v0.Row(1) == FVec4(6, 7, 8, 9));
}

TEST(MathTest, MatFromVecsTest)
{
    const FMat2x3 m0 = FMat2x3::FromRowVecs(FVec3(1, 2, 3), FVec3(4, 5, 6));
    ASSERT_TRUE(m0.Row(0) == FVec3(1, 2, 3));
    ASSERT_TRUE(m0.Row(1) == FVec3(4, 5, 6));

    const FMat2x3 m1 = FMat2x3::FromColVecs(FVec2(1, 4), FVec2(2, 5), FVec2(3, 6));
    ASSERT_TRUE(m1.Col(0) == FVec2(1, 4));
    ASSERT_TRUE(m1 == m0);
}

TEST(MathTest, MatCastToTest)
{
    const FMat2x2 m0(1.5f, 2.5f, 3.5f, 4.5f);
    const IMat2x2 m1 = m0.CastTo<int32_t>();
    ASSERT_EQ(m1[0], 1);
    ASSERT_EQ(m1[1], 2);
    ASSERT_EQ(m1[2], 3);
    ASSERT_EQ(m1[3], 4);
}

TEST(MathTest, MatMulTest)
{
    const FMat3x4 m0(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    );
    const FMat4x2 m1(
        1, 2,
        3, 4,
        5, 6,
        7, 8
    );
    const FVec4 v0(1, 2, 3, 4);
    const FVec2 v1(1, 2);

    const auto v2 = m0 * v0;
    const auto v3 = m1 * v1;
    const auto m2 = m0 * m1;

    ASSERT_TRUE(m2.Row(0) == FVec2(50, 60));
    ASSERT_TRUE(m2.Row(1) == FVec2(114, 140));
    ASSERT_TRUE(m2.Row(2) == FVec2(178, 220));
    ASSERT_TRUE(v2 == FVec3(30, 70, 110));
    ASSERT_TRUE(v3 == FVec4(5, 11, 17, 23));
}

TEST(MathTest, MathTranposeTest)
{
    const FMat3x4 v0(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    );
    const FMat4x3 v1 = v0.Transpose();
    ASSERT_TRUE(v1.Row(0) == FVec3(1, 5, 9));
    ASSERT_TRUE(v1.Row(1) == FVec3(2, 6, 10));
    ASSERT_TRUE(v1.Row(2) == FVec3(3, 7, 11));
    ASSERT_TRUE(v1.Row(3) == FVec3(4, 8, 12));
}

TEST(MathTest, MatrixDetInverseTest)
{
    const FMat2x2 m0(
        5, 7,
        3, 4
    );

    const FMat3x3 m1(
        5, 1, 7,
        4, 2, 6,
        3, 5, 1
    );

    const FMat4x4 m2(
        5, 1, 7, 2,
        4, 2, 6, 5,
        3, 5, 1, 8,
        1, 2, 3, 4
    );

    const FMat2x2 im0 = m0.Inverse();
    const FMat3x3 im1 = m1.Inverse();
    const FMat4x4 im2 = m2.Inverse();

    const FMat2x2 inverse0(
        -4, 7,
        3, -5
    );

    const FVec3 col1 { 1.f, -.5f, -.5f };
    const FVec4 col2 { 1.f / 76.f, 16.f / 19.f, 13.f / 76.f, -21.f / 38.f};

    ASSERT_TRUE(m0.Determinant() == -1.0f);
    ASSERT_TRUE(m1.Determinant() == -28.0f);
    ASSERT_TRUE(m2.Determinant() == 76.0f);
    ASSERT_TRUE(im0 == inverse0);
    ASSERT_TRUE(im1.Col(0) == col1);
    ASSERT_TRUE(im2.Col(0) == col2);
}

TEST(MathTest, MatCanInverseTest)
{
    const FMat2x2 m0(1, 2, 3, 4);
    ASSERT_TRUE(m0.CanInverse());

    const FMat2x2 m1(1, 2, 2, 4);
    ASSERT_FALSE(m1.CanInverse());
}

TEST(MathTest, MatScalarArithmeticTest)
{
    FMat2x2 m0(1, 2, 3, 4);
    m0 += 1.0f;
    ASSERT_TRUE(m0 == FMat2x2(2, 3, 4, 5));
    m0 -= 1.0f;
    ASSERT_TRUE(m0 == FMat2x2(1, 2, 3, 4));
    m0 *= 2.0f;
    ASSERT_TRUE(m0 == FMat2x2(2, 4, 6, 8));
    m0 /= 2.0f;
    ASSERT_TRUE(m0 == FMat2x2(1, 2, 3, 4));

    const FMat2x2 a(1, 2, 3, 4);
    const FMat2x2 b(4, 3, 2, 1);
    ASSERT_TRUE(a != b);
    ASSERT_TRUE(a != 0.0f);
    ASSERT_TRUE((a + b) == FMat2x2(5, 5, 5, 5));
    ASSERT_TRUE((b - a) == FMat2x2(3, 1, -1, -3));
    ASSERT_TRUE((a * 2.0f) == FMat2x2(2, 4, 6, 8));
    ASSERT_TRUE((a / 2.0f) == FMat2x2(0.5f, 1.0f, 1.5f, 2.0f));
}

TEST(MathTest, MatExtractionTest)
{
    const FMat4x4 m = {
        0, -2, 0, 7,
        4, 0, 0, 5,
        0, 0, 3, 3,
        7, 5, 3, 1
    };

    const FTransform trans = FTransform(m);

    ASSERT_TRUE(trans.translation == FVec3(7.0f, 5.0f, 3.0f));
    ASSERT_TRUE(trans.scale == FVec3(4.0f, 2.0f, 3.0f));
    ASSERT_TRUE(trans.rotation == FQuat(0.7071067f, .0f, .0f, .7071067f));
}

// ==================================== Quaternion ====================================

TEST(MathTest, AngleAndRadianTest)
{
    const FAngle v0(90.0f);
    ASSERT_TRUE(v0.ToRadian() == pi / 2.0f);

    const FRadian v1(pi / 4.0f);
    ASSERT_TRUE(v1.ToAngle() == 45.0f);
}

TEST(MathTest, AngleRadianConvertTest)
{
    const FAngle a0(90.0f);
    const FRadian r0(a0);
    ASSERT_TRUE(r0 == FRadian(pi / 2.0f));

    const FRadian r1(pi);
    const FAngle a1(r1);
    ASSERT_TRUE(a1 == FAngle(180.0f));

    ASSERT_TRUE(FAngle(45.0f) == FAngle(45.0f));
    ASSERT_FALSE(FRadian(1.0f) == FRadian(2.0f));
}

TEST(MathTest, QuaternionBasicTest)
{
    const FQuat v0(1, 2, 3, 4);
    const FQuat v1(2, 3, 4, 5);
    const FQuat v2(1, 1, 2, 2);
    ASSERT_TRUE((v0 * 2 + v1 - v2) == FQuat(3, 6, 8, 11));
}

TEST(MathTest, QuaternionArithmeticTest)
{
    FQuat v0(1, 2, 3, 4);
    v0 += FQuat(1, 1, 1, 1);
    ASSERT_TRUE(v0 == FQuat(2, 3, 4, 5));
    v0 -= FQuat(1, 1, 1, 1);
    ASSERT_TRUE(v0 == FQuat(1, 2, 3, 4));
    v0 *= 2.0f;
    ASSERT_TRUE(v0 == FQuat(2, 4, 6, 8));
    v0 /= 2.0f;
    ASSERT_TRUE(v0 == FQuat(1, 2, 3, 4));

    ASSERT_TRUE((FQuat(2, 4, 6, 8) / 2.0f) == FQuat(1, 2, 3, 4));

    FQuat v1(1, 2, 3, 4);
    v1 *= FQuatConsts::identity;
    ASSERT_TRUE(v1 == FQuat(1, 2, 3, 4));

    const FQuat r0(FVec3Consts::unitZ, 90);
    const FQuat r1(FVec3Consts::unitY, 45);
    FQuat r2 = r0;
    r2 *= r1;
    ASSERT_TRUE(r2 == (r0 * r1));
}

TEST(MathTest, QuaternionPropertiesTest)
{
    const FQuat v0(1, 2, 3, 4);
    ASSERT_TRUE(v0.ImaginaryPart() == FVec3(2, 3, 4));
    ASSERT_FLOAT_EQ(v0.Model(), std::sqrt(30.0f));
    ASSERT_TRUE(v0.Negatived() == FQuat(-1, -2, -3, -4));
    ASSERT_TRUE(v0.Conjugated() == FQuat(1, -2, -3, -4));

    const FQuat v1(2, 0, 0, 0);
    ASSERT_TRUE(v1.Normalized() == FQuat(1, 0, 0, 0));

    const FQuat v2(2, 3, 4, 5);
    ASSERT_FLOAT_EQ(v0.Dot(v2), 1.0f * 2 + 2 * 3 + 3 * 4 + 4 * 5);
}

TEST(MathTest, QuaternionRotationTest)
{
    const FQuat v0(FVec3(0, 0, 1), 90);
    const FVec3 v0r0 = v0.RotateVector(FVec3(1, 0, 0));
    ASSERT_TRUE(v0r0 == FVec3(0, -1, 0));
    const FVec3 v0r1 = v0.RotateVector(FVec3(1, 1, 1));
    ASSERT_TRUE(v0r1 == FVec3(1, -1, 1));

    const FQuat v1(FVec3(1, 0, 0), 90);
    const FVec3 v1r0 = v1.RotateVector(FVec3(0, 1, 0));
    ASSERT_TRUE(v1r0 == FVec3(0, 0, -1));
}

TEST(MathTest, QuaternionRadianTest)
{
    const FQuat v0(FVec3Consts::unitZ, FRadian(pi / 2.0f));
    const FQuat v1(FVec3Consts::unitZ, 90.0f);
    ASSERT_TRUE(v0 == v1);

    const FQuat v2 = FQuat::FromEulerZYX(FRadian(0.0f), FRadian(0.0f), FRadian(pi / 2.0f));
    const FQuat v3 = FQuat::FromEulerZYX(0.0f, 0.0f, 90.0f);
    ASSERT_TRUE(v2 == v3);
}

TEST(MathTest, EulerRotationTest)
{
    const FQuat v0 = FQuat::FromEulerZYX(0, 0, 90);
    const FVec3 v0r0 = v0.RotateVector(FVec3(1, 0, 0));
    ASSERT_TRUE(v0r0 == FVec3(0, -1, 0));

    const FQuat v1 = FQuat::FromEulerZYX(90, 0, 90);
    const FVec3 v1r0 = v1.RotateVector(FVec3(1, 0, 0));
    ASSERT_TRUE(v1r0 == FVec3(0, 0, 1));

    ASSERT_TRUE(FQuatConsts::identity == FQuat::FromEulerZYX(0, 0, 0));
}

TEST(MathTest, QuaternionToRotationMatrixTest)
{
    auto applyRotationMatrix = [](const FMat4x4& rotationMatrix, const FVec3& vec) -> FVec3 {
        return (rotationMatrix * FMat4x1::FromColVecs(FVec4(vec.x, vec.y, vec.z, 0))).Col(0).SubVec<0, 1, 2>();
    };

    const FMat4x4 v0 = FQuat(FVec3Consts::unitZ, 90).GetRotationMatrix();
    const FVec3 v0r0 = applyRotationMatrix(v0, FVec3(1, 0, 0));
    ASSERT_TRUE(v0r0 == FVec3(0, -1, 0));

    const FMat4x4 v1 = (FQuat(FVec3Consts::unitZ, 90) * FQuat(FVec3Consts::unitY, 180)).GetRotationMatrix();
    const FVec3 v1r0 = applyRotationMatrix(v1, FVec3(0, 1, 0));
    ASSERT_TRUE(v1r0 == FVec3(-1, 0, 0));
}

TEST(MathTest, QuatConstsTest)
{
    ASSERT_TRUE(FQuatConsts::zero == FQuat(0, 0, 0, 0));
    ASSERT_TRUE(FQuatConsts::identity == FQuat(1, 0, 0, 0));
}

TEST(MathTest, QuaternionCastToTest)
{
    const FQuat v0(1.0f, 2.0f, 3.0f, 4.0f);
    const DQuat v1 = v0.CastTo<double>();
    ASSERT_TRUE(v1 == DQuat(1.0, 2.0, 3.0, 4.0));
}

// ==================================== Transform ====================================

TEST(MathTest, TransformTest)
{
    FVec3 x(1, 0, 0);
    FVec3 y(0, 1, 0);
    FVec3 z(0, 0, 1);

    FTransform v0(FVec3(2, 2, 2), FQuat(FVec3Consts::unitZ, 90), FVec3(5, 0, 0));
    FVec3 v0r0 = v0.TransformPosition(x);
    ASSERT_TRUE(v0r0 == FVec3(5, -2, 0));

    FTransform v1(FQuat::FromEulerZYX(90, 0, 90), FVec3(0, 0, 0));
    FTransform v2(FQuat::FromEulerZYX(0, 90, 90), FVec3(0, 0, 0));
    FTransform v3(FQuat::FromEulerZYX(90, 90, 0), FVec3(0, 0, 0));
    FVec3 v1x = v1.TransformPosition(x);
    FVec3 v2y = v2.TransformPosition(y);
    FVec3 v3x = v3.TransformPosition(x);
    FVec3 v3z = v3.TransformPosition(z);
    ASSERT_TRUE(v1x == FVec3(0, 0, 1));
    ASSERT_TRUE(v2y == FVec3(0, 0, 1));
    ASSERT_TRUE(v3x == FVec3(0, 1, 0));
    ASSERT_TRUE(v3z == FVec3(-1, 0, 0));
    // In general, quaternion from eular angle performs z-axis rotation first, then y-axis, and last z-axis

}

TEST(MathTest, TransformOperatorsTest)
{
    const FTransform base;
    const FQuat rot(FVec3Consts::unitZ, 90);

    const FTransform v0 = base + FVec3(1, 2, 3);
    ASSERT_TRUE(v0.translation == FVec3(1, 2, 3));
    ASSERT_TRUE(v0.scale == FVec3(1, 1, 1));

    const FTransform v1 = base * FVec3(2, 3, 4);
    ASSERT_TRUE(v1.scale == FVec3(2, 3, 4));

    const FTransform v2 = base | rot;
    ASSERT_TRUE(v2.rotation == rot);

    FTransform v3 = base;
    v3 += FVec3(1, 1, 1);
    v3 *= FVec3(2, 2, 2);
    v3 |= rot;
    ASSERT_TRUE(v3.translation == FVec3(1, 1, 1));
    ASSERT_TRUE(v3.scale == FVec3(2, 2, 2));
    ASSERT_TRUE(v3.rotation == rot);

    FTransform v4 = base;
    v4.Translate(FVec3(5, 0, 0)).Scale(FVec3(2, 2, 2));
    ASSERT_TRUE(v4.translation == FVec3(5, 0, 0));
    ASSERT_TRUE(v4.scale == FVec3(2, 2, 2));
}

TEST(MathTest, TransformMatrixTest)
{
    const FTransform v0(FVec3(2, 3, 4), FQuatConsts::identity, FVec3(5, 6, 7));

    const FMat4x4 tm = v0.GetTranslationMatrix();
    ASSERT_TRUE(tm.Col(3) == FVec4(5, 6, 7, 1));

    const FMat4x4 sm = v0.GetScaleMatrix();
    ASSERT_FLOAT_EQ(sm.At(0, 0), 2.0f);
    ASSERT_FLOAT_EQ(sm.At(1, 1), 3.0f);
    ASSERT_FLOAT_EQ(sm.At(2, 2), 4.0f);

    ASSERT_TRUE(v0.GetRotationMatrix() == FMat4x4Consts::identity);

    const FVec3 p0 = v0.TransformPosition(FVec3(1, 1, 1));
    ASSERT_TRUE(p0 == FVec3(7, 9, 11));

    const FVec4 p1 = (v0.GetTransformMatrixNoScale() * FVec4(1, 1, 1, 1));
    const FVec3 p1Xyz = p1.SubVec<0, 1, 2>();
    ASSERT_TRUE(p1Xyz == FVec3(6, 7, 8));
}

TEST(MathTest, TransformPositionVec4Test)
{
    const FTransform v0(FVec3(2, 2, 2), FQuatConsts::identity, FVec3(1, 1, 1));
    const FVec4 p0 = v0.TransformPosition(FVec4(1, 1, 1, 1));
    ASSERT_TRUE(p0 == FVec4(3, 3, 3, 1));
}

TEST(MathTest, TransformLookAtTest)
{
    FTransform v0;
    v0.MoveAndLookTo(FVec3(1, 2, 3), FVec3(4, 2, 3));
    ASSERT_TRUE(v0.translation == FVec3(1, 2, 3));

    const FTransform v1 = FTransform::LookAt(FVec3(1, 2, 3), FVec3(4, 2, 3));
    ASSERT_TRUE(v0 == v1);

    FTransform v2(FQuatConsts::identity, FVec3(0, 0, 0));
    v2.LookTo(FVec3(1, 0, 0));
    ASSERT_TRUE(v2.translation == FVec3(0, 0, 0));
    ASSERT_TRUE(v2.rotation == FQuat(0.5f, 0.5f, 0.5f, 0.5f));
}

TEST(MathTest, TransformCastToTest)
{
    const FTransform v0(FVec3(2, 3, 4), FQuat(1, 0, 0, 0), FVec3(5, 6, 7));
    const DTransform v1 = v0.CastTo<double>();
    ASSERT_TRUE(v1.scale == DVec3(2, 3, 4));
    ASSERT_TRUE(v1.translation == DVec3(5, 6, 7));
    ASSERT_TRUE(v1.rotation == DQuat(1, 0, 0, 0));
}

// ==================================== Rect ====================================

TEST(MathTest, RectTest)
{
    const FRect rect0(0.0f, 0.0f, 2.0f, 1.0f);
    const FRect rect1(1.0f, 0.5f, 3.0f, 2.0f);
    const FRect rect2(0.0f, 2.0f, 2.0f, 3.0f);
    const FVec2 point(0.5f, 0.5f);

    ASSERT_TRUE(rect0.Inside(point));
    ASSERT_TRUE(rect0.Intersect(rect1));
    ASSERT_TRUE(!rect0.Intersect(rect2));
    ASSERT_TRUE(rect0.Distance(rect2) == 2.0f);
}

TEST(MathTest, RectGeometryTest)
{
    const FRect v0 = FRect::FromMinExtent(1.0f, 2.0f, 4.0f, 6.0f);
    ASSERT_TRUE(v0.min == FVec2(1, 2));
    ASSERT_TRUE(v0.max == FVec2(5, 8));
    ASSERT_TRUE(v0.Extent() == FVec2(4, 6));
    ASSERT_FLOAT_EQ(v0.ExtentX(), 4.0f);
    ASSERT_FLOAT_EQ(v0.ExtentY(), 6.0f);
    ASSERT_TRUE(v0.Center() == FVec2(3, 5));
    ASSERT_FLOAT_EQ(v0.CenterX(), 3.0f);
    ASSERT_FLOAT_EQ(v0.CenterY(), 5.0f);
    ASSERT_FLOAT_EQ(v0.Size(), std::sqrt(4.0f * 4 + 6 * 6) / 2.0f);

    const FRect v1 = FRect::FromMinExtent(FVec2(0, 0), FVec2(2, 2));
    ASSERT_TRUE(v1.max == FVec2(2, 2));

    const IRect v2 = v0.CastTo<int32_t>();
    ASSERT_TRUE(v2.min == IVec2(1, 2));
    ASSERT_TRUE(v2.max == IVec2(5, 8));
}

// ==================================== Box ====================================

TEST(MathTest, BoxTest)
{
    const FBox box0(FVec3(0.0f), FVec3(1.0f));
    const FBox box1(FVec3(0.5f), FVec3(1.5f));
    const FBox box2(FVec3(3.0f), FVec3(4.0f));
    const FVec3 point(1.0f);

    ASSERT_TRUE(box1.Inside(point));
    ASSERT_TRUE(box0.Intersect(box1));
    ASSERT_TRUE(!box0.Intersect(box2));
}

TEST(MathTest, BoxGeometryTest)
{
    const FBox v0 = FBox::FromMinExtent(1.0f, 2.0f, 3.0f, 2.0f, 4.0f, 6.0f);
    ASSERT_TRUE(v0.min == FVec3(1, 2, 3));
    ASSERT_TRUE(v0.max == FVec3(3, 6, 9));
    ASSERT_TRUE(v0.Extent() == FVec3(2, 4, 6));
    ASSERT_FLOAT_EQ(v0.ExtentX(), 2.0f);
    ASSERT_FLOAT_EQ(v0.ExtentY(), 4.0f);
    ASSERT_FLOAT_EQ(v0.ExtentZ(), 6.0f);
    ASSERT_TRUE(v0.Center() == FVec3(2, 4, 6));
    ASSERT_FLOAT_EQ(v0.CenterX(), 2.0f);
    ASSERT_FLOAT_EQ(v0.CenterY(), 4.0f);
    ASSERT_FLOAT_EQ(v0.CenterZ(), 6.0f);
    ASSERT_FLOAT_EQ(v0.Size(), std::sqrt(2.0f * 2 + 4 * 4 + 6 * 6) / 2.0f);

    const FBox v1(FVec3(0), FVec3(2));
    const FBox v2(FVec3(10), FVec3(12));
    ASSERT_FLOAT_EQ(v1.Distance(v2), std::sqrt(300.0f));

    const FBox v3 = FBox::FromMinExtent(FVec3(0), FVec3(5));
    ASSERT_TRUE(v3.max == FVec3(5));

    const IBox v4 = v0.CastTo<int32_t>();
    ASSERT_TRUE(v4.min == IVec3(1, 2, 3));
    ASSERT_TRUE(v4.max == IVec3(3, 6, 9));
}

// ==================================== Sphere ====================================

TEST(MathTest, SphereTest)
{
    const FSphere sphere0(FVec3(0.0f), 1.0f);
    const FSphere sphere1(FVec3(0.5f, 0.0f, 0.0f), 1.0f);
    const FSphere sphere2(FVec3(2.0f, 0.0f, 0.0f), 0.5f);
    const FVec3 point(0.1f, 0.2f, 0.3f);

    ASSERT_TRUE(sphere0.Inside(point));
    ASSERT_TRUE(sphere0.Intersect(sphere1));
    ASSERT_TRUE(!sphere0.Intersect(sphere2));
    ASSERT_TRUE(sphere0.Distance(sphere1) == 0.5f);
}

TEST(MathTest, SphereGeometryTest)
{
    const FSphere v0(FVec3(0), 1.0f);
    const FSphere v1(FVec3(3, 0, 0), 2.0f);
    ASSERT_FLOAT_EQ(v0.Distance(v1), 3.0f);
    ASSERT_FALSE(v0.Inside(FVec3(2, 0, 0)));
    ASSERT_TRUE(v0.Inside(FVec3(0.5f, 0, 0)));

    const FSphere v2(FVec3(1, 2, 3), 1.0f);
    const DSphere v3 = v2.CastTo<double>();
    ASSERT_TRUE(v3.center == DVec3(1, 2, 3));
    ASSERT_TRUE(CompareNumber(v3.radius, 1.0));
}

// ==================================== Color ====================================

TEST(MathTest, ColorConversionTest)
{
    const Color v0(255, 128, 0, 255);
    const LinearColor v1 = v0.ToLinearColor();
    ASSERT_TRUE(v1 == LinearColor(1.0f, 128.0f / 255.0f, 0.0f, 1.0f));
    ASSERT_TRUE(v1.ToColor() == v0);

    ASSERT_TRUE(Color(LinearColor(1.0f, 0.0f, 0.0f, 1.0f)) == Color(255, 0, 0, 255));
    ASSERT_TRUE(LinearColor(Color(0, 255, 0, 255)) == LinearColor(0.0f, 1.0f, 0.0f, 1.0f));

    ASSERT_EQ(Color(255, 128, 0, 255).ToHexString(), "0xff8000ff");
    ASSERT_EQ(Color(0, 0, 0, 0).ToHexString(), "0x00000000");

    ASSERT_TRUE(ColorConsts::white == Color(255, 255, 255, 255));
    ASSERT_TRUE(ColorConsts::black == Color(0, 0, 0, 255));
    ASSERT_TRUE(ColorConsts::red == Color(255, 0, 0, 255));
    ASSERT_TRUE(ColorConsts::green == Color(0, 255, 0, 255));
    ASSERT_TRUE(ColorConsts::blue == Color(0, 0, 255, 255));
    ASSERT_TRUE(LinearColorConsts::white == LinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    ASSERT_TRUE(LinearColorConsts::black == LinearColor(0.0f, 0.0f, 0.0f, 1.0f));
}

// ==================================== View ====================================

TEST(MathTest, ViewMatrixTest)
{
    const FViewTransform v0;
    const FMat4x4 vm0 = v0.GetViewMatrix();
    ASSERT_TRUE((vm0 * FVec4(2, 3, 5, 1)) == FVec4(3, 5, 2, 1));

    const FViewTransform v1 = FViewTransform::LookAt(FVec3(3, 4, 5), FVec3(0, 0, 0), FVec3Consts::unitZ);
    const FMat4x4 vm1 = v1.GetViewMatrix();

    const FVec4 camInView = vm1 * FVec4(3, 4, 5, 1);
    ASSERT_NEAR(camInView.x, 0.0f, 1e-4f);
    ASSERT_NEAR(camInView.y, 0.0f, 1e-4f);
    ASSERT_NEAR(camInView.z, 0.0f, 1e-4f);

    const FVec4 targetInView = vm1 * FVec4(0, 0, 0, 1);
    const FVec3 targetXyz = targetInView.SubVec<0, 1, 2>();
    ASSERT_NEAR(targetXyz.Model(), std::sqrt(50.0f), 1e-4f);
}

// ==================================== Projection ====================================

TEST(MathTest, OrthoProjectionMatrixTest)
{
    const FReversedZOrthoProjection v0(4.0f, 2.0f, 1.0f, 11.0f);
    const FMat4x4 m0 = v0.GetProjectionMatrix();
    ASSERT_NEAR((m0 * FVec4(0, 0, 1.0f, 1)).z, 1.0f, 1e-4f);
    ASSERT_NEAR((m0 * FVec4(0, 0, 11.0f, 1)).z, 0.0f, 1e-4f);
    ASSERT_NEAR((m0 * FVec4(2.0f, 0, 1.0f, 1)).x, 1.0f, 1e-4f);
    ASSERT_NEAR((m0 * FVec4(0, 1.0f, 1.0f, 1)).y, 1.0f, 1e-4f);

    const FReversedZOrthoProjection v1(4.0f, 2.0f, 1.0f);
    const FMat4x4 m1 = v1.GetProjectionMatrix();
    ASSERT_NEAR((m1 * FVec4(0, 0, 1.0f, 1)).z, 1.0f, 1e-4f);

    ASSERT_TRUE(v0 == FReversedZOrthoProjection(4.0f, 2.0f, 1.0f, 11.0f));
    ASSERT_FALSE(v0 == v1);
}

TEST(MathTest, PerspectiveProjectionMatrixTest)
{
    const FReversedZPerspectiveProjection v0(90.0f, 2.0f, 2.0f, 1.0f, 11.0f);
    const FMat4x4 m0 = v0.GetProjectionMatrix();
    ASSERT_NEAR(m0.At(1, 1), 1.0f, 1e-4f);

    const FVec4 nearClip = m0 * FVec4(0, 0, 1.0f, 1);
    ASSERT_NEAR(nearClip.z / nearClip.w, 1.0f, 1e-4f);
    const FVec4 farClip = m0 * FVec4(0, 0, 11.0f, 1);
    ASSERT_NEAR(farClip.z / farClip.w, 0.0f, 1e-4f);

    const FReversedZPerspectiveProjection v1(90.0f, 2.0f, 2.0f, 1.0f, std::nullopt);
    const FMat4x4 m1 = v1.GetProjectionMatrix();
    const FVec4 nearClipInf = m1 * FVec4(0, 0, 1.0f, 1);
    ASSERT_NEAR(nearClipInf.z / nearClipInf.w, 1.0f, 1e-4f);

    ASSERT_TRUE(v0 == FReversedZPerspectiveProjection(90.0f, 2.0f, 2.0f, 1.0f, 11.0f));
    ASSERT_FALSE(v0 == FReversedZPerspectiveProjection(60.0f, 2.0f, 2.0f, 1.0f, 11.0f));
}

// ==================================== Serialization / String / Json ====================================

TEST(MathTest, SerializationTest)
{
    // half
    PerformTypedSerializationTest(HFloat(1.0f));

    // vec
    PerformTypedSerializationTest(FVec1(1.0f));
    PerformTypedSerializationTest(HVec2(2.0f, 3.0f));
    PerformTypedSerializationTest(IVec3(1, 2, 3));
    PerformTypedSerializationTest(UVec4(1, 2, 3, 4));

    // mat
    PerformTypedSerializationTest(FMat1x1(1.0f));
    PerformTypedSerializationTest(HMat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f));
    PerformTypedSerializationTest(IMat3x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12));
    PerformTypedSerializationTest(FMat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f));

    // angle/radian
    PerformTypedSerializationTest(FAngle(67.0f));
    PerformTypedSerializationTest(FRadian(1.5f * pi));

    // quaternion
    PerformTypedSerializationTest(FQuat(1.0f, 0.1f, 0.2f, 0.5f));
    PerformTypedSerializationTest(DQuat(1.0f, 0.1f, 0.2f, 0.5f));

    // color
    PerformTypedSerializationTest(Color(1, 2, 3, 1));
    PerformTypedSerializationTest(LinearColor(1.0f, 0.5f, 0.2f, 1.0f));

    // rect
    PerformTypedSerializationTest(FRect(1.0f, 2.0f, 3.0f, 4.0f));

    // box
    PerformTypedSerializationTest(FBox(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f));

    // sphere
    PerformTypedSerializationTest(FSphere(1.0f, 2.0f, 3.0f, 1.0f));

    // transform
    const FTransform transform(FVec3(2.0f, 2.0f, 2.0f), FQuat::FromEulerZYX(90.0f, 0, 0), FVec3(1.0f, 2.0f, 3.0f));
    PerformTypedSerializationTest(transform);

    // view transform
    PerformTypedSerializationTest(FViewTransform(transform));

    // projection
    PerformTypedSerializationTest(FReversedZOrthoProjection(512.0f, 1024.0f, 1.0f));
    PerformTypedSerializationTest(FReversedZPerspectiveProjection(90.0f, 512.0f, 1024.0f, 1.0f, 500.0f));
}

TEST(MathTest, ToStringTest)
{
    // half
    ASSERT_EQ(ToString(HFloat(1.0f)), std::to_string(1.0f));

    // vec
    ASSERT_EQ(ToString(UVec4(1, 2, 3, 4)), "(1, 2, 3, 4)");

    // mat
    ASSERT_EQ(
        ToString(HMat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)),
        std::format("({}, {}, {}, {}, {}, {})",
            std::to_string(2.0f), std::to_string(3.0f), std::to_string(4.0f),
            std::to_string(5.0f), std::to_string(6.0f), std::to_string(7.0f)));

    // angle/radian
    ASSERT_EQ(ToString(FAngle(75.0f)), std::format("a{}", std::to_string(75.0f)));
    ASSERT_EQ(ToString(FRadian(1.5f * pi)), std::to_string(1.5f * pi));

    // quaternion
    ASSERT_EQ(
        ToString(FQuat(1.0f, 0.1f, 0.2f, 0.5f)),
        std::format("({}, {}, {}, {})",
            std::to_string(1.0f), std::to_string(0.1f), std::to_string(0.2f), std::to_string(0.5f)));

    // color
    ASSERT_EQ(
        ToString(Color(1, 2, 3, 1)),
        std::format("{}r={}, g={}, b={}, a={}{}", "{", 1, 2, 3, 1, "}"));
    ASSERT_EQ(
        ToString(LinearColor(1.0f, 0.5f, 0.2f, 1.0f)),
        std::format("{}r={}, g={}, b={}, a={}{}",
            "{",
            std::to_string(1.0f), std::to_string(0.5f),
            std::to_string(0.2f), std::to_string(1.0f),
            "}"));

    // react
    ASSERT_EQ(
        ToString(FRect(1.0f, 2.0f, 3.0f, 4.0f)),
        std::format("{}min=({}, {}), max=({}, {}){}",
            "{",
            std::to_string(1.0f), std::to_string(2.0f),
            std::to_string(3.0f), std::to_string(4.0f),
            "}"));

    // box
    ASSERT_EQ(
        ToString(FBox(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f)),
        std::format("{}min=({}, {}, {}), max=({}, {}, {}){}",
            "{",
            std::to_string(1.0f), std::to_string(2.0f), std::to_string(3.0f),
            std::to_string(4.0f), std::to_string(5.0f), std::to_string(6.0f),
            "}"));

    // sphere
    ASSERT_EQ(
        ToString(FSphere(1.0f, 2.0f, 3.0f, 1.0f)),
        std::format("{}center=({}, {}, {}), radius={}{}",
            "{",
            std::to_string(1.0f), std::to_string(2.0f), std::to_string(3.0f), std::to_string(1.0f),
            "}"));

    // transform
    ASSERT_EQ(
        ToString(FTransform(FVec3(2.0f, 2.0f, 2.0f), FQuat(0.5f, 0.2f, 0.1f, 0.3f), FVec3(1.0f, 2.0f, 3.0f))),
        std::format("{}scale=({}, {}, {}), rotation=({}, {}, {}, {}), translation=({}, {}, {}){}",
            "{",
            std::to_string(2.0f), std::to_string(2.0f), std::to_string(2.0f),
            std::to_string(0.5f), std::to_string(0.2f), std::to_string(0.1f), std::to_string(0.3f),
            std::to_string(1.0f), std::to_string(2.0f), std::to_string(3.0f),
            "}"));

    // view transform
    ASSERT_EQ(
        ToString(FViewTransform(FQuat(0.5f, 0.2f, 0.1f, 0.3f), FVec3(1.0f, 2.0f, 3.0f))),
        std::format("{}scale=({}, {}, {}), rotation=({}, {}, {}, {}), translation=({}, {}, {}){}",
            "{",
            std::to_string(1.0f), std::to_string(1.0f), std::to_string(1.0f),
            std::to_string(0.5f), std::to_string(0.2f), std::to_string(0.1f), std::to_string(0.3f),
            std::to_string(1.0f), std::to_string(2.0f), std::to_string(3.0f),
            "}"));

    // projection
    ASSERT_EQ(
        ToString(FReversedZOrthoProjection(1024.0f, 768.0f, 1.0f, 500.0f)),
        std::format("{}width={}, height={}, near={}, far={}{}",
            "{",
            std::to_string(1024.0f), std::to_string(768.0f), std::to_string(1.0f), std::to_string(500.0f),
            "}"));
    ASSERT_EQ(
        ToString(FReversedZPerspectiveProjection(90.0f, 1024.0f, 768.0f, 1.0f, 500.0f)),
        std::format("{}fov={}, width={}, height={}, near={}, far={}{}",
            "{",
            std::to_string(90.0f), std::to_string(1024.0f), std::to_string(768.0f), std::to_string(1.0f), std::to_string(500.0f),
            "}"));
}

TEST(MathTest, JsonSerializationTest)
{
    // half
    PerformJsonSerializationTest(HFloat(1.0f), FltToJson(1.0f));

    // vec
    PerformJsonSerializationTest(FVec1(1.0f), std::format("[{}]", FltToJson(1.0f)));
    PerformJsonSerializationTest(HVec2(2.0f, 3.0f), std::format("[{},{}]", FltToJson(2.0f), FltToJson(3.0f)));
    PerformJsonSerializationTest(IVec3(1, 2, 3), "[1,2,3]");
    PerformJsonSerializationTest(UVec4(1, 2, 3, 4), "[1,2,3,4]");

    // mat
    PerformJsonSerializationTest(FMat1x1(1.0f), std::format("[{}]", FltToJson(1.0f)));
    PerformJsonSerializationTest(
        HMat2x3(2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f),
        std::format("[{},{},{},{},{},{}]", FltToJson(2.0f), FltToJson(3.0f), FltToJson(4.0f), FltToJson(5.0f), FltToJson(6.0f), FltToJson(7.0f)));
    PerformJsonSerializationTest(
        IMat3x4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12),
        "[1,2,3,4,5,6,7,8,9,10,11,12]");
    PerformJsonSerializationTest(
        FMat4x4(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f),
        std::format("[{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}]",
            FltToJson(1.0f), FltToJson(2.0f), FltToJson(3.0f), FltToJson(4.0f),
            FltToJson(5.0f), FltToJson(6.0f), FltToJson(7.0f), FltToJson(8.0f),
            FltToJson(9.0f), FltToJson(10.0f), FltToJson(11.0f), FltToJson(12.0f),
            FltToJson(13.0f), FltToJson(14.0f), FltToJson(15.0f), FltToJson(16.0f)));

    // angle/radian
    PerformJsonSerializationTest(FAngle(67.0f), FltToJson(67.0f));
    PerformJsonSerializationTest(FRadian(1.0f), FltToJson(1.0f));

    // quaternion
    PerformJsonSerializationTest(FQuat(1.0f, 0.1f, 0.2f, 0.5f), std::format("[{},{},{},{}]", FltToJson(1.0f), FltToJson(0.1f), FltToJson(0.2f), FltToJson(0.5f)));

    // color
    PerformJsonSerializationTest(Color(1, 2, 3, 1), R"({"r":1,"g":2,"b":3,"a":1})");
    PerformJsonSerializationTest(
        LinearColor(1.0f, 0.5f, 0.2f, 1.0f),
        std::format(R"({}"r":{},"g":{},"b":{},"a":{}{})",
            "{",
            FltToJson(1.0f), FltToJson(0.5f), FltToJson(0.2f), FltToJson(1.0f),
            "}"));

    // rect
    PerformJsonSerializationTest(
        FRect(1.0f, 2.0f, 3.0f, 4.0f),
        std::format(R"({}"min":[{},{}],"max":[{},{}]{})",
            "{",
            FltToJson(1.0f), FltToJson(2.0f),
            FltToJson(3.0f), FltToJson(4.0f),
            "}"));

    // box
    PerformJsonSerializationTest(
        FBox(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f),
        std::format(R"({}"min":[{},{},{}],"max":[{},{},{}]{})",
            "{",
            FltToJson(1.0f), FltToJson(2.0f), FltToJson(3.0f),
            FltToJson(4.0f), FltToJson(5.0f), FltToJson(6.0f),
            "}"));

    // sphere
    PerformJsonSerializationTest(
        FSphere(1.0f, 2.0f, 3.0f, 1.0f),
        std::format(R"({}"center":[{},{},{}],"radius":{}{})",
            "{",
            FltToJson(1.0f), FltToJson(2.0f), FltToJson(3.0f),
            FltToJson(1.0f),
            "}"));

    // transform
    const FTransform transform(FVec3(2.0f, 2.0f, 2.0f), FQuat(1.0f, 0.2f, 0.3f, 0.4f), FVec3(1.0f, 2.0f, 3.0f));
    PerformJsonSerializationTest(
        transform,
        std::format(R"({}"scale":[{},{},{}],"rotation":[{},{},{},{}],"translation":[{},{},{}]{})",
            "{",
            FltToJson(2.0f), FltToJson(2.0f), FltToJson(2.0f),
            FltToJson(1.0f), FltToJson(0.2f), FltToJson(0.3f), FltToJson(0.4f),
            FltToJson(1.0f), FltToJson(2.0f), FltToJson(3.0f),
            "}"));

    // view transform
    PerformJsonSerializationTest(
        FViewTransform(transform),
        std::format(R"({}"scale":[{},{},{}],"rotation":[{},{},{},{}],"translation":[{},{},{}]{})",
            "{",
            FltToJson(2.0f), FltToJson(2.0f), FltToJson(2.0f),
            FltToJson(1.0f), FltToJson(0.2f), FltToJson(0.3f), FltToJson(0.4f),
            FltToJson(1.0f), FltToJson(2.0f), FltToJson(3.0f),
            "}"));

    // projection
    PerformJsonSerializationTest(
        FReversedZOrthoProjection(512.0f, 1024.0f, 1.0f),
        std::format(R"({}"width":{},"height":{},"near":{},"far":{}{})",
            "{",
            FltToJson(512.0f), FltToJson(1024.0f), FltToJson(1.0f), "null",
            "}"));
    PerformJsonSerializationTest(
        FReversedZPerspectiveProjection(90.0f, 512.0f, 1024.0f, 1.0f, 500.0f),
        std::format(R"({}"fov":{},"width":{},"height":{},"near":{},"far":{}{})",
            "{",
            FltToJson(90.0f), FltToJson(512.0f), FltToJson(1024.0f), FltToJson(1.0f), FltToJson(500.0f),
            "}"));
}
