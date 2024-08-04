//
// Created by johnk on 2022/9/5.
//

#include <string>
#include <vector>
#include <unordered_map>

#include <Test/Test.h>

#include <Mirror/Mirror.h>
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
    ASSERT_EQ(a1.Ptr(), a0.Ptr());

    Any a2 { a0, AnyPolicy::constRef };
    ASSERT_FALSE(called);
    ASSERT_EQ(a2.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a2.Size(), a0.Size());
    ASSERT_EQ(a2.Ptr(), a0.Ptr());

    Any a3 { a0, AnyPolicy::memoryHolder };
    ASSERT_TRUE(called);
    ASSERT_EQ(a3.Policy(), AnyPolicy::memoryHolder);
    ASSERT_EQ(a3.Size(), a0.Size());
    ASSERT_NE(a3.Ptr(), a0.Ptr());
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

    const float v1 = 2.0f;
    auto r0 = std::ref(v1);
    Any a1 = r0;
    ASSERT_EQ(a1.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a1.As<const float&>(), 2.0f);

    std::vector v2 = { 1, 2, 3 };
    const Any a2 = std::ref(v2);
    ASSERT_EQ(a2.Policy(), AnyPolicy::ref);
    ::Test::AssertVecEq(a2.As<const decltype(v2)&>(), std::vector { 1, 2, 3 });

    const AnyBasicTest v3 { 1, 2.0f };
    const auto r1 = std::ref(v3);
    Any a3 = r1;
    ASSERT_EQ(a3.Policy(), AnyPolicy::constRef);
    ASSERT_EQ(a3.As<AnyBasicTest>(), (AnyBasicTest { 1, 2.0f }));
}

TEST(AnyTest, CopyAssignTest)
{
    // TODO
}

TEST(AnyTest, MoveAssignTest)
{
    // TODO
}

TEST(AnyTest, ValueAssignTest)
{
    // TODO
}

TEST(AnyTest, RefAssignTest)
{
    // TODO
}

TEST(AnyTest, ConvertibleTest)
{
    // TODO
}

TEST(AnyTest, ConstConvertibleTest)
{
    // TODO
}

TEST(AnyTest, AsTest)
{
    // TODO
}

TEST(AnyTest, ConstAsTest)
{
    // TODO
}

TEST(AnyTest, TryAsTest)
{
    // TODO
}

TEST(AnyTest, ConstTryAsTest)
{
    // TODO
}

TEST(AnyTest, GetRefTest)
{
    // TODO
}

TEST(AnyTest, ConstGetRefTest)
{
    // TODO
}

TEST(AnyTest, AsValueTest)
{
    // TODO
}

TEST(AnyTest, PolicyTest)
{
    // TODO
}

TEST(AnyTest, ResetTest)
{
    // TODO
}

TEST(AnyTest, EmptyTest)
{
    // TODO
}

TEST(AnyTest, PtrTest)
{
    // TODO
}

TEST(AnyTest, SizeTest)
{
    // TODO
}

TEST(AnyTest, OperatorBoolTest)
{
    // TODO
}

TEST(AnyTest, OperatorEqualTest)
{
    // TODO
}
