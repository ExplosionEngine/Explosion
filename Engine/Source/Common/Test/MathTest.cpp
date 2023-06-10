//
// Created by Zach Lee on 2022/9/12.
//

#include <gtest/gtest.h>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>

using namespace Common;

TEST(MathTest, HFloatTest)
{
    HFloat v0 = 1.0f;
    ASSERT_FLOAT_EQ(v0, 1.0f);
    ASSERT_TRUE(v0 == 1.0f);
    ASSERT_TRUE(v0 != 2.0f);

    HFloat v1 = v0 + 2.0f;
    ASSERT_FLOAT_EQ(v1, 3.0f);

    HFloat v2 = v1 / 2.0f;
    ASSERT_FLOAT_EQ(v2.AsFloat(), 1.5f);

    HFloat v3 = 1.0f;
    v3 /= v2;
    ASSERT_TRUE(v3 == (1.0f / 1.5f));

    HFloat v4 = 2.0f;
    v4 -= v2;
    ASSERT_TRUE(v4 == HFloat(0.5f));

    HFloat v5 = 3.0f;
    ASSERT_TRUE(HFloat(9.0f) = HFloat(std::pow(v5, 2.0f)));

    HFloat v6 = 9.0f;
    ASSERT_TRUE(v5 == HFloat(std::sqrt(v6)));
}

TEST(MathTest, FVec1Test)
{
    FVec1 v0;
    ASSERT_FLOAT_EQ(v0.x, 0.0f);

    FVec1 v1(1);
    ASSERT_FLOAT_EQ(v1.x, 1.0f);
    ASSERT_TRUE(v1 == FVec1(1.0f));

    FVec1 v2 = v0 + v1;
    ASSERT_FLOAT_EQ(v2.x, 1.0f);

    FVec1 v3 = v2 * 4 - 1;
    ASSERT_FLOAT_EQ(v3.x, 3.0f);

    FVec1 v4 = (v3 - v2) * v2;
    ASSERT_FLOAT_EQ(v4.x, 2.0f);

    FVec1 v5 = FVec1(6.0f) / v4;
    ASSERT_FLOAT_EQ(v5.x, 3.0f);
}

TEST(MathTest, FVec2Test)
{
    FVec2 v0;
    ASSERT_FLOAT_EQ(v0.x, 0.0f);
    ASSERT_FLOAT_EQ(v0.y, 0.0f);

    FVec2 v1(3.0f, 4.0f);
    ASSERT_FLOAT_EQ(v1[0], 3.0f);
    ASSERT_FLOAT_EQ(v1[1], 4.0f);

    FVec2 v2 = v0 + v1 + 1;
    ASSERT_FLOAT_EQ(v2.x, 4.0f);
    ASSERT_FLOAT_EQ(v2.y, 5.0f);

    FVec2 v3 = v2 / 2.0f;
    ASSERT_TRUE(v3 == FVec2(2.0f, 2.5f));

    FVec2 v4 = v3 / FVec2(2.0f, 5.0f);
    ASSERT_FLOAT_EQ(v4.x, 1.0f);
    ASSERT_FLOAT_EQ(v4.y, 0.5f);
}

TEST(MathTest, FVec3Test)
{
    FVec3 v0(4, 5, 6);
    ASSERT_FLOAT_EQ(v0.x, 4.0f);
    ASSERT_FLOAT_EQ(v0.y, 5.0f);
    ASSERT_FLOAT_EQ(v0.z, 6.0f);

    FVec3 v1 = v0 * 2.0f - FVec3(1, 2, 3);
    ASSERT_FLOAT_EQ(v1.x, 7.0f);
    ASSERT_FLOAT_EQ(v1.y, 8.0f);
    ASSERT_FLOAT_EQ(v1.z, 9.0f);

    FVec3 v2 = v1 / FVec3(2, 2, 2);
    ASSERT_TRUE(v2 == FVec3(3.5f, 4.0f, 4.5f));
}

TEST(MathTest, FVec4Test)
{
    FVec4 v0 = FVec4(4, 5, 6, 7);
    ASSERT_FLOAT_EQ(v0.x, 4.0f);
    ASSERT_FLOAT_EQ(v0.y, 5.0f);
    ASSERT_FLOAT_EQ(v0.z, 6.0f);
    ASSERT_FLOAT_EQ(v0.w, 7.0f);

    FVec4 v1 = v0 * 3.0f + FVec4(1, 1, 2, 2) - FVec4(4, 4, 3, 3);
    ASSERT_FLOAT_EQ(v1.x, 9.0f);
    ASSERT_FLOAT_EQ(v1.y, 12.0f);
    ASSERT_FLOAT_EQ(v1.z, 17.0f);
    ASSERT_FLOAT_EQ(v1.w, 20.0f);
}

TEST(MathTest, IVec1Test)
{
    IVec1 v0 = 1;
    ASSERT_EQ(v0.x, 1);

    IVec1 v1 = (v0 + 3) / 2;
    ASSERT_EQ(v1.x, 2);

    IVec1 v2 = v1 - IVec1(3);
    ASSERT_EQ(v2.x, -1);
}

TEST(MathTest, IVec2Test)
{
    IVec2 v0(-1, -2);
    ASSERT_EQ(v0.x, -1);
    ASSERT_EQ(v0.y, -2);

    IVec2 v1 { 3, 4 };
    ASSERT_EQ(v1.x, 3);
    ASSERT_EQ(v1.y, 4);

    IVec2 v2 = v0 + v1 - 1;
    ASSERT_TRUE(v2 == IVec2(1, 1));

    IVec2 v3 = v2 * 3 / IVec2(2);
    ASSERT_EQ(v3.x, 1);
    ASSERT_EQ(v3.y, 1);
}

TEST(MathTest, IVec3Test)
{
    IVec3 v0 = IVec3(3, 4, 5);
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
    IVec4 v0 = IVec4(-1, -2, -3, -4);
    ASSERT_EQ(v0.x, -1);
    ASSERT_EQ(v0.y, -2);
    ASSERT_EQ(v0.z, -3);
    ASSERT_EQ(v0.w, -4);

    IVec4 v1;
    v1 += v0 * 2;
    ASSERT_TRUE(v1 == IVec4(-2, -4, -6, -8));

    IVec4 v2 = (v1 + 10) / IVec4(1, 2, 3, 4);
    ASSERT_TRUE(v2 == IVec4(8, 3, 1, 0));
}

TEST(MathTest, HVec1Test)
{
    HVec1 v0 = 1.0f;
    ASSERT_FLOAT_EQ(v0.x, 1.0f);

    HVec1 v1;
    v1 += v0;
    v1 /= HFloat(2.0f);
    v1 -= HFloat(3.0f);
    ASSERT_TRUE(v1.x == HFloat(-2.5f));
}

TEST(MathTest, HVec2Test)
{
    HVec2 v0 = HVec2(1.0f, 2.0f);
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
    HVec3 v0 { HFloat(1.0f), HFloat(2.0f), HFloat(3.0f) };
    ASSERT_TRUE(v0.x == 1.0f);
    ASSERT_TRUE(v0.y == HFloat(2.0f));
    ASSERT_TRUE(v0.z == 3.0f);
    ASSERT_FALSE(v0.z == 2.0f);

    HVec3 v1 = v0 + HFloat(3.0f) / HFloat(2.0f) - HFloat(1.0f);
    ASSERT_TRUE(v1 == HVec3(1.5f, 2.5f, 3.5f));
}

TEST(MathTest, HVec4Test)
{
    HVec4 v0 = 2.0f;
    ASSERT_FALSE(v0.x == 3.0f);
    ASSERT_TRUE(v0.w == 2.0f);

    HVec4 v1 = v0 - HVec4(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_TRUE(v1 == HVec4(1.0f, 0.0f, -1.0f, -2.0f));
}

TEST(MathTest, SubVecTest)
{
    FVec4 v0 = FVec4(1, 2, 3, 4);
    FVec3 v1 = v0.SubVec<0, 1, 2>();
    ASSERT_TRUE(v1 == FVec3(1, 2, 3));

    FVec2 v2 = v0.SubVec<1, 3>();
    ASSERT_TRUE(v2 == FVec2(2, 4));
}

TEST(MathTest, VecModelTest)
{
    FVec4 v0 = FVec4(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_FLOAT_EQ(v0.Model(), std::sqrt(30.0f));

    FVec2 v1 { 2.0f, 3.0f };
    ASSERT_FLOAT_EQ(v1.Model(), std::sqrt(13.0f));
}

TEST(MathTest, VecDotTest)
{
    FVec4 v0 = FVec4(1.0f, 2.0f, 3.0f, 4.0f);
    FVec4 v1 = FVec4(2.0f, 3.0f, 4.0f, 5.0f);
    ASSERT_FLOAT_EQ(v0.Dot(v1), 40.0f);

    FVec2 v2 { 2.0f, 3.0f };
    FVec2 v3 { 3.0f, 4.0f };
    ASSERT_FLOAT_EQ(v2.Dot(v3), 18.0f);
}

TEST(MathTest, VecCrossTest)
{
    FVec2 v0 { 1.0f, 2.0f };
    FVec2 v1 { 3.0f, 4.0f };
    ASSERT_FLOAT_EQ(v0.Cross(v1), -2.0f);

    FVec3 v2 = FVec3(1.0f, 2.0f, 3.0f);
    FVec3 v3 = FVec3(2.0f, 3.0f, 4.0f);
    FVec3 v4 = v2.Cross(v3);
    ASSERT_FLOAT_EQ(v4.x, -1.0f);
    ASSERT_FLOAT_EQ(v4.y, 2.0f);
    ASSERT_FLOAT_EQ(v4.z, -1.0f);
}

TEST(MathTest, VecConstsTest)
{
    ASSERT_TRUE(FVec2(1, 0) == FVec2Consts::unitX);
    ASSERT_TRUE(IVec3(0, 0, 1) == IVec3Consts::unitZ);
}

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

    FMat3x4 v1 = (v0 - 3.0f) * 4.0f;
    FMat3x4 v2;
    v2.SetRows(
        FVec4(-8.0f, -4.0f, 0.0f, 4.0f),
        FVec4(8.0f, 12.0f, 16.0f, 20.0f),
        FVec4(24.0f, 28.0f, 32.0f, 36.0f)
    );
    ASSERT_TRUE(v1 == v2);
}

TEST(MathTest, MatViewTest)
{
    FMat3x3 v0(
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
    FMat2x4 v0 = FMat2x4Consts::zero;
    ASSERT_TRUE(v0.Row(1) == FVec4Consts::zero);

    FMat3x3 v1 = FMat3x3Consts::zero;
    ASSERT_TRUE(v1.Col(2) == FVec3Consts::zero);

    FMat3x3 v2 = FMat3x3Consts::identity;
    ASSERT_TRUE(v2.Row(0) == FVec3(1, 0, 0));
    ASSERT_TRUE(v2.Row(1) == FVec3(0, 1, 0));
    ASSERT_TRUE(v2.Row(2) == FVec3(0, 0, 1));
}

TEST(MathTest, MatSetTest)
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

TEST(MathTest, MatMulTest)
{
    FMat3x4 v0(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    );
    FMat4x2 v1(
        1, 2,
        3, 4,
        5, 6,
        7, 8
    );
    FMat3x2 v2 = v0 * v1;
    ASSERT_TRUE(v2.Row(0) == FVec2(50, 60));
    ASSERT_TRUE(v2.Row(1) == FVec2(114, 140));
    ASSERT_TRUE(v2.Row(2) == FVec2(178, 220));
}

TEST(MathTest, MathTranposeTest)
{
    FMat3x4 v0(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    );
    FMat4x3 v1 = v0.Transpose();
    ASSERT_TRUE(v1.Row(0) == FVec3(1, 5, 9));
    ASSERT_TRUE(v1.Row(1) == FVec3(2, 6, 10));
    ASSERT_TRUE(v1.Row(2) == FVec3(3, 7, 11));
    ASSERT_TRUE(v1.Row(3) == FVec3(4, 8, 12));
}

TEST(MathTest, AngleAndRadianTest)
{
    // TODO
}

TEST(MathTest, EulerRotationToQuaternionTest)
{
    // TODO
}

TEST(MathTest, QuaternionBasicTest)
{
    // TODO
}

TEST(MathTest, QuaternionRotationTest)
{
    // TODO
}
