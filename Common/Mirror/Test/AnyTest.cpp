//
// Created by John Kindem on 2021/9/5.
//

#include <array>

#include <gtest/gtest.h>

#include <Mirror/Any.h>
using namespace Explosion::Mirror;

#define EXCEPTION_COUNTER(block) \
try {                            \
    block                        \
} catch(std::exception& e) {     \
    exceptionTimes++;            \
}                                \

struct SmallTestStruct {
    uint32_t a;
    uint32_t b;
};

struct BigTestStruct {
    std::array<int, 4> a;
    std::array<double, 4> b;
};

TEST(MirrorTest, AnyTest0)
{
    uint32_t exceptionTimes = 0;

    Any any = 1;
    ASSERT_EQ(any.CastTo<int>(), 1);
    ASSERT_EQ(*any.CastToPointer<int>(), 1);
    EXCEPTION_COUNTER({ any.CastTo<float>(); })

    any = 2.f;
    ASSERT_EQ(std::abs(any.CastTo<float>() - 2.f) < 0.001f, true);
    EXCEPTION_COUNTER({ any.CastTo<int>(); })

    ASSERT_EQ(exceptionTimes, 2);
}

TEST(MirrorTest, AnyTest1)
{
    uint32_t exceptionTimes = 0;

    Any any = SmallTestStruct { 1, 2 };
    ASSERT_EQ(any.CastTo<SmallTestStruct>().a, 1);
    ASSERT_EQ(any.CastTo<SmallTestStruct>().b, 2);
    ASSERT_EQ(any.CastToPointer<SmallTestStruct>()->a, 1);
    ASSERT_EQ(any.CastToPointer<SmallTestStruct>()->b, 2);
    EXCEPTION_COUNTER({ any.CastTo<int>(); })
    EXCEPTION_COUNTER({ any.CastTo<BigTestStruct>(); })

    any = BigTestStruct {
        { 1, 1, 1, 1 },
        { 2.f, 2.f, 2.f, 2.f }
    };
    for (auto i = 0 ; i < 4; i++) {
        ASSERT_EQ(any.CastTo<BigTestStruct>().a[i], 1);
        ASSERT_EQ(any.CastTo<BigTestStruct>().b[i] - 2.f < 0.001f, true);
    }
    EXCEPTION_COUNTER({ any.CastTo<int>(); })
    EXCEPTION_COUNTER({ any.CastTo<SmallTestStruct>(); })

    ASSERT_EQ(exceptionTimes, 4);
}

TEST(MirrorTest, AnyTest3)
{
    Any any0(1);
    ASSERT_EQ(any0.CastTo<int>(), 1);

    Any any1(any0);
    ASSERT_EQ(any1.CastTo<int>(), 1);

    Any any2 = Any(1);
    ASSERT_EQ(any2.CastTo<int>(), 1);

    Any any3 = std::move(any0);
    ASSERT_EQ(any3.CastTo<int>(), 1);
}

TEST(MirrorTest, AnyTest4)
{
    uint32_t exceptionTimes = 0;

    int value0 = 1;
    int* pointer0 = &value0;
    Any any0(*pointer0);
    ASSERT_EQ(any0.CastTo<int>(), 1);
    EXCEPTION_COUNTER({ any0.CastTo<float>(); })

    float value1 = 2.f;
    float* pointer1 = &value1;
    Any any1(*pointer1);
    ASSERT_EQ(std::abs(any1.CastTo<float>() - 2.f) < 0.0001f, true);
    EXCEPTION_COUNTER({ any1.CastTo<double>(); })

    double value2 = 3.0;
    double* pointer2 = &value2;
    Any any2(*pointer2);
    ASSERT_EQ(std::abs(any2.CastTo<double>() - 3.0) < 0.0001, true);
    EXCEPTION_COUNTER({ any2.CastTo<int>(); })

    ASSERT_EQ(exceptionTimes, 3);
}
