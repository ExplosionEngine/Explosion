//
// Created by LiZhen on 2021/5/9.
//

#include <gtest/gtest.h>
#include <thread>
#include <atomic>
#include <Explosion/Common/Template/RefObject.h>

struct TestObject : public URefObject<TestObject> {};
struct ThreadSafeObject : public UTsRefObject<ThreadSafeObject> {};

TEST(RefObjectTest, ReferenceTest1)
{
    TestObject obj;
    ASSERT_EQ(obj.GetRef(), 0);

    obj.AddRef();
    ASSERT_EQ(obj.GetRef(), 1);

    obj.AddRef();
    ASSERT_EQ(obj.GetRef(), 2);

    obj.RemoveRef();
    ASSERT_EQ(obj.GetRef(), 1);

    obj.RemoveRef();
    ASSERT_EQ(obj.GetRef(), 0);

    obj.RemoveRef();
    ASSERT_EQ(obj.GetRef(), 0);
}

TEST(RefObjectTest, ReferenceTest2)
{
    ThreadSafeObject obj;
    ASSERT_EQ(obj.GetRef(), (uint32_t)0);
    constexpr uint32_t THD_NUM = 10;
    constexpr uint32_t LOOP_NUM = 100;
    std::unique_ptr<std::thread> threads[THD_NUM];
    for (uint32_t i = 0; i < THD_NUM; ++i) {
        threads[i] = std::make_unique<std::thread>([&obj, LOOP_NUM]() {
            for (uint32_t i = 0; i < LOOP_NUM; ++i) {
                obj.AddRef();
            }
        });
    }
    for (uint32_t i = 0; i < THD_NUM; ++i) {
        threads[i]->join();
    }
    ASSERT_EQ(obj.GetRef(), THD_NUM * LOOP_NUM);
}

struct TestObjDev : TestObject {};

using TestObjectPtr = CounterPtr<TestObject>;
using TestObjectDevPtr = CounterPtr<TestObjDev>;

TEST(RefObjectTest, RefPtrTest1)
{
    std::unique_ptr<TestObjDev> src = std::make_unique<TestObjDev>();

    TestObjectDevPtr p(src.get());
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // base class construct
    {
        TestObjectPtr pt(src.get());
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // base class assign
    {
        TestObjectPtr pt = p;
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // base class copy
    {
        TestObjectPtr pt(p);
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // same class construct
    {
        TestObjectDevPtr pt(p);
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // same class assign
    {
        TestObjectDevPtr pt(p);
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);

    // same class copy
    {
        TestObjectDevPtr pt(p);
        ASSERT_EQ(src->GetRef(), (uint32_t)2);
    }
    ASSERT_EQ(src->GetRef(), (uint32_t)1);
}