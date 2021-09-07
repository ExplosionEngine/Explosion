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
    ASSERT_EQ(any.CastTo<float>() - 2.f < 0.001f, true);
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
