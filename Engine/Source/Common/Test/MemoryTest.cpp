//
// Created by johnk on 2023/4/14.
//

#include <Test/Test.h>

#include <Common/Memory.h>
using namespace Common;

struct TestStruct {
    uint32_t value;
    bool& live;

    TestStruct(const uint32_t inValue, bool& inLive) : value(inValue), live(inLive)
    {
        live = true;
    }

    ~TestStruct()
    {
        live = false;
    }
};

struct ChildTestStruct : TestStruct {
    uint32_t cValue;

    ChildTestStruct(const uint32_t inValue, const uint32_t inCValue, bool& inLive) : TestStruct(inValue, inLive), cValue(inCValue)
    {
    }
};

TEST(MemoryTest, UniqueRefTest) // NOLINT
{
    bool live;
    {
        const UniquePtr ref = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref->value, 1);
    }
    ASSERT_EQ(live, false);

    {
        UniquePtr ref = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref->value, 1);

        UniquePtr<TestStruct> movedRef = std::move(ref);
        ASSERT_EQ(live, true);
    }
    ASSERT_EQ(live, false);
}

TEST(MemoryTest, SharedRefTest) // NOLINT
{
    bool live;
    {
        const SharedPtr ref1 = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref1->value, 1);
        ASSERT_EQ(ref1.RefCount(), 1);

        SharedPtr<TestStruct> ref2 = ref1;
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref2->value, 1);
        ASSERT_EQ(ref2.RefCount(), 2);

        const SharedPtr<TestStruct> ref3 = std::move(ref2);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref3->value, 1);
        ASSERT_EQ(ref1.RefCount(), 2);
        ASSERT_EQ(ref3.RefCount(), 2);
    }
    ASSERT_EQ(live, false);
}

TEST(MemoryTest, WeakRefTest) // NOLINT
{
    bool live;
    SharedPtr ref = new TestStruct(1, live);
    ASSERT_EQ(live, true);
    ASSERT_EQ(ref->value, 1);
    ASSERT_EQ(ref.RefCount(), 1);

    const WeakPtr<TestStruct> weakRef = ref;
    ASSERT_EQ(live, true);
    ASSERT_EQ(weakRef.Expired(), false);
    ASSERT_EQ(ref.RefCount(), 1);

    {
        const auto lockRef = weakRef.Lock();
        ASSERT_EQ(live, true);
        ASSERT_EQ(lockRef->value, 1);
        ASSERT_EQ(lockRef.RefCount(), 2);
    }

    ASSERT_EQ(ref.RefCount(), 1);
    ref.Reset();
    ASSERT_EQ(live, false);
    ASSERT_EQ(weakRef.Expired(), true);

    {
        const auto lockRef = weakRef.Lock();
        ASSERT_EQ(lockRef, nullptr);
    }
}

TEST(MemoryTest, WeakRefDeriveTest) // NOLINT
{
    bool live;
    SharedPtr ref = new ChildTestStruct(1, 2, live);
    ASSERT_EQ(live, true);
    ASSERT_EQ(ref->value, 1);
    ASSERT_EQ(ref->cValue, 2);
    ASSERT_EQ(ref.RefCount(), 1);

    const WeakPtr<TestStruct> weakRef = ref;
    ASSERT_EQ(live, true);
    ASSERT_EQ(weakRef.Expired(), false);
    ASSERT_EQ(ref.RefCount(), 1);

    {
        const SharedPtr<ChildTestStruct> lockRef = weakRef.Lock().StaticCast<ChildTestStruct>();
        ASSERT_EQ(lockRef != nullptr, true);
        ASSERT_EQ(live, true);
        ASSERT_EQ(lockRef->value, 1);
        ASSERT_EQ(lockRef->cValue, 2);
        ASSERT_EQ(ref.RefCount(), 2);
        ASSERT_EQ(lockRef.RefCount(), 2);
    }

    ASSERT_EQ(ref.RefCount(), 1);
    ref.Reset();
    ASSERT_EQ(live, false);
    ASSERT_EQ(weakRef.Expired(), true);
}
