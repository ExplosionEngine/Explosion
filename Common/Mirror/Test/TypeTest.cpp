#include <gtest/gtest.h>

#include <Mirror/Mirror.h>
using namespace Explosion::Mirror;

struct S0 {
    int a;
    int b;
};

class C0 {
public:
    int F0() { return 0; }

    int v0;
};

void F0() {}

enum E0 {
    A, B
};

union U0 {
    int a;
    float b;
};

TEST(MirrorTest, ResolveTypeTest)
{
    ASSERT_EQ(Type::Resolve<uint32_t>() == Type::Resolve<uint32_t>(), true);
    ASSERT_EQ(Type::Resolve<uint32_t>() == Type::Resolve<bool>(), false);
    ASSERT_EQ(Type::Resolve<double>() != Type::Resolve<double>(), false);
    ASSERT_EQ(Type::Resolve<int>() != Type::Resolve<double>(), true);
}

TEST(MirrorTest, BasicTypeTraitsTest)
{
    auto voidType = Type::Resolve<void>();
    ASSERT_EQ(voidType.IsVoid(), true);

    auto boolType = Type::Resolve<bool>();
    ASSERT_EQ(boolType.IsIntegral(), true);

    auto intType = Type::Resolve<int>();
    ASSERT_EQ(intType.IsIntegral(), true);

    auto floatType = Type::Resolve<float>();
    ASSERT_EQ(floatType.IsFloatingPoint(), true);

    auto doubleType = Type::Resolve<double>();
    ASSERT_EQ(doubleType.IsFloatingPoint(), true);
}

TEST(MirrorTest, ArrayTypeTraitsTest)
{
    auto intArrayType = Type::Resolve<int[]>();
    ASSERT_EQ(intArrayType.IsArray(), true);

    auto v0MemberType = Type::Resolve<decltype(&C0::v0)>();
    ASSERT_EQ(v0MemberType.IsMemberPointer(), true);
}

TEST(MirrorTest, FunctionTypeTraitsTest)
{
    auto f0Type = Type::Resolve<decltype(F0)>();
    ASSERT_EQ(f0Type.IsFunction(), true);

    auto f0MemberType = Type::Resolve<decltype(&C0::F0)>();
    ASSERT_EQ(f0MemberType.IsMemberPointer(), true);
}

TEST(MirrorTest, ComplexTypeTraitsTest)
{
    auto s0Type = Type::Resolve<S0>();
    ASSERT_EQ(s0Type.IsClass(), true);

    auto c0Type = Type::Resolve<C0>();
    ASSERT_EQ(c0Type.IsClass(), true);

    auto e0Type = Type::Resolve<E0>();
    ASSERT_EQ(e0Type.IsEnum(), true);

    auto u0Type = Type::Resolve<U0>();
    ASSERT_EQ(u0Type.IsUnion(), true);
}
