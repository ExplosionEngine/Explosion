//
// Created by johnk on 2022/9/5.
//

#include <string>
#include <vector>
#include <unordered_map>

#include <Test/Test.h>
#include <AnyTest.h>
#include <Mirror/Mirror.h>
#include <Common/Memory.h>

using namespace Mirror;

AnyDtorTest::AnyDtorTest(bool& inLive)
    : record(false)
    , live(inLive)
{
}

AnyDtorTest::AnyDtorTest(bool& inLive, bool inRecord)
    : record(inRecord)
    , live(inLive)
{
    if (record) {
        live = true;
    }
}

AnyDtorTest::AnyDtorTest(AnyDtorTest&& inOther) noexcept
    : record(true)
    , live(inOther.live)
{
    live = true;
}

AnyDtorTest::~AnyDtorTest()
{
    if (record) {
        live = false;
    }
}

AnyCopyCtorTest::AnyCopyCtorTest(bool& inCopyCtorCalled)
    : copyCtorCalled(inCopyCtorCalled)
{
}

AnyCopyCtorTest::AnyCopyCtorTest(AnyCopyCtorTest&& inOther) noexcept
    : copyCtorCalled(inOther.copyCtorCalled)
{
}

AnyCopyCtorTest::AnyCopyCtorTest(const AnyCopyCtorTest& inOther)
    : copyCtorCalled(inOther.copyCtorCalled)
{
    copyCtorCalled = true;
}

AnyMoveCtorTest::AnyMoveCtorTest(uint8_t& inMoveTime)
    : moveTime(inMoveTime)
{
}

AnyMoveCtorTest::AnyMoveCtorTest(AnyMoveCtorTest&& inOther) noexcept
    : moveTime(inOther.moveTime)
{
    moveTime++;
}

AnyCopyAssignTest::AnyCopyAssignTest()
    : called(false)
{
}

AnyCopyAssignTest::AnyCopyAssignTest(AnyCopyAssignTest&& inOther) noexcept
    : called(inOther.called)
{
}

AnyCopyAssignTest& AnyCopyAssignTest::operator=(const AnyCopyAssignTest& inOther)
{
    called = true;
    return *this;
}

AnyMoveAssignTest::AnyMoveAssignTest()
    : called(false)
{
}

AnyMoveAssignTest::AnyMoveAssignTest(AnyMoveAssignTest&& inOther) noexcept
    : called(inOther.called)
{
}

AnyMoveAssignTest& AnyMoveAssignTest::operator=(AnyMoveAssignTest&& inOther) noexcept
{
    called = true;
    return *this;
}

bool AnyBasicTest::operator==(const AnyBasicTest& inRhs) const
{
    return a == inRhs.a
        && b == inRhs.b;
}

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

    Any a1 { a0, AnyPolicy::nonConstRef };
    ASSERT_FALSE(called);
    ASSERT_EQ(a1.Policy(), AnyPolicy::nonConstRef);
    ASSERT_EQ(a1.MemorySize(), a0.MemorySize());
    ASSERT_EQ(a1.Data(), a0.Data());

    Any a2 { a0, AnyPolicy::constRef };
    ASSERT_FALSE(called);
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a2.MemorySize(), a0.MemorySize());
    ASSERT_EQ(a2.Data(), a0.Data());

    Any a3 { a0, AnyPolicy::memoryHolder };
    ASSERT_TRUE(called);
    ASSERT_EQ(a3.Policy(), AnyPolicy::memoryHolder);
    ASSERT_EQ(a3.MemorySize(), a0.MemorySize());
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
    ASSERT_EQ(a2.As<std::vector<int>>(), (std::vector { 1, 2, 3 }));

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
    ASSERT_EQ(a0.Policy(), AnyPolicy::nonConstRef);
    ASSERT_EQ(a0.As<const int&>(), 1);

    const float v1 = 2.0f; // NOLINT
    auto r0 = std::ref(v1);
    Any a1 = r0;
    ASSERT_EQ(a1.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a1.As<const float&>(), 2.0f);

    std::vector v2 = { 1, 2, 3 };
    const Any a2 = std::ref(v2);
    ASSERT_EQ(a2.Policy(), AnyPolicy::nonConstRef);
    ASSERT_EQ(a2.As<const decltype(v2)&>(), (std::vector { 1, 2, 3 }));

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

TEST(AnyTest, ValueCopyAssignTest)
{
    Any a0 = AnyCopyAssignTest();
    ASSERT_EQ(a0.As<const AnyCopyAssignTest&>().called, false);

    Any a1 = AnyCopyAssignTest();
    a0.CopyAssign(a1);
    ASSERT_EQ(a0.As<const AnyCopyAssignTest&>().called, true);
}

TEST(AnyTest, ValueMoveAssignTest)
{
    Any a0 = AnyMoveAssignTest();
    ASSERT_EQ(a0.As<const AnyMoveAssignTest&>().called, false);

    Any a1 = AnyMoveAssignTest();
    a0.MoveAssign(a1);
    ASSERT_EQ(a0.As<const AnyMoveAssignTest&>().called, true);
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
    ASSERT_EQ(a0.Policy(), AnyPolicy::nonConstRef);
    ASSERT_EQ(a0.As<float>(), 2.0f);
}

TEST(AnyTest, ConvertibleTest)
{
    Any a0 = 1;
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_TRUE(a0.Convertible<int&&>());

    const int v0 = 1; // NOLINT
    a0 = v0;
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_TRUE(a0.Convertible<int&&>());

    int v1 = 1;
    a0 = std::ref(v1);
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_TRUE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_TRUE(a0.Convertible<int&&>());

    a0 = std::ref(v0);
    ASSERT_TRUE(a0.Convertible<int>());
    ASSERT_TRUE(a0.Convertible<const int>());
    ASSERT_FALSE(a0.Convertible<int&>());
    ASSERT_TRUE(a0.Convertible<const int&>());
    ASSERT_TRUE(a0.Convertible<int&&>());

    a0 = 1;
    Any a1 = a0.Ref();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_TRUE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_TRUE(a1.Convertible<int&&>());

    a1 = a1.ConstRef();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_FALSE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_TRUE(a1.Convertible<int&&>());

    a1 = a1.Value();
    ASSERT_TRUE(a1.Convertible<int>());
    ASSERT_TRUE(a1.Convertible<const int>());
    ASSERT_TRUE(a1.Convertible<int&>());
    ASSERT_TRUE(a1.Convertible<const int&>());
    ASSERT_TRUE(a1.Convertible<int&&>());

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

    const Any a7 = a6.Value();
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

TEST(AnyTest, PolyAsTest)
{
    const Common::UniquePtr<AnyBaseClassTest2> v0 = new AnyDerivedClassTest2(1, 2.0f, "3");

    Any a0 = std::ref(*v0);
    const auto& r0 = a0.As<AnyDerivedClassTest2&>();
    ASSERT_EQ(r0.a, 1);
    ASSERT_EQ(r0.b, 2.0f);
    ASSERT_EQ(r0.c, "3");

    Any a1 = v0.Get();
    const auto* p0 = a1.As<AnyDerivedClassTest2*>();
    ASSERT_EQ(p0->a, 1);
    ASSERT_EQ(p0->b, 2.0f);
    ASSERT_EQ(p0->c, "3");
}

TEST(AnyTest, ConstPolyAsTest)
{
    const Common::UniquePtr<AnyBaseClassTest2> v0 = new AnyDerivedClassTest2(1, 2.0f, "3");

    const Any a0 = std::ref(*v0);
    const auto& r0 = a0.As<AnyDerivedClassTest2&>();
    ASSERT_EQ(r0.a, 1);
    ASSERT_EQ(r0.b, 2.0f);
    ASSERT_EQ(r0.c, "3");

    const Any a1 = v0.Get();
    const auto* p0 = a1.As<AnyDerivedClassTest2*>();
    ASSERT_EQ(p0->a, 1);
    ASSERT_EQ(p0->b, 2.0f);
    ASSERT_EQ(p0->c, "3");
}

TEST(AnyTest, GetRefTest)
{
    Any a0 = 1;
    const Any a1 = a0.Ref();
    ASSERT_EQ(a1.Policy(), AnyPolicy::nonConstRef);

    const Any a2 = a0.ConstRef();
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);

    Any a3 = a0.Ref();
    const Any a4 = a3.Ref();
    ASSERT_EQ(a4.Policy(), AnyPolicy::nonConstRef);
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
    ASSERT_EQ(a8.Policy(), AnyPolicy::nonConstRef);
    const Any a9 = a7.ConstRef();
    ASSERT_EQ(a9.Policy(), AnyPolicy::constRef);
}

TEST(AnyTest, ValueTest)
{
    const Any a0 = 1;
    const Any a1 = a0.Value();
    ASSERT_NE(a0.Data(), a1.Data());
    ASSERT_EQ(a1.Policy(), AnyPolicy::memoryHolder);

    int v0 = 2;
    const Any a2 = std::ref(v0);
    const Any a3 = a2.Value();
    ASSERT_EQ(&v0, a2.Data());
    ASSERT_NE(a2.Data(), a3.Data());
    ASSERT_EQ(a3.Policy(), AnyPolicy::memoryHolder);

    const int v1 = 2; // NOLINT
    const Any a4 = std::ref(v1);
    const Any a5 = a4.Value();
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
    ASSERT_EQ(a0.Policy(), AnyPolicy::nonConstRef);

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
    ASSERT_EQ(a0.MemorySize(), sizeof(int));

    int v0 = 1;
    const Any a1 = std::ref(v0);
    ASSERT_EQ(a1.MemorySize(), sizeof(int));

    const int v1 = 1; // NOLINT
    const Any a2 = std::ref(v1);
    ASSERT_EQ(a2.MemorySize(), sizeof(int));
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
    const Any a0 = 1;
    ASSERT_EQ(a0.ToString(), "1");
}

TEST(AnyTest, ArrayTest)
{
    int v0[] = { 1, 2, 3 };
    Any a0 = v0;
    ASSERT_TRUE(a0.IsMemoryHolder());
    ASSERT_EQ(a0.ElementSize(), sizeof(int));
    ASSERT_EQ(a0.MemorySize(), sizeof(v0));
    ASSERT_EQ(a0.ArrayLength(), sizeof(v0) / sizeof(int));

    const Any a1 = a0.At(0);
    ASSERT_TRUE(a1.IsNonConstRef());
    ASSERT_EQ(a1.As<int&>(), 1);

    const Any a2 = v0;
    ASSERT_TRUE(a2.IsMemoryHolder());
    ASSERT_EQ(a2.ElementSize(), sizeof(int));
    ASSERT_EQ(a2.MemorySize(), sizeof(v0));
    ASSERT_EQ(a2.ArrayLength(), sizeof(v0) / sizeof(int));

    const Any a3 = a2[1];
    ASSERT_EQ(a3.As<const int&>(), 2);

    Any a4 = std::move(v0);
    ASSERT_TRUE(a4.IsMemoryHolder());
    ASSERT_EQ(a4.ElementSize(), sizeof(int));
    ASSERT_EQ(a4.MemorySize(), sizeof(v0));
    ASSERT_EQ(a4.ArrayLength(), sizeof(v0) / sizeof(int));

    const Any a5 = a4.At(2);
    ASSERT_EQ(a5.As<int>(), 3);

    const int v1[] = { 1, 2, 3 }; // NOLINT
    Any a6 = std::ref(v1);
    ASSERT_TRUE(a6.IsConstRef());
    ASSERT_EQ(a6.ElementSize(), sizeof(int));
    ASSERT_EQ(a6.MemorySize(), sizeof(v0));
    ASSERT_EQ(a6.ArrayLength(), sizeof(v0) / sizeof(int));
    ASSERT_EQ(a6[1].As<int>(), 2);
}

TEST(AnyTest, StdOptionalViewTest)
{
    Any a0 = std::optional<int> {};
    const StdOptionalView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_FALSE(v0.HasValue());

    std::optional<int> t0 = 1;
    Any a1 = std::ref(t0);
    const StdOptionalView v1(a1.Ref());
    ASSERT_TRUE(v1.HasValue());
    ASSERT_EQ(v1.Value().As<const int&>(), 1);
}

TEST(AnyTest, StdPairViewTest)
{
    Any a0 = std::pair<int, bool> { 1, true };
    const StdPairView v0(a0.Ref());
    ASSERT_EQ(v0.KeyType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.ValueType(), GetTypeInfo<bool>());
    ASSERT_EQ(v0.Key().As<const int&>(), 1);
    ASSERT_EQ(v0.Value().As<const bool&>(), true);
}

TEST(AnyTest, StdArrayViewTest)
{
    Any a0 = std::array<int, 3> { 1, 2, 3 };
    const StdArrayView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.Size(), 3);
    for (auto i = 0; i < 3; i++) {
        ASSERT_EQ(v0.At(i).As<int&>(), i + 1);
        ASSERT_EQ(v0.ConstAt(i).As<const int&>(), i + 1);
    };
}

TEST(AnyTest, StdVectorViewTest)
{
    Any a0 = std::vector<int> { 1, 2, 3 };
    const StdVectorView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.Size(), 3);
    for (auto i = 0; i < 3; i++) {
        ASSERT_EQ(v0.At(i).As<int&>(), i + 1);
        ASSERT_EQ(v0.ConstAt(i).As<const int&>(), i + 1);
    };
    v0.Erase(1);
    ASSERT_EQ(v0.Size(), 2);
    v0.EmplaceBack(Any(5));
    ASSERT_EQ(v0.Size(), 3);
}

TEST(AnyTest, StdListViewTest)
{
    std::list<int> t0 = { 1, 2, 3 };
    Any a0 = std::ref(t0);
    const StdListView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.Size(), 3);

    int count = 0;
    v0.Traverse([&count](const Any& inRef) -> void {
        ASSERT_EQ(inRef.As<int&>(), ++count);
    });

    v0.EmplaceBack(Any(4));
    ASSERT_EQ(v0.Size(), 4);
    v0.EmplaceFront(Any(0));
    ASSERT_EQ(v0.Size(), 5);

    count = 0;
    v0.ConstTraverse([&count](const Any& inRef) -> void {
        ASSERT_EQ(inRef.As<const int&>(), count++);
    });
}

TEST(AnyTest, StdunorderedSetViewTest)
{
    Any a0 = std::unordered_set<int> { 1, 2, 3 };
    const StdUnorderedSetView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.Size(), 3);

    v0.Traverse([](const Any& inRef) -> void {
        const int& valueRef = inRef.As<const int&>();
        ASSERT_TRUE(valueRef >= 1 && valueRef <= 3);
    });

    v0.Emplace(Any(4));
    v0.ConstTraverse([](const Any& inRef) -> void {
        const int& valueRef = inRef.As<const int&>();
        ASSERT_TRUE(valueRef >= 1 && valueRef <= 4);
    });
    v0.Erase(Any(1));
    for (auto i = 2; i <= 4; i++) {
        v0.Contains(Any(std::ref(i)));
    }
}

TEST(AnyTest, StdSetViewTest)
{
    Any a0 = std::set<int> { 1, 2, 3 };
    const StdSetView v0(a0.Ref());
    ASSERT_EQ(v0.ElementType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.Size(), 3);

    v0.Traverse([](const Any& inRef) -> void {
        const int& valueRef = inRef.As<const int&>();
        ASSERT_TRUE(valueRef >= 1 && valueRef <= 3);
    });

    v0.Emplace(Any(4));
    v0.Traverse([](const Any& inRef) -> void {
        const int& valueRef = inRef.As<const int&>();
        ASSERT_TRUE(valueRef >= 1 && valueRef <= 4);
    });
    v0.Erase(Any(1));
    for (auto i = 2; i <= 4; i++) {
        v0.Contains(Any(std::ref(i)));
    }
}

TEST(AnyTest, StdUnorderedMapViewTest)
{
    Any a0 = std::unordered_map<int, bool> {
        { 1, false },
        { 2, true }
    };
    const StdUnorderedMapView v0(a0.Ref());
    ASSERT_EQ(v0.KeyType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.ValueType(), GetTypeInfo<bool>());
    ASSERT_EQ(v0.Size(), 2);
    ASSERT_TRUE(v0.At(Any(2)).As<const bool&>());
    v0.At(Any(1)).As<bool&>() = true;

    v0.GetOrAdd(Any(3)).As<bool&>() = true;
    ASSERT_TRUE(v0.Contains(Any(3)));
    ASSERT_TRUE(v0.At(Any(3)).As<const bool&>());
    v0.Emplace(Any(4), Any(true));

    auto traverser = [](const Any& inKey, const Any& inValue) -> void {
        const int keyNum = inKey.As<const int&>();
        ASSERT_TRUE(keyNum >= 1 && keyNum <= 4);
        ASSERT_TRUE(inValue.As<const bool&>());
    };
    v0.Traverse(traverser);
    v0.ConstTraverse(traverser);
}

TEST(AnyTest, StdMapViewTest)
{
    Any a0 = std::map<int, bool> {
        { 1, false },
        { 2, true }
    };
    const StdMapView v0(a0.Ref());
    ASSERT_EQ(v0.KeyType(), GetTypeInfo<int>());
    ASSERT_EQ(v0.ValueType(), GetTypeInfo<bool>());
    ASSERT_EQ(v0.Size(), 2);
    ASSERT_TRUE(v0.At(Any(2)).As<const bool&>());
    v0.At(Any(1)).As<bool&>() = true;

    v0.GetOrAdd(Any(3)).As<bool&>() = true;
    ASSERT_TRUE(v0.Contains(Any(3)));
    ASSERT_TRUE(v0.At(Any(3)).As<const bool&>());
    v0.Emplace(Any(4), Any(true));

    auto traverser = [](const Any& inKey, const Any& inValue) -> void {
        const int keyNum = inKey.As<const int&>();
        ASSERT_TRUE(keyNum >= 1 && keyNum <= 4);
        ASSERT_TRUE(inValue.As<const bool&>());
    };
    v0.Traverse(traverser);
    v0.ConstTraverse(traverser);
}

TEST(AnyTest, StdTupleViewTest)
{
    Any a0 = std::tuple<int, bool, std::string> { 1, true, "2" };
    const StdTupleView v0(a0.Ref());
    ASSERT_EQ(v0.Size(), 3);
    ASSERT_EQ(v0.ElementType(0), GetTypeInfo<int>());
    ASSERT_EQ(v0.ElementType(1), GetTypeInfo<bool>());
    ASSERT_EQ(v0.ElementType(2), GetTypeInfo<std::string>());
    ASSERT_EQ(v0.Get(0).As<const int&>(), 1);
    ASSERT_TRUE(v0.Get(1).As<const bool&>());
    ASSERT_EQ(v0.Get(2).As<const std::string&>(), "2");

    int count = 0;
    v0.Traverse([&](const Any& inRef, size_t inIndex) -> void {
        if (inIndex == 0) {
            ASSERT_EQ(inRef.As<const int&>(), 1);
        } else if (inIndex == 1) {
            ASSERT_TRUE(inRef.As<const bool&>());
        } else if (inIndex == 2) {
            ASSERT_EQ(inRef.As<const std::string&>(), "2");
        }
        count++;
    });
    ASSERT_EQ(count, 3);
}

TEST(AnyTest, StdTupleViewCreateAndConstTraverseTest)
{
    Any a0 = std::tuple<int, bool, std::string> { 1, true, "hello" };
    const StdTupleView v0(a0.Ref());

    const Any e0 = v0.CreateElement(0);
    ASSERT_EQ(e0.Type()->id, GetTypeInfo<int>()->id);
    const Any e1 = v0.CreateElement(2);
    ASSERT_EQ(e1.Type()->id, GetTypeInfo<std::string>()->id);

    int count = 0;
    v0.ConstTraverse([&](const Any& inRef, size_t inIndex) -> void {
        if (inIndex == 0) {
            ASSERT_EQ(inRef.As<const int&>(), 1);
        } else if (inIndex == 2) {
            ASSERT_EQ(inRef.As<const std::string&>(), "hello");
        }
        count++;
    });
    ASSERT_EQ(count, 3);
}

TEST(AnyTest, StdVariantViewTest)
{
    using Var = std::variant<int, std::string, float>;

    Var v { 42 };
    Any a0 = std::ref(v);
    const StdVariantView view(a0.Ref());

    ASSERT_EQ(view.TypeNum(), 3);
    ASSERT_EQ(view.TypeByIndex(0)->id, GetTypeInfo<int>()->id);
    ASSERT_EQ(view.TypeByIndex(1)->id, GetTypeInfo<std::string>()->id);
    ASSERT_EQ(view.TypeByIndex(2)->id, GetTypeInfo<float>()->id);

    ASSERT_EQ(view.Index(), 0);
    ASSERT_EQ(view.GetElement(0).As<const int&>(), 42);
    ASSERT_EQ(view.GetConstElement(0).As<const int&>(), 42);

    bool visited = false;
    view.Visit([&](const Any& inAny) -> void {
        ASSERT_EQ(inAny.As<const int&>(), 42);
        visited = true;
    });
    ASSERT_TRUE(visited);
}

TEST(AnyTest, StdVariantViewEmplaceAndCreateTest)
{
    using Var = std::variant<int, std::string, float>;

    Var v;
    Any a0 = std::ref(v);
    const StdVariantView view(a0.Ref());

    const Any newStr = view.CreateElement(1);
    ASSERT_EQ(newStr.Type()->id, GetTypeInfo<std::string>()->id);

    Any tmp = std::string("hello");
    view.Emplace(1, tmp);
    ASSERT_EQ(view.Index(), 1);
    ASSERT_EQ(view.GetConstElement(1).As<const std::string&>(), "hello");

    Any tmpFloat = 3.14f;
    view.Emplace(2, tmpFloat);
    ASSERT_EQ(view.Index(), 2);
    ASSERT_EQ(view.GetConstElement(2).As<const float&>(), 3.14f);
}

TEST(AnyTest, StdOptionalViewMutationTest)
{
    std::optional<int> v;
    Any a0 = std::ref(v);
    const StdOptionalView view(a0.Ref());
    ASSERT_FALSE(view.HasValue());

    Any tmp = 7;
    view.Emplace(tmp);
    ASSERT_TRUE(view.HasValue());
    ASSERT_EQ(view.Value().As<const int&>(), 7);

    view.Reset();
    ASSERT_FALSE(view.HasValue());

    view.EmplaceDefault();
    ASSERT_TRUE(view.HasValue());
    ASSERT_EQ(view.Value().As<const int&>(), 0);

    ASSERT_EQ(view.ConstValue().As<const int&>(), 0);
}

TEST(AnyTest, StdPairViewMutationTest)
{
    std::pair<int, std::string> p { 1, "abc" };
    Any a0 = std::ref(p);
    const StdPairView view(a0.Ref());

    ASSERT_EQ(view.Key().As<const int&>(), 1);
    ASSERT_EQ(view.Value().As<const std::string&>(), "abc");
    ASSERT_EQ(view.ConstKey().As<const int&>(), 1);
    ASSERT_EQ(view.ConstValue().As<const std::string&>(), "abc");

    view.Key().As<int&>() = 100;
    view.Value().As<std::string&>() = "xyz";
    ASSERT_EQ(p.first, 100);
    ASSERT_EQ(p.second, "xyz");

    view.Reset();
    ASSERT_EQ(p.first, 0);
    ASSERT_EQ(p.second, "");
}

TEST(AnyTest, StdVectorViewExtraTest)
{
    Any a0 = std::vector<int> {};
    const StdVectorView view(a0.Ref());
    ASSERT_EQ(view.Size(), 0);

    view.Reserve(16);
    ASSERT_EQ(view.Size(), 0);

    view.Resize(3);
    ASSERT_EQ(view.Size(), 3);
    view.At(0).As<int&>() = 1;
    view.At(1).As<int&>() = 2;
    view.At(2).As<int&>() = 3;
    ASSERT_EQ(view.At(2).As<const int&>(), 3);

    view.EmplaceDefaultBack();
    ASSERT_EQ(view.Size(), 4);
    ASSERT_EQ(view.At(3).As<const int&>(), 0);

    view.Clear();
    ASSERT_EQ(view.Size(), 0);
}

TEST(AnyTest, StdListViewConstTraverseTest)
{
    std::list<int> t0 = { 10, 20, 30 };
    Any a0 = std::ref(t0);
    const StdListView view(a0.Ref());
    ASSERT_EQ(view.Size(), 3);

    std::vector<int> visited;
    view.ConstTraverse([&](const Any& inRef) -> void {
        visited.push_back(inRef.As<const int&>());
    });
    ASSERT_EQ(visited, (std::vector<int> { 10, 20, 30 }));

    view.EmplaceDefaultFront();
    view.EmplaceDefaultBack();
    ASSERT_EQ(view.Size(), 5);

    view.Clear();
    ASSERT_EQ(view.Size(), 0);
}

TEST(AnyTest, StdSetViewExtraTest)
{
    Any a0 = std::set<int> { 1, 2, 3 };
    const StdSetView view(a0.Ref());
    ASSERT_TRUE(view.Contains(Any(1)));
    ASSERT_TRUE(view.Contains(Any(2)));
    ASSERT_FALSE(view.Contains(Any(99)));

    view.Erase(Any(2));
    ASSERT_FALSE(view.Contains(Any(2)));
    ASSERT_EQ(view.Size(), 2);

    const Any created = view.CreateElement();
    ASSERT_EQ(created.Type()->id, GetTypeInfo<int>()->id);

    view.Clear();
    ASSERT_EQ(view.Size(), 0);
}

TEST(AnyTest, StdUnorderedSetViewExtraTest)
{
    Any a0 = std::unordered_set<int> {};
    const StdUnorderedSetView view(a0.Ref());

    view.Reserve(16);
    view.Emplace(Any(1));
    view.Emplace(Any(2));
    ASSERT_EQ(view.Size(), 2);
    ASSERT_TRUE(view.Contains(Any(1)));
    ASSERT_FALSE(view.Contains(Any(99)));

    view.Erase(Any(1));
    ASSERT_FALSE(view.Contains(Any(1)));

    const Any created = view.CreateElement();
    ASSERT_EQ(created.Type()->id, GetTypeInfo<int>()->id);

    view.Clear();
    ASSERT_EQ(view.Size(), 0);
}

TEST(AnyTest, StdMapViewExtraTest)
{
    std::map<int, std::string> m = { { 1, "a" }, { 2, "b" } };
    Any a0 = std::ref(m);
    const StdMapView view(a0.Ref());
    ASSERT_EQ(view.Size(), 2);
    ASSERT_TRUE(view.Contains(Any(1)));
    ASSERT_FALSE(view.Contains(Any(99)));
    ASSERT_EQ(view.ConstAt(Any(1)).As<const std::string&>(), "a");

    view.Erase(Any(1));
    ASSERT_FALSE(view.Contains(Any(1)));
    ASSERT_EQ(view.Size(), 1);

    const Any newKey = view.CreateKey();
    const Any newValue = view.CreateValue();
    ASSERT_EQ(newKey.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_EQ(newValue.Type()->id, GetTypeInfo<std::string>()->id);

    view.Clear();
    ASSERT_EQ(view.Size(), 0);
}

TEST(AnyTest, StdUnorderedMapViewExtraTest)
{
    Any a0 = std::unordered_map<int, std::string> {};
    const StdUnorderedMapView view(a0.Ref());

    view.Reserve(16);
    view.Emplace(Any(1), Any(std::string("a")));
    ASSERT_EQ(view.Size(), 1);
    ASSERT_TRUE(view.Contains(Any(1)));

    view.Erase(Any(1));
    ASSERT_FALSE(view.Contains(Any(1)));

    const Any newKey = view.CreateKey();
    const Any newValue = view.CreateValue();
    ASSERT_EQ(newKey.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_EQ(newValue.Type()->id, GetTypeInfo<std::string>()->id);
}

TEST(AnyTest, HasTemplateViewTest)
{
    Any a0 = std::vector<int> { 1, 2, 3 };
    ASSERT_TRUE(a0.HasTemplateView());
    ASSERT_EQ(a0.GetTemplateViewId(), StdVectorView::id);

    Any a1 = 1;
    ASSERT_FALSE(a1.HasTemplateView());

    ASSERT_TRUE(a0.CanAsTemplateView<StdVectorView>());
    ASSERT_FALSE(a0.CanAsTemplateView<StdListView>());
}

TEST(AnyTest, OperatorIndexTest)
{
    int v0[] = { 7, 8, 9 };
    Any a0 = v0;
    ASSERT_EQ(a0[0].As<int&>(), 7);
    ASSERT_EQ(a0[1].As<int&>(), 8);

    a0[2].As<int&>() = 100;
    ASSERT_EQ(a0[2].As<const int&>(), 100);

    const Any a1 = v0;
    ASSERT_EQ(a1[0].As<const int&>(), 7);
}

TEST(AnyTest, MemberFuncMoveAssignTest)
{
    Any a0 = AnyMoveAssignTest();
    Any a1 = AnyMoveAssignTest();
    ASSERT_FALSE(a0.As<const AnyMoveAssignTest&>().called);

    a0.MoveAssign(std::move(a1));
    ASSERT_TRUE(a0.As<const AnyMoveAssignTest&>().called);
}

TEST(AnyTest, ForwardAsAnyTest)
{
    Any a0 = ForwardAsAny(42);
    ASSERT_TRUE(a0.IsMemoryHolder());
    ASSERT_EQ(a0.As<int>(), 42);

    int v0 = 7;
    Any a1 = ForwardAsAny(v0);
    ASSERT_TRUE(a1.IsNonConstRef());
    ASSERT_EQ(a1.As<int&>(), 7);

    const int v1 = 8; // NOLINT
    Any a2 = ForwardAsAny(v1);
    ASSERT_TRUE(a2.IsConstRef());
    ASSERT_EQ(a2.As<const int&>(), 8);
}

TEST(AnyTest, ForwardAsAnyByValueTest)
{
    int v0 = 7;
    Any a0 = ForwardAsAnyByValue(v0);
    ASSERT_TRUE(a0.IsMemoryHolder());
    ASSERT_EQ(a0.As<int>(), 7);
    ASSERT_NE(a0.Data(), &v0);

    Any a1 = ForwardAsAnyByValue(42);
    ASSERT_TRUE(a1.IsMemoryHolder());
    ASSERT_EQ(a1.As<int>(), 42);
}

TEST(AnyTest, ForwardAsArgListTest)
{
    int v0 = 1;
    const float v1 = 2.0f; // NOLINT

    ArgumentList args = ForwardAsArgList(v0, v1, 3.0);
    ASSERT_EQ(args.size(), 3);
    ASSERT_TRUE(args[0].IsNonConstRef());
    ASSERT_TRUE(args[1].IsConstRef());
    ASSERT_TRUE(args[2].IsMemoryHolder());

    ASSERT_EQ(args[0].As<int&>(), 1);
    ASSERT_EQ(args[1].As<const float&>(), 2.0f);
    ASSERT_EQ(args[2].As<const double&>(), 3.0);
}

TEST(AnyTest, ForwardAsArgListByValueTest)
{
    int v0 = 1;
    ArgumentList args = ForwardAsArgListByValue(v0, 2.0f);
    ASSERT_EQ(args.size(), 2);
    ASSERT_TRUE(args[0].IsMemoryHolder());
    ASSERT_TRUE(args[1].IsMemoryHolder());
    ASSERT_EQ(args[0].As<const int&>(), 1);
    ASSERT_EQ(args[1].As<const float&>(), 2.0f);
}

TEST(AnyTest, ArgumentBasicTest)
{
    Any a0 = 7;
    Argument arg0 = a0; // NOLINT
    ASSERT_FALSE(arg0.IsConstRef());
    ASSERT_TRUE(arg0.IsMemoryHolder());
    ASSERT_EQ(arg0.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_EQ(arg0.As<const int&>(), 7);

    int v0 = 9;
    Any a1 = std::ref(v0);
    Argument arg1 = a1; // NOLINT
    ASSERT_TRUE(arg1.IsRef());
    ASSERT_TRUE(arg1.IsNonConstRef());
    ASSERT_EQ(*arg1.TryAs<int>(), 9);

    const Any a2 = 10;
    Argument arg2 = a2; // NOLINT
    ASSERT_EQ(arg2.As<int>(), 10);

    Argument arg3 = Any(20); // NOLINT
    ASSERT_EQ(arg3.As<int>(), 20);
}

TEST(AnyTest, ArgumentTypeInspectTest)
{
    int v0 = 9;
    Any a0 = std::ref(v0);
    Argument arg0 = a0; // NOLINT
    ASSERT_EQ(arg0.Type()->id, GetTypeInfo<int&>()->id);
    ASSERT_EQ(arg0.RemoveRefType()->id, GetTypeInfo<int>()->id);
    ASSERT_EQ(arg0.AddPointerType()->id, GetTypeInfo<int*>()->id);

    Any a1 = &v0;
    Argument arg1 = a1; // NOLINT
    ASSERT_EQ(arg1.RemovePointerType()->id, GetTypeInfo<int>()->id);
}

TEST(AnyTest, ConvertibleFreeFunctionTest)
{
    const TypeInfoCompact intType { GetTypeInfo<int>(), GetTypeInfo<int>(), GetTypeInfo<int>() };
    const TypeInfoCompact intRefType { GetTypeInfo<int&>(), GetTypeInfo<int>(), GetTypeInfo<int>() };
    const TypeInfoCompact intConstRefType { GetTypeInfo<const int&>(), GetTypeInfo<const int>(), GetTypeInfo<int>() };
    const TypeInfoCompact intPtrType { GetTypeInfo<int*>(), GetTypeInfo<int*>(), GetTypeInfo<int>() };
    const TypeInfoCompact constIntPtrType { GetTypeInfo<const int*>(), GetTypeInfo<const int*>(), GetTypeInfo<const int>() };
    const TypeInfoCompact floatType { GetTypeInfo<float>(), GetTypeInfo<float>(), GetTypeInfo<float>() };

    ASSERT_TRUE(Mirror::Convertible(intType, intType, nullptr));
    ASSERT_TRUE(Mirror::Convertible(intType, intRefType, nullptr));
    ASSERT_TRUE(Mirror::Convertible(intType, intConstRefType, nullptr));
    ASSERT_FALSE(Mirror::Convertible(intType, floatType, nullptr));

    ASSERT_TRUE(Mirror::PointerConvertible(intPtrType, intPtrType));
    ASSERT_TRUE(Mirror::PointerConvertible(intPtrType, constIntPtrType));
    ASSERT_FALSE(Mirror::PointerConvertible(constIntPtrType, intPtrType));
    ASSERT_FALSE(Mirror::PointerConvertible(intType, intPtrType));
}

TEST(AnyTest, GetDynamicClassTest)
{
    AnyDerivedClassTest derived {};
    Any a0 = std::ref(derived);
    const Class* derivedClass = a0.GetDynamicClass();
    ASSERT_TRUE(derivedClass != nullptr);
    ASSERT_EQ(derivedClass->GetName(), "AnyDerivedClassTest");
}

TEST(AnyTest, ResetClearsTypeTest)
{
    Any a0 = 1;
    ASSERT_FALSE(a0.Empty());
    ASSERT_TRUE(a0);

    a0.Reset();
    ASSERT_TRUE(a0.Empty());
    ASSERT_FALSE(a0);

    a0 = 2.0f;
    ASSERT_FALSE(a0.Empty());
    ASSERT_EQ(a0.TypeId(), GetTypeInfo<float>()->id);
}

TEST(AnyTest, OperatorEqualEmptyTest)
{
    Any a0;
    Any a1;
    ASSERT_TRUE(a0.Empty());
    ASSERT_TRUE(a1.Empty());
    // Both empty Anys do not call rtti->equal, but TypeId() asserts on rtti != nullptr,
    // so we only check non-empty equality cases here. The real semantics of operator==
    // are exercised in OperatorEqualTest.
    Any a2 = 3;
    Any a3 = 3;
    Any a4 = 4;
    ASSERT_TRUE(a2 == a3);
    ASSERT_TRUE(a2 != a4);
}

TEST(AnyTest, ArrayAssignTest)
{
    int v0[] = { 1, 2, 3 };
    Any a0;
    a0 = v0;
    ASSERT_TRUE(a0.IsMemoryHolder());
    ASSERT_EQ(a0.ArrayLength(), 3);
    ASSERT_EQ(a0.At(0).As<int&>(), 1);

    int v1[] = { 4, 5 };
    a0 = std::move(v1);
    ASSERT_TRUE(a0.IsMemoryHolder());
    ASSERT_EQ(a0.ArrayLength(), 2);
    ASSERT_EQ(a0.At(1).As<int&>(), 5);

    const int v2[] = { 7, 8, 9 }; // NOLINT
    Any a1;
    a1 = std::ref(v2);
    ASSERT_TRUE(a1.IsConstRef());
    ASSERT_EQ(a1.ArrayLength(), 3);
    ASSERT_EQ(a1.At(0).As<const int&>(), 7);
}

TEST(AnyTest, CopyMoveAssignFromRvalueTest)
{
    // CopyAssign(Any&&) — value-category overload: takes an rvalue Any and copies its content.
    Any a0 = AnyCopyAssignTest();
    ASSERT_FALSE(a0.As<const AnyCopyAssignTest&>().called);

    a0.CopyAssign(Any(AnyCopyAssignTest()));
    ASSERT_TRUE(a0.As<const AnyCopyAssignTest&>().called);

    // MoveAssign(const Any&) — needs the source to be a non-const ref.
    AnyMoveAssignTest tmp;
    Any a1 = AnyMoveAssignTest();
    ASSERT_FALSE(a1.As<const AnyMoveAssignTest&>().called);
    Any srcRef = std::ref(tmp);
    const Any& constSrcRef = srcRef;
    a1.MoveAssign(constSrcRef);
    ASSERT_TRUE(a1.As<const AnyMoveAssignTest&>().called);
}

TEST(AnyTest, ConstAnyCopyAssignTest)
{
    // The const-qualified CopyAssign requires `this` to be a non-const ref Any.
    AnyCopyAssignTest backing;
    Any holder = std::ref(backing);
    const Any& holderRef = holder;
    ASSERT_FALSE(backing.called);

    Any rhs = AnyCopyAssignTest();
    holderRef.CopyAssign(rhs);
    ASSERT_TRUE(backing.called);
}

TEST(AnyTest, ConstAnyMoveAssignTest)
{
    AnyMoveAssignTest backing;
    Any holder = std::ref(backing);
    const Any& holderRef = holder;
    ASSERT_FALSE(backing.called);

    Any rhs = AnyMoveAssignTest();
    holderRef.MoveAssign(rhs);
    ASSERT_TRUE(backing.called);
}

TEST(AnyTest, ArgumentAssignmentTest)
{
    Any a0 = 1;
    Any a1 = 2;

    Argument arg = a0;     // NOLINT
    ASSERT_EQ(arg.As<int>(), 1);

    arg = a1;              // operator=(Any&)
    ASSERT_EQ(arg.As<int>(), 2);

    const Any a2 = 3;
    arg = a2;              // operator=(const Any&)
    ASSERT_EQ(arg.As<int>(), 3);

    arg = Any(4);          // operator=(Any&&)
    ASSERT_EQ(arg.As<int>(), 4);
}

TEST(AnyTest, ArgumentGetDynamicClassTest)
{
    AnyDerivedClassTest d {};
    Any a0 = std::ref(d);
    Argument arg = a0; // NOLINT
    const Class* dynClass = arg.GetDynamicClass();
    ASSERT_NE(dynClass, nullptr);
    ASSERT_EQ(dynClass->GetName(), "AnyDerivedClassTest");
}

TEST(AnyTest, PolymorphismConvertibleTest)
{
    AnyDerivedClassTest derived {};
    const Class* derivedClass = Class::Find<AnyDerivedClassTest>();
    ASSERT_NE(derivedClass, nullptr);

    // Pointer up-cast: derived* → base*.
    {
        const TypeInfoCompact srcType {
            GetTypeInfo<AnyDerivedClassTest*>(),
            GetTypeInfo<AnyDerivedClassTest*>(),
            GetTypeInfo<AnyDerivedClassTest>()
        };
        const TypeInfoCompact dstType {
            GetTypeInfo<AnyBaseClassTest*>(),
            GetTypeInfo<AnyBaseClassTest*>(),
            GetTypeInfo<AnyBaseClassTest>()
        };
        ASSERT_TRUE(Mirror::PolymorphismConvertible(srcType, dstType, nullptr));
    }

    // Reference up-cast: derived& → base&.
    {
        const TypeInfoCompact srcType {
            GetTypeInfo<AnyDerivedClassTest&>(),
            GetTypeInfo<AnyDerivedClassTest>(),
            GetTypeInfo<AnyDerivedClassTest>()
        };
        const TypeInfoCompact dstType {
            GetTypeInfo<AnyBaseClassTest&>(),
            GetTypeInfo<AnyBaseClassTest>(),
            GetTypeInfo<AnyBaseClassTest>()
        };
        ASSERT_TRUE(Mirror::PolymorphismConvertible(srcType, dstType, nullptr));
    }

    // Down-cast without dynamic type info: should fail.
    {
        const TypeInfoCompact srcType {
            GetTypeInfo<AnyBaseClassTest*>(),
            GetTypeInfo<AnyBaseClassTest*>(),
            GetTypeInfo<AnyBaseClassTest>()
        };
        const TypeInfoCompact dstType {
            GetTypeInfo<AnyDerivedClassTest*>(),
            GetTypeInfo<AnyDerivedClassTest*>(),
            GetTypeInfo<AnyDerivedClassTest>()
        };
        ASSERT_FALSE(Mirror::PolymorphismConvertible(srcType, dstType, nullptr));
        // With dynamic class info pointing to derived, down-cast becomes legal.
        ASSERT_TRUE(Mirror::PolymorphismConvertible(srcType, dstType, derivedClass));
    }

    // Unrelated types are never polymorphism-convertible.
    {
        const TypeInfoCompact srcType {
            GetTypeInfo<int*>(),
            GetTypeInfo<int*>(),
            GetTypeInfo<int>()
        };
        const TypeInfoCompact dstType {
            GetTypeInfo<float*>(),
            GetTypeInfo<float*>(),
            GetTypeInfo<float>()
        };
        ASSERT_FALSE(Mirror::PolymorphismConvertible(srcType, dstType, nullptr));
    }
}

TEST(AnyTest, NonRefArgumentVariantTest)
{
    // Build an Argument that owns its Any (operator=(Any&&) path).
    Argument arg;
    arg = Any(123);
    ASSERT_EQ(arg.As<int>(), 123);
    ASSERT_EQ(arg.Type()->id, GetTypeInfo<int>()->id);
    ASSERT_TRUE(arg.IsMemoryHolder());
}

TEST(AnyTest, ClassCastViaPolyClassTest)
{
    // AnyDerivedClassTest2 / AnyBaseClassTest2 use EPolyClassBody (virtual GetClass);
    // PolyAsTest above already exercises As<Derived*>/As<Derived&>, so Class::Cast
    // should round-trip cleanly here.
    AnyDerivedClassTest2 derived(1, 2.0f, "3");
    const Class& dc = Class::Get<AnyDerivedClassTest2>();

    // Ref cast via Cast.
    Any anyRef = std::ref(derived);
    Any castRef = dc.Cast(anyRef);
    ASSERT_FALSE(castRef.Empty());

    // Pointer cast via Cast.
    Any anyPtr = &derived;
    Any castPtr = dc.Cast(anyPtr);
    ASSERT_FALSE(castPtr.Empty());
}
