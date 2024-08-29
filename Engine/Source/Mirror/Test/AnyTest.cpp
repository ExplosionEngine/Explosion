//
// Created by johnk on 2022/9/5.
//

#include <string>
#include <vector>
#include <unordered_map>

#include <Test/Test.h>
#include <Mirror/Mirror.h>
#include <Mirror/Registry.h>

using namespace Mirror;

struct AnyDtorTest {
    explicit AnyDtorTest(bool& inLive)
        : record(false)
        , live(inLive)
    {
    }

    AnyDtorTest(bool& inLive, bool inRecord)
        : record(inRecord)
        , live(inLive)
    {
        if (record) {
            live = true;
        }
    }

    AnyDtorTest(AnyDtorTest&& inOther) noexcept
        : record(true)
        , live(inOther.live)
    {
        live = true;
    }

    ~AnyDtorTest()
    {
        if (record) {
            live = false;
        }
    }

    bool record;
    bool& live;
};

struct AnyCopyCtorTest {
    explicit AnyCopyCtorTest(bool& inCopyCtorCalled)
        : copyCtorCalled(inCopyCtorCalled)
    {
    }

    AnyCopyCtorTest(AnyCopyCtorTest&& inOther) noexcept
        : copyCtorCalled(inOther.copyCtorCalled)
    {
    }

    AnyCopyCtorTest(const AnyCopyCtorTest& inOther)
        : copyCtorCalled(inOther.copyCtorCalled)
    {
        copyCtorCalled = true;
    }

    bool& copyCtorCalled;
};

struct AnyMoveCtorTest {
    explicit AnyMoveCtorTest(uint8_t& inMoveTime)
        : moveTime(inMoveTime)
    {
    }

    AnyMoveCtorTest(AnyMoveCtorTest&& inOther) noexcept
        : moveTime(inOther.moveTime)
    {
        moveTime++;
    }

    uint8_t& moveTime;
};

struct AnyBasicTest {
    int a;
    float b;

    bool operator==(const AnyBasicTest& inRhs) const
    {
        return a == inRhs.a
            && b == inRhs.b;
    }
};

struct AnyBaseClassTest {};

struct AnyDerivedClassTest : AnyBaseClassTest {};

struct AnyTestMirrorRegistry {
    AnyTestMirrorRegistry()
    {
        Registry::Get()
            .Class<AnyBaseClassTest>("AnyBaseClassTest");

        Registry::Get()
            .Class<AnyDerivedClassTest, AnyBaseClassTest>("AnyDerivedClassTest");
    }
};
static AnyTestMirrorRegistry registry;

TEST(AnyTest, DefaultCtorTest)
{
    Any a0; // NOLINT
    ASSERT_TRUE(a0.Empty());

    Any a1 = {}; // NOLINT
    ASSERT_TRUE(a1.Empty());

    Any a2 {};
    ASSERT_TRUE(a2.Empty());
}

TEST(AnyTest, DetorTest)
{
    bool live = false;
    {
        Any a0 = AnyDtorTest(live);
        ASSERT_TRUE(live);
    }
    ASSERT_FALSE(live);

    live = false;
    {
        AnyDtorTest obj(live, true); // NOLINT
        {
            Any a1 = std::ref(obj);
            ASSERT_TRUE(live);
        }
        ASSERT_TRUE(live);
    }
    ASSERT_FALSE(live);
}

TEST(AnyTest, CopyCtorTest)
{
    bool called = false;
    Any a0 = AnyCopyCtorTest(called);
    ASSERT_FALSE(called);

    Any a1 = a0;
    ASSERT_TRUE(called);

    called = false;
    const Any a2 = AnyCopyCtorTest(called);
    ASSERT_FALSE(called);

    Any a3 = a2;
    ASSERT_TRUE(called);
}

TEST(AnyTest, CopyCtorWithPolicyTest)
{
    bool called = false;
    Any a0 = AnyCopyCtorTest(called);
    ASSERT_FALSE(called);

    Any a1 { a0, AnyPolicy::ref };
    ASSERT_FALSE(called);
    ASSERT_EQ(a1.Policy(), AnyPolicy::ref);
    ASSERT_EQ(a1.Size(), a0.Size());
    ASSERT_EQ(a1.Data(), a0.Data());

    Any a2 { a0, AnyPolicy::constRef };
    ASSERT_FALSE(called);
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a2.Size(), a0.Size());
    ASSERT_EQ(a2.Data(), a0.Data());

    Any a3 { a0, AnyPolicy::memoryHolder };
    ASSERT_TRUE(called);
    ASSERT_EQ(a3.Policy(), AnyPolicy::memoryHolder);
    ASSERT_EQ(a3.Size(), a0.Size());
    ASSERT_NE(a3.Data(), a0.Data());
}

TEST(AnyTest, MoveTest)
{
    uint8_t moveTime = 0;
    Any a0 = AnyMoveCtorTest(moveTime);
    ASSERT_EQ(moveTime, 1);

    Any a1 = std::move(a0);
    ASSERT_EQ(moveTime, 2);

    const Any a2 = std::move(a1);
    ASSERT_EQ(moveTime, 3);
}

TEST(AnyTest, ValueCtorTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.As<int>(), 1);

    const Any a1 = 2.0f;
    ASSERT_EQ(a1.As<float>(), 2.0f);

    Any a2 = std::vector { 1, 2, 3 };
    ::Test::AssertVecEq(a2.As<std::vector<int>>(), std::vector { 1, 2, 3 });

    const Any a3 = AnyBasicTest { 1, 2.0f };
    ASSERT_EQ(a3.As<const AnyBasicTest&>(), (AnyBasicTest { 1, 2 }));

    std::pair v0 { 1, 2 };
    const Any a4 = v0;
    const auto& [key, value] = a4.As<const decltype(v0)&>();
    ASSERT_EQ(key, 1);
    ASSERT_EQ(value, 2);
}

TEST(AnyTest, RefCtorTest)
{
    int v0 = 1;
    Any a0 = std::ref(v0);
    ASSERT_EQ(a0.Policy(), AnyPolicy::ref);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const float v1 = 2.0f; // NOLINT
    auto r0 = std::ref(v1);
    Any a1 = r0;
    ASSERT_EQ(a1.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a1.As<const float&>(), 2.0f);

    std::vector v2 = { 1, 2, 3 };
    const Any a2 = std::ref(v2);
    ASSERT_EQ(a2.Policy(), AnyPolicy::ref);
    ::Test::AssertVecEq(a2.As<const decltype(v2)&>(), std::vector { 1, 2, 3 });

    const AnyBasicTest v3 { 1, 2.0f }; // NOLINT
    const auto r1 = std::ref(v3);
    Any a3 = r1;
    ASSERT_EQ(a3.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a3.As<AnyBasicTest>(), (AnyBasicTest { 1, 2.0f }));
}

TEST(AnyTest, CopyAssignTest)
{
    bool called = false;
    const Any a0 = AnyCopyCtorTest(called);
    Any a1 = 1;
    ASSERT_FALSE(called);
    ASSERT_NE(a0.TypeId(), a1.TypeId());

    a1 = a0;
    ASSERT_TRUE(called);
    ASSERT_EQ(a0.TypeId(), a1.TypeId());
}

TEST(AnyTest, MoveAssignTest)
{
    uint8_t moveTime = 0;
    Any a0 = AnyMoveCtorTest(moveTime);
    Any a1 = 1.0f;
    ASSERT_EQ(moveTime, 1);
    ASSERT_NE(a0.TypeId(), a1.TypeId());

    a1 = std::move(a0);
    ASSERT_EQ(moveTime, 2);
    ASSERT_EQ(a0.TypeId(), a1.TypeId());
}

TEST(AnyTest, ValueAssignTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);

    a0 = 2.0f;
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_EQ(a0.As<float>(), 2.0f);
}

TEST(AnyTest, RefAssignTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);

    float v0 = 2.0f;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_EQ(a0.Policy(), AnyPolicy::ref);
    ASSERT_EQ(a0.As<float>(), 2.0f);
}

TEST(AnyTest, ConvertibleTest)
{
    Any a0 = 1;
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_FALSE(a0.Convertible<int&&>());

    const int v0 = 1; // NOLINT
    a0 = v0;
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_FALSE(a0.Convertible<int&&>());

    int v1 = 1;
    a0 = std::ref(v1);
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_FALSE(a0.Convertible<int&&>());

    a0 = std::ref(v0);
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_FALSE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_FALSE(a0.Convertible<int&&>());

    a0 = 1;
    Any a1 = a0.Ref();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_TRUE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_FALSE(a1.Convertible<int&&>());

    a1 = a1.ConstRef();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_FALSE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_FALSE(a1.Convertible<int&&>());

    a1 = a1.AsValue();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_TRUE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_FALSE(a1.Convertible<int&&>());

    AnyDerivedClassTest derived {};
    a0 = derived;
    ASSERT_TRUE(a0.Convertible<AnyDerivedClassTest>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest>());
    ASSERT_TRUE(a0.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest&>());
    ASSERT_FALSE(a0.Convertible<AnyBaseClassTest>());
    ASSERT_FALSE(a0.Convertible<const AnyBaseClassTest>());
    ASSERT_TRUE(a0.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyBaseClassTest&>());

    a0 = &derived;
    ASSERT_TRUE(a0.Convertible<AnyDerivedClassTest*>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest*>());
    ASSERT_TRUE(a0.Convertible<AnyBaseClassTest*>());
    ASSERT_TRUE(a0.Convertible<const AnyBaseClassTest*>());

    const AnyDerivedClassTest derived2 {}; // NOLINT
    a0 = &derived2;
    ASSERT_FALSE(a0.Convertible<AnyDerivedClassTest*>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest*>());
    ASSERT_FALSE(a0.Convertible<AnyBaseClassTest*>());
    ASSERT_TRUE(a0.Convertible<const AnyBaseClassTest*>());

    a0 = std::ref(derived);
    ASSERT_TRUE(a0.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest&>());
    ASSERT_TRUE(a0.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyBaseClassTest&>());

    a0 = a0.ConstRef();
    ASSERT_FALSE(a0.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyDerivedClassTest&>());
    ASSERT_FALSE(a0.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a0.Convertible<const AnyBaseClassTest&>());
}

TEST(AnyTest, ConstConvertibleTest)
{
    int v0 = 1;
    const Any a0 = v0;
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_FALSE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());

    const int v1 = 1; // NOLINT
    const Any a1 = v1;
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_FALSE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());

    const Any a2 = std::ref(v0);
    ASSERT_TRUE(a2.Convertible<int>());
    ASSERT_TRUE(a2.Convertible<const int>());
    ASSERT_TRUE(a2.Convertible<int&>());
    ASSERT_TRUE(a2.Convertible<const int&>());

    const Any a3 = std::ref(v1);
    ASSERT_TRUE(a3.Convertible<int>());
    ASSERT_TRUE(a3.Convertible<const int>());
    ASSERT_FALSE(a3.Convertible<int&>());
    ASSERT_TRUE(a3.Convertible<const int&>());

    Any a4 = 1;
    const Any a5 = a4.Ref();
    ASSERT_TRUE(a5.Convertible<int>());
    ASSERT_TRUE(a5.Convertible<const int>());
    ASSERT_TRUE(a5.Convertible<int&>());
    ASSERT_TRUE(a5.Convertible<const int&>());

    const Any a6 = a4.ConstRef();
    ASSERT_TRUE(a6.Convertible<int>());
    ASSERT_TRUE(a6.Convertible<const int>());
    ASSERT_FALSE(a6.Convertible<int&>());
    ASSERT_TRUE(a6.Convertible<const int&>());

    const Any a7 = a6.AsValue();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_FALSE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());

    AnyDerivedClassTest derived {};
    const Any a8 = derived;
    ASSERT_TRUE(a8.Convertible<AnyDerivedClassTest>());
    ASSERT_TRUE(a8.Convertible<const AnyDerivedClassTest>());
    ASSERT_FALSE(a8.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a8.Convertible<const AnyDerivedClassTest&>());
    ASSERT_FALSE(a8.Convertible<AnyBaseClassTest>());
    ASSERT_FALSE(a8.Convertible<const AnyBaseClassTest>());
    ASSERT_FALSE(a8.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a8.Convertible<const AnyBaseClassTest&>());

    const Any a9 = &derived;
    ASSERT_TRUE(a9.Convertible<AnyDerivedClassTest*>());
    ASSERT_TRUE(a9.Convertible<const AnyDerivedClassTest*>());
    ASSERT_TRUE(a9.Convertible<AnyBaseClassTest*>());
    ASSERT_TRUE(a9.Convertible<const AnyBaseClassTest*>());

    const AnyDerivedClassTest derived2 {}; // NOLINT
    const Any a10 = &derived2;
    ASSERT_FALSE(a10.Convertible<AnyDerivedClassTest*>());
    ASSERT_TRUE(a10.Convertible<const AnyDerivedClassTest*>());
    ASSERT_FALSE(a10.Convertible<AnyBaseClassTest*>());
    ASSERT_TRUE(a10.Convertible<const AnyBaseClassTest*>());

    const Any a11 = std::ref(derived);
    ASSERT_TRUE(a11.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a11.Convertible<const AnyDerivedClassTest&>());
    ASSERT_TRUE(a11.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a11.Convertible<const AnyBaseClassTest&>());

    const Any a12 = a11.ConstRef();
    ASSERT_FALSE(a12.Convertible<AnyDerivedClassTest&>());
    ASSERT_TRUE(a12.Convertible<const AnyDerivedClassTest&>());
    ASSERT_FALSE(a12.Convertible<AnyBaseClassTest&>());
    ASSERT_TRUE(a12.Convertible<const AnyBaseClassTest&>());
}

TEST(AnyTest, PointerConvertibleTest)
{
    int v0 = 1;
    const int v1 = 1; // NOLINT

    Any a0 = &v0;
    ASSERT_FALSE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<int*>());
    ASSERT_TRUE(a0.Convertible<const int*>());

    a0 = &v1;
    ASSERT_FALSE(a0.Convertible<int>());
    ASSERT_FALSE(a0.Convertible<int*>());
    ASSERT_TRUE(a0.Convertible<const int*>());
}

TEST(AnyTest, PointerConstConvertibleTest)
{
    int v0 = 1;
    const int v1 = 1; // NOLINT

    const Any a0 = &v0;
    ASSERT_FALSE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<int*>());
    ASSERT_TRUE(a0.Convertible<const int*>());

    const Any a1 = &v1;
    ASSERT_FALSE(a1.Convertible<int>());
    ASSERT_FALSE(a1.Convertible<int*>());
    ASSERT_TRUE(a1.Convertible<const int*>());
}

TEST(AnyTest, ValueAsTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.As<int>(), 1);
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<int&>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v0 = 2; // NOLINT
    a0 = v0;
    ASSERT_EQ(a0.As<int>(), 2);
    ASSERT_EQ(a0.As<const int>(), 2);
    ASSERT_EQ(a0.As<int&>(), 2);
    ASSERT_EQ(a0.As<const int&>(), 2);

    a0 = std::pair { 1, 2 };
    auto& [key, value] = a0.As<std::pair<int, int>&>();
    ASSERT_EQ(key, 1);
    ASSERT_EQ(value, 2);
    key = 2;
    value = 3;
    const auto& [key2, value2] = a0.As<const std::pair<int, int>&>();
    ASSERT_EQ(key2, 2);
    ASSERT_EQ(value2, 3);

    uint8_t moveTime = 0;
    a0 = AnyMoveCtorTest(moveTime);
    ASSERT_EQ(moveTime, 1);
    AnyMoveCtorTest v1 = std::move(a0.As<AnyMoveCtorTest&>());
    ASSERT_EQ(moveTime, 2);
}

TEST(AnyTest, ValueConstAsTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.As<int>(), 1);
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v0 = 2; // NOLINT
    const Any v1 = v0;
    ASSERT_EQ(v1.As<int>(), 2);
    ASSERT_EQ(v1.As<const int>(), 2);
    ASSERT_EQ(v1.As<const int&>(), 2);

    const Any v2 = std::pair { 1, 2 };
    const auto& [key, value] = v2.As<const std::pair<int, int>&>();
    ASSERT_EQ(key, 1);
    ASSERT_EQ(value, 2);
}

TEST(AnyTest, RefAsTest)
{
    int v0 = 1;
    Any a0 = std::ref(v0);
    ASSERT_EQ(a0.As<int>(), 1);
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<int&>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v1 = 2; // NOLINT
    Any a1 = std::ref(v1);
    ASSERT_EQ(a1.As<int>(), 2);
    ASSERT_EQ(a1.As<const int>(), 2);
    ASSERT_EQ(a1.As<const int&>(), 2);
}

TEST(AnyTest, RefConstAsTest)
{
    int v0 = 1;
    const Any a0 = std::ref(v0);
    ASSERT_EQ(a0.As<int>(), 1);
    ASSERT_EQ(a0.As<const int>(), 1);
    ASSERT_EQ(a0.As<int&>(), 1);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const int v1 = 2; // NOLINT
    const Any a1 = std::ref(v1);
    ASSERT_EQ(a1.As<int>(), 2);
    ASSERT_EQ(a1.As<const int>(), 2);
    ASSERT_EQ(a1.As<const int&>(), 2);
}

TEST(AnyTest, TryAsTest)
{
    int v0 = 1;
    Any a0 = v0;
    ASSERT_EQ(*a0.TryAs<int>(), 1);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
    ASSERT_EQ(a0.TryAs<const float>(), nullptr);

    const int v1 = 1; // NOLINT
    a0 = v1;
    ASSERT_EQ(*a0.TryAs<int>(), 1);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
    ASSERT_EQ(a0.TryAs<const float>(), nullptr);

    a0 = std::ref(v0);
    ASSERT_EQ(*a0.TryAs<int>(), 1);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
    ASSERT_EQ(a0.TryAs<const float>(), nullptr);

    a0 = std::ref(v1);
    ASSERT_EQ(a0.TryAs<int>(), nullptr);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
    ASSERT_EQ(a0.TryAs<const float>(), nullptr);
}

TEST(AnyTest, ConstTryAsTest)
{
    int v0 = 1;
    const Any a0 = v0;
    ASSERT_EQ(a0.TryAs<int>(), nullptr);
    ASSERT_EQ(*a0.TryAs<const int>(), 1);
    ASSERT_EQ(a0.TryAs<float>(), nullptr);
    ASSERT_EQ(a0.TryAs<const float>(), nullptr);

    const int v1 = 1; // NOLINT
    const Any a1 = v1;
    ASSERT_EQ(a1.TryAs<int>(), nullptr);
    ASSERT_EQ(*a1.TryAs<const int>(), 1);
    ASSERT_EQ(a1.TryAs<float>(), nullptr);
    ASSERT_EQ(a1.TryAs<const float>(), nullptr);

    const Any a2 = std::ref(v0);
    ASSERT_EQ(*a2.TryAs<int>(), 1);
    ASSERT_EQ(*a2.TryAs<const int>(), 1);
    ASSERT_EQ(a2.TryAs<float>(), nullptr);
    ASSERT_EQ(a2.TryAs<const float>(), nullptr);

    const Any a3 = std::ref(v1);
    ASSERT_EQ(a3.TryAs<int>(), nullptr);
    ASSERT_EQ(*a3.TryAs<const int>(), 1);
    ASSERT_EQ(a3.TryAs<float>(), nullptr);
    ASSERT_EQ(a3.TryAs<const float>(), nullptr);
}

TEST(AnyTest, GetRefTest)
{
    Any a0 = 1;
    const Any a1 = a0.Ref();
    ASSERT_EQ(a1.Policy(), AnyPolicy::ref);

    const Any a2 = a0.ConstRef();
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);

    Any a3 = a0.Ref();
    const Any a4 = a3.Ref();
    ASSERT_EQ(a4.Policy(), AnyPolicy::ref);
    const Any a5 = a3.ConstRef();
    ASSERT_EQ(a5.Policy(), AnyPolicy::constRef);

    Any a6 = a0.ConstRef();
    const Any a7 = a6.Ref();
    ASSERT_EQ(a7.Policy(), AnyPolicy::constRef);
    const Any a8 = a6.Ref();
    ASSERT_EQ(a8.Policy(), AnyPolicy::constRef);
}

TEST(AnyTest, ConstGetRefTest)
{
    const Any a0 = 1;
    const Any a1 = a0.Ref();
    ASSERT_EQ(a1.Policy(), AnyPolicy::constRef);

    const Any a2 = a0.ConstRef();
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);

    const Any a3 = a1.Ref();
    ASSERT_EQ(a3.Policy(), AnyPolicy::constRef);
    const Any a4 = a1.ConstRef();
    ASSERT_EQ(a4.Policy(), AnyPolicy::constRef);

    const Any a5 = a2.Ref();
    ASSERT_EQ(a5.Policy(), AnyPolicy::constRef);
    const Any a6 = a2.ConstRef();
    ASSERT_EQ(a6.Policy(), AnyPolicy::constRef);

    int v0 = 1;
    const Any a7 = std::ref(v0);
    const Any a8 = a7.Ref();
    ASSERT_EQ(a8.Policy(), AnyPolicy::ref);
    const Any a9 = a7.ConstRef();
    ASSERT_EQ(a9.Policy(), AnyPolicy::constRef);
}

TEST(AnyTest, AsValueTest)
{
    const Any a0 = 1;
    const Any a1 = a0.AsValue();
    ASSERT_NE(a0.Data(), a1.Data());
    ASSERT_EQ(a1.Policy(), AnyPolicy::memoryHolder);

    int v0 = 2;
    const Any a2 = std::ref(v0);
    const Any a3 = a2.AsValue();
    ASSERT_EQ(&v0, a2.Data());
    ASSERT_NE(a2.Data(), a3.Data());
    ASSERT_EQ(a3.Policy(), AnyPolicy::memoryHolder);

    const int v1 = 2; // NOLINT
    const Any a4 = std::ref(v1);
    const Any a5 = a4.AsValue();
    ASSERT_EQ(&v1, a4.Data());
    ASSERT_NE(a4.Data(), a5.Data());
    ASSERT_EQ(a5.Policy(), AnyPolicy::memoryHolder);
}

TEST(AnyTest, GetPtrTest)
{
    Any a0 = 1;
    ASSERT_EQ(*a0.Ptr().As<int*>(), 1);
    ASSERT_EQ(*a0.Ptr().As<const int*>(), 1);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(*a0.Ptr().As<int*>(), 1);
    ASSERT_EQ(*a0.Ptr().As<const int*>(), 1);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_FALSE(a0.Ptr().Convertible<int*>());
    ASSERT_EQ(*a0.Ptr().As<const int*>(), 1);
}

TEST(AnyTest, ConstGetPtrTest)
{
    const Any a0 = 1;
    ASSERT_FALSE(a0.Ptr().Convertible<int*>());
    ASSERT_EQ(*a0.Ptr().As<const int*>(), 1);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(*a1.Ptr().As<int*>(), 1);
    ASSERT_EQ(*a1.Ptr().As<const int*>(), 1);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_FALSE(a2.Ptr().Convertible<int*>());
    ASSERT_EQ(*a2.Ptr().As<const int*>(), 1);
}

TEST(AnyTest, GetConstPtrTest)
{
    const Any a0 = 1;
    ASSERT_FALSE(a0.ConstPtr().Convertible<int*>());
    ASSERT_EQ(*a0.ConstPtr().As<const int*>(), 1);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_FALSE(a1.ConstPtr().Convertible<int*>());
    ASSERT_EQ(*a1.ConstPtr().As<const int*>(), 1);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_FALSE(a2.ConstPtr().Convertible<int*>());
    ASSERT_EQ(*a2.ConstPtr().As<const int*>(), 1);
}

TEST(AnyTest, DereferenceTest)
{
    int v0 = 1;
    const Any a0 = &v0;
    ASSERT_EQ(a0.Deref().As<const int&>(), 1);
}

TEST(AnyTest, PolicyTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.Policy(), AnyPolicy::memoryHolder);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.Policy(), AnyPolicy::ref);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_EQ(a0.Policy(), AnyPolicy::constRef);
}

TEST(AnyTest, TypeIdTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<int*>()->id);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<const int>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int&>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<const int&>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<int*>()->id);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<const int>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int&>()->id);
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<const int&>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<int*>()->id);
}

TEST(AnyTest, ConstTypeIdTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a0.TypeId(), GetTypeInfo<int*>()->id);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_EQ(a1.TypeId(), GetTypeInfo<const int>()->id);
    ASSERT_EQ(a1.TypeId(), GetTypeInfo<int&>()->id);
    ASSERT_EQ(a1.TypeId(), GetTypeInfo<const int&>()->id);
    ASSERT_NE(a1.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a1.TypeId(), GetTypeInfo<int*>()->id);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.TypeId(), GetTypeInfo<int>()->id);
    ASSERT_EQ(a2.TypeId(), GetTypeInfo<const int>()->id);
    ASSERT_EQ(a2.TypeId(), GetTypeInfo<int&>()->id);
    ASSERT_EQ(a2.TypeId(), GetTypeInfo<const int&>()->id);
    ASSERT_NE(a2.TypeId(), GetTypeInfo<float>()->id);
    ASSERT_NE(a2.TypeId(), GetTypeInfo<int*>()->id);
}

TEST(AnyTest, TypeInfoTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.Type()->isConst);
    ASSERT_FALSE(a0.Type()->isLValueReference);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.Type()->isConst);
    ASSERT_TRUE(a0.Type()->isLValueReference);
    ASSERT_FALSE(a0.Type()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_EQ(a0.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.Type()->isConst);
    ASSERT_TRUE(a0.Type()->isLValueReference);
    ASSERT_TRUE(a0.Type()->isLValueConstReference);
}

TEST(AnyTest, ConstTypeInfoTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_TRUE(a0.Type()->isConst);
    ASSERT_FALSE(a0.Type()->isLValueReference);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a1.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a1.Type()->isConst);
    ASSERT_TRUE(a1.Type()->isLValueReference);
    ASSERT_FALSE(a1.Type()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a2.Type()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a2.Type()->isConst);
    ASSERT_TRUE(a2.Type()->isLValueReference);
    ASSERT_TRUE(a2.Type()->isLValueConstReference);
}

TEST(AnyTest, RemoveRefTypeTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.RemoveRefType()->isConst);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueReference);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.RemoveRefType()->isConst);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueReference);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_EQ(a0.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_TRUE(a0.RemoveRefType()->isConst);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueReference);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueConstReference);
}

TEST(AnyTest, ConstRemoveRefTypeTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_TRUE(a0.RemoveRefType()->isConst);
    ASSERT_FALSE(a0.RemoveRefType()->isLValueReference);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a1.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a1.RemoveRefType()->isConst);
    ASSERT_FALSE(a1.RemoveRefType()->isLValueReference);
    ASSERT_FALSE(a1.RemoveRefType()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a2.RemoveRefType()->id, GetTypeInfo<float>()->id);
    ASSERT_TRUE(a2.RemoveRefType()->isConst);
    ASSERT_FALSE(a2.RemoveRefType()->isLValueReference);
    ASSERT_FALSE(a2.RemoveRefType()->isLValueConstReference);
}

TEST(AnyTest, AddPointerTypeTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a0.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.AddPointerType()->isConst);
    ASSERT_FALSE(a0.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a0.AddPointerType()->isPointer);
    ASSERT_FALSE(a0.AddPointerType()->isConstPointer);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a0.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.AddPointerType()->isConst);
    ASSERT_FALSE(a0.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a0.AddPointerType()->isPointer);
    ASSERT_FALSE(a0.AddPointerType()->isConstPointer);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_EQ(a0.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a0.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.AddPointerType()->isConst);
    ASSERT_FALSE(a0.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a0.AddPointerType()->isPointer);
    ASSERT_TRUE(a0.AddPointerType()->isConstPointer);
}

TEST(AnyTest, ConstAddPointerTypeTest)
{
    const Any a0 = 1;
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a0.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_EQ(a0.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a0.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a0.AddPointerType()->isConst);
    ASSERT_FALSE(a0.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a0.AddPointerType()->isPointer);
    ASSERT_TRUE(a0.AddPointerType()->isConstPointer);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a1.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_NE(a1.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a1.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a1.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a1.AddPointerType()->isConst);
    ASSERT_FALSE(a1.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a1.AddPointerType()->isPointer);
    ASSERT_FALSE(a1.AddPointerType()->isConstPointer);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_NE(a2.AddPointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_NE(a2.AddPointerType()->id, GetTypeInfo<float*>()->id);
    ASSERT_EQ(a2.AddPointerType()->id, GetTypeInfo<const int*>()->id);
    ASSERT_EQ(a2.AddPointerType()->removePointerType, GetTypeInfo<int>()->id);
    ASSERT_NE(a2.AddPointerType()->removePointerType, GetTypeInfo<float>()->id);
    ASSERT_FALSE(a2.AddPointerType()->isConst);
    ASSERT_FALSE(a2.AddPointerType()->isLValueReference);
    ASSERT_TRUE(a2.AddPointerType()->isPointer);
    ASSERT_TRUE(a2.AddPointerType()->isConstPointer);
}

TEST(AnyTest, RemovePointerTypeTest)
{
    Any a0 = 1;
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueReference);

    int v0 = 1;
    a0 = std::ref(v0);
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_TRUE(a0.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    a0 = std::ref(v1);
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_TRUE(a0.RemovePointerType()->isLValueReference);
    ASSERT_TRUE(a0.RemovePointerType()->isLValueConstReference);

    a0 = &v0;
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueConstReference);

    a0 = &v1;
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_TRUE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueConstReference);
}

TEST(AnyTest, ConstRemovePointerTypeTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a0.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_TRUE(a0.RemovePointerType()->isConst);
    ASSERT_FALSE(a0.RemovePointerType()->isPointer);
    ASSERT_FALSE(a0.RemovePointerType()->isLValueReference);

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a1.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a1.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a1.RemovePointerType()->isConst);
    ASSERT_FALSE(a1.RemovePointerType()->isPointer);
    ASSERT_TRUE(a1.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a1.RemovePointerType()->isLValueConstReference);

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a2.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a2.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a2.RemovePointerType()->isConst);
    ASSERT_FALSE(a2.RemovePointerType()->isPointer);
    ASSERT_TRUE(a2.RemovePointerType()->isLValueReference);
    ASSERT_TRUE(a2.RemovePointerType()->isLValueConstReference);

    const Any a3 = &v0;
    ASSERT_EQ(a3.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a3.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a3.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_FALSE(a3.RemovePointerType()->isConst);
    ASSERT_FALSE(a3.RemovePointerType()->isPointer);
    ASSERT_FALSE(a3.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a3.RemovePointerType()->isLValueConstReference);

    const Any a4 = &v1;
    ASSERT_EQ(a4.RemovePointerType()->id, GetTypeInfo<int>()->id);
    ASSERT_NE(a4.RemovePointerType()->id, GetTypeInfo<float>()->id);
    ASSERT_NE(a4.RemovePointerType()->id, GetTypeInfo<int*>()->id);
    ASSERT_TRUE(a4.RemovePointerType()->isConst);
    ASSERT_FALSE(a4.RemovePointerType()->isPointer);
    ASSERT_FALSE(a4.RemovePointerType()->isLValueReference);
    ASSERT_FALSE(a4.RemovePointerType()->isLValueConstReference);
}

TEST(AnyTest, ResetAndEmptyTest)
{
    Any a0 = 1;
    ASSERT_FALSE(a0.Empty());

    a0.Reset();
    ASSERT_TRUE(a0.Empty());
}

TEST(AnyTest, DataTest)
{
    const Any a0 = 1;
    const Any a1 = 1;
    ASSERT_NE(a0.Data(), a1.Data());

    int v0 = 1;
    const Any a2 = std::ref(v0);
    ASSERT_EQ(&v0, a2.Data());

    const int v1 = 1; // NOLINT
    const Any a3 = std::ref(v1);
    ASSERT_EQ(&v1, a3.Data());
}

TEST(AnyTest, SizeTest)
{
    const Any a0 = 1;
    ASSERT_EQ(a0.Size(), sizeof(int));

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.Size(), sizeof(int));

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.Size(), sizeof(int));
}

TEST(AnyTest, OperatorBoolTest)
{
    bool test = false;
    Any a0 = 1; // NOLINT
    if (a0) {
        test = true;
    }
    ASSERT_TRUE(test);

    test = false;
    a0.Reset();
    if (a0) {
        test = true;
    }
    ASSERT_FALSE(test);
}

TEST(AnyTest, OperatorEqualTest)
{
    const Any a0 = 1;
    const Any a1 = 2;

    const int v0 = 1; // NOLINT
    const Any a2 = std::ref(v0);
    ASSERT_NE(a0, a1);
    ASSERT_EQ(a0, a2);

    const Any a3 = AnyBasicTest { 1, 2 };
    const Any a4 = AnyBasicTest { 2, 3 };
    const Any a5 = AnyBasicTest { 1, 2 };
    ASSERT_NE(a3, a4);
    ASSERT_EQ(a3, a5);

    struct A {
        void Foo() {}
        void Foo() const {}
    };
}

TEST(AnyTest, ToStringTest)
{
    // TODO
}
