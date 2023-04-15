//
// Created by johnk on 2023/4/14.
//

#include <gtest/gtest.h>

#include <Common/Memory.h>
using namespace Common;

struct TestStruct {
    uint32_t value;
    bool& live;

    TestStruct(uint32_t inValue, bool& inLive) : value(inValue), live(inLive)
    {
        live = true;
    }

    ~TestStruct()
    {
        live = false;
    }
};

TEST(MemoryTest, UniqueRefTest)
{
    bool live;
    {
        UniqueRef<TestStruct> ref = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref->value, 1);
    }
    ASSERT_EQ(live, false);

    {
        UniqueRef<TestStruct> ref = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref->value, 1);

        UniqueRef<TestStruct> movedRef = std::move(ref);
        ASSERT_EQ(live, true);
    }
    ASSERT_EQ(live, false);
}

TEST(MemoryTest, SharedRefTest)
{
    bool live;
    {
        SharedRef<TestStruct> ref1 = new TestStruct(1, live);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref1->value, 1);
        ASSERT_EQ(ref1.RefCount(), 1);

        SharedRef<TestStruct> ref2 = ref1;
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref2->value, 1);
        ASSERT_EQ(ref2.RefCount(), 2);

        SharedRef<TestStruct> ref3 = std::move(ref2);
        ASSERT_EQ(live, true);
        ASSERT_EQ(ref3->value, 1);
        ASSERT_EQ(ref1.RefCount(), 2);
        ASSERT_EQ(ref3.RefCount(), 2);
    }
    ASSERT_EQ(live, false);
}

TEST(MemoryTest, WeakRefTest)
{
    bool live;
    SharedRef<TestStruct> ref = new TestStruct(1, live);
    ASSERT_EQ(live, true);
    ASSERT_EQ(ref->value, 1);
    ASSERT_EQ(ref.RefCount(), 1);

    WeakRef<TestStruct> weakRef = ref;
    ASSERT_EQ(live, true);
    ASSERT_EQ(weakRef.Expired(), false);
    ASSERT_EQ(ref.RefCount(), 1);

    {
        auto lockRef = weakRef.Lock();
        ASSERT_EQ(live, true);
        ASSERT_EQ(lockRef->value, 1);
        ASSERT_EQ(lockRef.RefCount(), 2);
    }

    ASSERT_EQ(ref.RefCount(), 1);
    ref.Reset();
    ASSERT_EQ(live, false);
    ASSERT_EQ(weakRef.Expired(), true);

    {
        auto lockRef = weakRef.Lock();
        ASSERT_EQ(lockRef, nullptr);
    }
}
