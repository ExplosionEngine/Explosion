//
// Created by Zach Lee on 2022/9/12.
//

#include <gtest/gtest.h>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>
#include <Common/Math/Transform.h>
#include <Common/Math/Rect.h>
#include <Common/Math/Box.h>
#include <Common/Math/Sphere.h>

using namespace Common;

TEST(MathTest, CommonTest)
{
    ASSERT_EQ(DivideAndRoundUp(7, 3), 3);
    ASSERT_EQ(DivideAndRoundUp(6, 2), 3);
    ASSERT_EQ(DivideAndRoundUp(70, 9), 8);
}

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

TEST(MathTest, AngleAndRadianTest)
{
    const FAngle v0(90.0f);
    ASSERT_TRUE(v0.ToRadian() == pi / 2.0f);

    const FRadian v1(pi / 4.0f);
    ASSERT_TRUE(v1.ToAngle() == 45.0f);
}

TEST(MathTest, QuaternionBasicTest)
{
    const FQuat v0(1, 2, 3, 4);
    const FQuat v1(2, 3, 4, 5);
    const FQuat v2(1, 1, 2, 2);
    ASSERT_TRUE((v0 * 2 + v1 - v2) == FQuat(3, 6, 8, 11));
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