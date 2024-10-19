//
// Created by johnk on 2023/12/5.
//

#include <Test/Test.h>
#include <Common/Container.h>
using namespace Common;

enum class ConstructType : uint8_t {
    cDefault,
    cCopy,
    cMove,
    max
};

struct CopyAndMoveTest {
    ConstructType constructType;
    bool copyAssigned;
    bool moveAssigned;

    CopyAndMoveTest()
        : constructType(ConstructType::cDefault)
        , copyAssigned(false)
        , moveAssigned(false)
    {
    }

    CopyAndMoveTest(ConstructType inType, bool inCopyAssigned, bool inMoveAssigned)
        : constructType(inType)
        , copyAssigned(inCopyAssigned)
        , moveAssigned(inMoveAssigned)
    {
    }

    CopyAndMoveTest(const CopyAndMoveTest& inOther)
        : constructType(ConstructType::cCopy)
        , copyAssigned(false)
        , moveAssigned(false)
    {
    }

    CopyAndMoveTest(CopyAndMoveTest&& inOther) noexcept
        : constructType(ConstructType::cMove)
        , copyAssigned(false)
        , moveAssigned(false)
    {
    }

    CopyAndMoveTest& operator=(const CopyAndMoveTest& inOther)
    {
        copyAssigned = true;
        return *this;
    }

    CopyAndMoveTest& operator=(CopyAndMoveTest&& inOther) noexcept
    {
        moveAssigned = true;
        return *this;
    }

    bool operator==(const CopyAndMoveTest& inOther) const
    {
        return constructType == inOther.constructType
            && copyAssigned == inOther.copyAssigned
            && moveAssigned == inOther.moveAssigned;
    }
};

TEST(ContainerTest, VectorSwapDeleteTest)
{
    std::vector vec0 = { 1, 2, 3, 4, 5 };
    const auto index = VectorUtils::SwapWithLastAndDelete(vec0, 2);
    ASSERT_EQ(index, 2);
    ASSERT_EQ(vec0, (std::vector { 1, 2, 5, 4 }));

    std::vector vec1 = { 1, 1, 2, 2, 3, 4, 4, 4 };
    for (size_t i = 0; i < vec1.size();) {
        if (vec1[i] == 2) {
            i = VectorUtils::SwapWithLastAndDelete(vec1, i);
        } else {
            i++;
        }
    }
    ASSERT_EQ(vec1, (std::vector { 1, 1, 4, 4, 3, 4 }));

    std::vector vec2 = { 2, 2, 3, 3, 4, 4, 5 };
    auto iter = vec2.begin();
    while (iter != vec2.end()) {
        if (*iter == 3) {
            iter = VectorUtils::SwapWithLastAndDelete(vec2, iter);
        } else {
            ++iter;
        }
    }
    ASSERT_EQ(vec2, (std::vector { 2, 2, 5, 4, 4 }));
}

TEST(ContainerTest, VectorGetIntersection)
{
    const std::vector a = { 1, 2, 3, 4, 5 };
    const std::vector b = { 3, 4, 5, 6, 7 };
    const auto result = VectorUtils::GetIntersection(a, b);

    ASSERT_EQ(result, (std::vector { 3, 4, 5 }));
}

TEST(ContainerTest, SetGetIntersection)
{
    const std::unordered_set a = { 1, 2, 3, 4, 5 };
    const std::unordered_set b = { 3, 4, 5, 6, 7 };
    const auto result = SetUtils::GetIntersection(a, b);

    ASSERT_EQ(result.size(), 3);
}

TEST(ContainerTest, InplaceVectorBasic)
{
    InplaceVector<int, 10> t0(4, 0);
    ASSERT_EQ(t0.Capacity(), 10);
    ASSERT_EQ(t0.Size(), 4);

    for (auto i = 0; i < t0.Size(); i++) {
        t0[i] = i + 1;
    }
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4 }));

    t0.EmplaceBack(5);
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4, 5 }));

    t0.PushBack(6);
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4, 5, 6 }));

    t0.PopBack();
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4, 5 }));

    t0.Insert(0, 0);
    ASSERT_EQ(t0.ToVector(), (std::vector { 0, 1, 2, 3, 4, 5 }));

    t0.Insert(3, 6);
    ASSERT_EQ(t0.ToVector(), (std::vector { 0, 1, 2, 6, 3, 4, 5 }));

    t0.Insert(7, 7);
    ASSERT_EQ(t0.ToVector(), (std::vector { 0, 1, 2, 6, 3, 4, 5, 7 }));

    t0.Erase(3);
    ASSERT_EQ(t0.ToVector(), (std::vector { 0, 1, 2, 3, 4, 5, 7 }));

    t0.Erase(0);
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4, 5, 7 }));

    t0.Erase(5);
    ASSERT_EQ(t0.ToVector(), (std::vector { 1, 2, 3, 4, 5 }));

    t0.EraseSwapLast(0);
    ASSERT_EQ(t0.ToVector(), (std::vector { 5, 2, 3, 4 }));

    t0.EraseSwapLast(3);
    ASSERT_EQ(t0.ToVector(), (std::vector { 5, 2, 3 }));
}

TEST(ContainerTest, InplaceVectorIter)
{
    struct S0 {
        int32_t a;
        uint32_t b;

        S0()
            : a(0)
            , b(0)
        {
        }

        S0(int32_t inA, uint32_t inB)
            : a(inA)
            , b(inB)
        {
        }

        bool operator==(const S0& inOther) const
        {
            return a == inOther.a && b == inOther.b;
        }
    };

    InplaceVector<S0, 10> t0(3, S0 {});
    ASSERT_EQ(t0.Capacity(), 10);
    ASSERT_EQ(t0.Size(), 3);

    t0.Resize(5);
    t0.At(0) = { 1, 2 };
    t0.At(1) = { 2, 3 };
    t0.At(2) = { 3, 4 };
    t0.At(3) = { 4, 5 };
    t0.At(4) = { 5, 6 };
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 6 } }));

    auto i = 0;
    for (const auto& element : t0) {
        i++;
        ASSERT_EQ(element, (S0 { i, static_cast<uint32_t>(i + 1) }));
    }

    t0.Erase(t0.Begin());
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 6 } }));

    t0.Erase(t0.Begin() + 2);
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 2, 3 }, { 3, 4 }, { 5, 6 } }));

    t0.Erase(t0.End());
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 2, 3 }, { 3, 4 } }));

    t0.EmplaceBack(7, 8);
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 2, 3 }, { 3, 4 }, { 7, 8 } }));

    t0.EmplaceBack(8, 9);
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 2, 3 }, { 3, 4 }, { 7, 8 }, { 8, 9 } }));

    t0.EraseSwapLast(t0.Begin());
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 8, 9 }, { 3, 4 }, { 7, 8 } }));

    t0.EraseSwapLast(t0.End() - 1);
    ASSERT_EQ(t0.ToVector(), (std::vector<S0> { { 8, 9 }, { 3, 4 } }));
}

TEST(ContainerTest, InplaceVectorCopyAndMove)
{
    InplaceVector<CopyAndMoveTest, 10> t0;
    t0.EmplaceBack();
    t0.EmplaceBack();
    ASSERT_EQ(t0[0].constructType, ConstructType::cDefault);
    ASSERT_EQ(t0[1].constructType, ConstructType::cDefault);

    t0.PushBack(CopyAndMoveTest());
    ASSERT_EQ(t0[2].constructType, ConstructType::cMove);

    const CopyAndMoveTest temp0;
    t0.PushBack(temp0);
    ASSERT_EQ(t0[3].constructType, ConstructType::cCopy);

    // copy assign
    InplaceVector<CopyAndMoveTest, 10> t2;
    t2.Resize(2);
    ASSERT_EQ(t2[0].constructType, ConstructType::cCopy);
    ASSERT_EQ(t2[1].constructType, ConstructType::cCopy);
    t2 = t0;
    CopyAndMoveTest temp1;
    temp1.constructType = ConstructType::cCopy;
    temp1.copyAssigned = true;
    temp1.moveAssigned = false;
    ASSERT_EQ(t2[0], temp1);
    ASSERT_EQ(t2[1], temp1);
    temp1.copyAssigned = false;
    ASSERT_EQ(t2[2], temp1);
    ASSERT_EQ(t2[3], temp1);

    // move assign
    InplaceVector<CopyAndMoveTest, 10> t3;
    t3.Resize(1);
    t3 = std::move(t2);
    temp1.constructType = ConstructType::cCopy;
    temp1.copyAssigned = false;
    temp1.moveAssigned = true;
    ASSERT_EQ(t3[0], temp1);
    temp1.constructType = ConstructType::cMove;
    temp1.copyAssigned = false;
    temp1.moveAssigned = false;
    ASSERT_EQ(t3[1], temp1);
    ASSERT_EQ(t3[2], temp1);
    ASSERT_EQ(t3[3], temp1);

    // copy construct
    InplaceVector<CopyAndMoveTest, 10> t4 = t3;
    temp1.constructType = ConstructType::cCopy;
    temp1.copyAssigned = false;
    temp1.moveAssigned = false;
    ASSERT_EQ(t4[0], temp1);
    ASSERT_EQ(t4[1], temp1);
    ASSERT_EQ(t4[2], temp1);
    ASSERT_EQ(t4[3], temp1);

    // move construct
    InplaceVector<CopyAndMoveTest, 10> t5 = std::move(t4);
    temp1.constructType = ConstructType::cMove;
    temp1.copyAssigned = false;
    temp1.moveAssigned = false;
    ASSERT_EQ(t5[0], temp1);
    ASSERT_EQ(t5[1], temp1);
    ASSERT_EQ(t5[2], temp1);
    ASSERT_EQ(t5[3], temp1);
}

TEST(ContainerTest, TrunkBasic)
{
    Trunk<int, 4> t0;
    ASSERT_TRUE(t0.HasFree());
    ASSERT_TRUE(t0.Empty());
    ASSERT_EQ(t0.Free(), 4);
    ASSERT_EQ(t0.Allocated(), 0);
    ASSERT_EQ(t0.Capacity(), 4); // NOLINT

    const size_t v0 = t0.Emplace(1);
    ASSERT_TRUE(t0.HasFree());
    ASSERT_FALSE(t0.Empty());
    ASSERT_EQ(t0.Free(), 3);
    ASSERT_EQ(t0.Allocated(), 1);
    ASSERT_EQ(t0[v0], 1);

    const size_t v1 = t0.Emplace(2);
    ASSERT_TRUE(t0.HasFree());
    ASSERT_FALSE(t0.Empty());
    ASSERT_EQ(t0.Free(), 2);
    ASSERT_EQ(t0.Allocated(), 2);
    ASSERT_EQ(t0[v1], 2);

    t0.Erase(v0);
    ASSERT_EQ(t0.Free(), 3);
    ASSERT_EQ(t0.Allocated(), 1);

    const size_t v2 = t0.Emplace(3);
    ASSERT_EQ(t0.Free(), 2);
    ASSERT_EQ(t0.Allocated(), 2);
    ASSERT_EQ(v0, v2);
    ASSERT_EQ(t0[v2], 3);
}

TEST(ContainerTest, TrunkCopyAndMove)
{
    // TODO
}

TEST(ContainerTest, TrunkListTest)
{
    // TODO
}
