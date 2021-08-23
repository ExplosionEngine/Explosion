//
// Created by LiZhen on 2021/8/23.
//

#include <gtest/gtest.h>
#include <Common/Templates/LinkedFreeList.h>

using namespace Explosion;

struct TestData {
    ~TestData()
    {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
    }
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
};

TEST(LinkedFreeListTest, LinkedFreeListTest1)
{
    using List = LinkedFreeList<sizeof(TestData), 64>;
    List list;
    ASSERT_EQ(List::BLOCK_SIZE, 128);
    void* ptr1 = list.Pop();
    TestData* data = new (ptr1) TestData{1, 2, 3, 4};
    void* ptr2 = list.Pop();
    ASSERT_EQ((uint8_t*)ptr2, (uint8_t*)ptr1 + 32);
    void* ptr3 = list.Pop();
    ASSERT_EQ((uint8_t*)ptr3, (uint8_t*)ptr2 + 32);

    ASSERT_EQ(data->a, 1llu);
    ASSERT_EQ(data->b, 2llu);
    ASSERT_EQ(data->c, 3llu);
    ASSERT_EQ(data->d, 4llu);

    void* ptr4 = list.Pop();
    ASSERT_NE((uint8_t*)ptr4, (uint8_t*)ptr3 + 32);

    data->~TestData();
    list.Push(ptr1);

    void* ptr5 = list.Pop();
    ASSERT_EQ((uint8_t*)ptr5, (uint8_t*)ptr1);
    ASSERT_EQ(data->a, 0llu);
    ASSERT_EQ(data->b, 0llu);
    ASSERT_EQ(data->c, 0llu);
    ASSERT_EQ(data->d, 0llu);
}
