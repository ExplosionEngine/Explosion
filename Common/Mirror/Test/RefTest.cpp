//
// Created by johnk on 2021/9/5.
//

#include <array>

#include <gtest/gtest.h>

#include <Mirror/Ref.h>
using namespace Explosion::Mirror;

struct Hello {
    int a;
    int b;
};

TEST(MirrorTest, RefTest0)
{
    int value0 = 1;
    Ref ref0(&value0);
    ASSERT_EQ(*static_cast<int*>(ref0.Value()), 1);

    float value1 = 2.f;
    Ref ref1 = Ref(&value1);
    ASSERT_EQ(*static_cast<float*>(ref1.Value()) - 2.f < 0.001f, true);

    Hello hello { 1, 2 };
    Ref ref2 = Ref(&hello);
    ASSERT_EQ(static_cast<Hello*>(ref2.Value())->a, 1);
    ASSERT_EQ(static_cast<Hello*>(ref2.Value())->b, 2);

    Any any3 = Hello { 3, 4 };
    Ref ref3 = Ref(any3);
    ASSERT_EQ(static_cast<Hello*>(ref3.Value())->a, 3);
    ASSERT_EQ(static_cast<Hello*>(ref3.Value())->b, 4);
}
