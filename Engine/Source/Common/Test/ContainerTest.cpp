//
// Created by johnk on 2023/12/5.
//

#include <Test/Test.h>
#include <Common/Container.h>
using namespace Test;
using namespace Common;

TEST(ContainerTest, VectorSwapDeleteTest)
{
    std::vector vec0 = { 1, 2, 3, 4, 5 };
    const auto index = VectorUtils::SwapWithLastAndDelete(vec0, 2);
    ASSERT_EQ(index, 2);
    AssertVecEq(vec0, std::vector { 1, 2, 5, 4 });

    std::vector vec1 = { 1, 1, 2, 2, 3, 4, 4, 4 };
    for (size_t i = 0; i < vec1.size();) {
        if (vec1[i] == 2) {
            i = VectorUtils::SwapWithLastAndDelete(vec1, i);
        } else {
            i++;
        }
    }
    AssertVecEq(vec1, std::vector { 1, 1, 4, 4, 3, 4 });

    std::vector vec2 = { 2, 2, 3, 3, 4, 4, 5 };
    auto iter = vec2.begin();
    while (iter != vec2.end()) {
        if (*iter == 3) {
            iter = VectorUtils::SwapWithLastAndDelete(vec2, iter);
        } else {
            ++iter;
        }
    }
    AssertVecEq(vec2, std::vector { 2, 2, 5, 4, 4 });
}

TEST(ContainerTest, VectorGetIntersection)
{
    const std::vector a = { 1, 2, 3, 4, 5 };
    const std::vector b = { 3, 4, 5, 6, 7 };
    const auto result = VectorUtils::GetIntersection(a, b);

    AssertVecEq(result, std::vector { 3, 4, 5 });
}

TEST(ContainerTest, SetGetIntersection)
{
    const std::unordered_set a = { 1, 2, 3, 4, 5 };
    const std::unordered_set b = { 3, 4, 5, 6, 7 };
    const auto result = SetUtils::GetIntersection(a, b);

    ASSERT_EQ(result.size(), 3);
}

TEST(ContainerTest, HeapVectorBasic)
{
    InplaceVector<int, 10> t0(4, 0);
    ASSERT_EQ(t0.Capacity(), 10);
    ASSERT_EQ(t0.Size(), 4);

    for (auto i = 0; i < t0.Size(); i++) {
        t0[i] = i + 1;
    }
    AssertVecEq(t0, std::vector { 1, 2, 3, 4 });

    t0.EmplaceBack(5);
    AssertVecEq(t0, std::vector { 1, 2, 3, 4, 5 });

    t0.PushBack(6);
    AssertVecEq(t0, std::vector { 1, 2, 3, 4, 5, 6 });

    t0.PopBack();
    AssertVecEq(t0, std::vector { 1, 2, 3, 4, 5 });

    t0.Insert(0, 0);
    AssertVecEq(t0, std::vector { 0, 1, 2, 3, 4, 5 });

    t0.Insert(3, 6);
    AssertVecEq(t0, std::vector { 0, 1, 2, 6, 3, 4, 5 });

    t0.Insert(7, 7);
    AssertVecEq(t0, std::vector { 0, 1, 2, 6, 3, 4, 5, 7 });

    t0.Erase(3);
    AssertVecEq(t0, std::vector { 0, 1, 2, 3, 4, 5, 7 });

    t0.Erase(0);
    AssertVecEq(t0, std::vector { 1, 2, 3, 4, 5, 7 });

    t0.Erase(5);
    AssertVecEq(t0, std::vector { 1, 2, 3, 4, 5 });

    t0.EraseSwapLast(0);
    AssertVecEq(t0, std::vector { 5, 2, 3, 4 });

    t0.EraseSwapLast(3);
    AssertVecEq(t0, std::vector { 5, 2, 3 });
}

TEST(ContainerTest, HeapVectorIter)
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
    AssertVecEq(t0, std::vector<S0> { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 6 } });

    auto i = 0;
    for (const auto& element : t0) {
        i++;
        ASSERT_EQ(element, (S0 { i, static_cast<uint32_t>(i + 1) }));
    }

    t0.Erase(t0.Begin());
    AssertVecEq(t0, std::vector<S0> { { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 6 } });

    t0.Erase(t0.Begin() + 2);
    AssertVecEq(t0, std::vector<S0> { { 2, 3 }, { 3, 4 }, { 5, 6 } });

    t0.Erase(t0.End());
    AssertVecEq(t0, std::vector<S0> { { 2, 3 }, { 3, 4 } });

    t0.EmplaceBack(7, 8);
    AssertVecEq(t0, std::vector<S0> { { 2, 3 }, { 3, 4 }, { 7, 8 } });

    t0.EmplaceBack(8, 9);
    AssertVecEq(t0, std::vector<S0> { { 2, 3 }, { 3, 4 }, { 7, 8 }, { 8, 9 } });

    t0.EraseSwapLast(t0.Begin());
    AssertVecEq(t0, std::vector<S0> { { 8, 9 }, { 3, 4 }, { 7, 8 } });

    t0.EraseSwapLast(t0.End() - 1);
    AssertVecEq(t0, std::vector<S0> { { 8, 9 }, { 3, 4 } });
}

TEST(ContainerTest, HeapVectorCopyAndMove)
{
    enum class ConstructType : uint8_t {
        cDefault,
        cCopy,
        cMove,
        max
    };

    struct S0 {
        ConstructType constructType;
        bool copyAssigned;
        bool moveAssigned;

        S0()
            : constructType(ConstructType::cDefault)
            , copyAssigned(false)
            , moveAssigned(false)
        {
        }

        S0(ConstructType inType, bool inCopyAssigned, bool inMoveAssigned)
            : constructType(inType)
            , copyAssigned(inCopyAssigned)
            , moveAssigned(inMoveAssigned)
        {
        }

        S0(const S0& inOther)
            : constructType(ConstructType::cCopy)
            , copyAssigned(false)
            , moveAssigned(false)
        {
        }

        S0(S0&& inOther) noexcept
            : constructType(ConstructType::cMove)
            , copyAssigned(false)
            , moveAssigned(false)
        {
        }

        S0& operator=(const S0& inOther)
        {
            copyAssigned = true;
            return *this;
        }

        S0& operator=(S0&& inOther) noexcept
        {
            moveAssigned = true;
            return *this;
        }

        bool operator==(const S0& inOther) const
        {
            return constructType == inOther.constructType
                && copyAssigned == inOther.copyAssigned
                && moveAssigned == inOther.moveAssigned;
        }
    };

    InplaceVector<S0, 10> t0;
    t0.EmplaceBack();
    t0.EmplaceBack();
    ASSERT_EQ(t0[0].constructType, ConstructType::cDefault);
    ASSERT_EQ(t0[1].constructType, ConstructType::cDefault);

    t0.PushBack(S0());
    ASSERT_EQ(t0[2].constructType, ConstructType::cMove);

    const S0 temp0;
    t0.PushBack(temp0);
    ASSERT_EQ(t0[3].constructType, ConstructType::cCopy);

    // copy assign
    InplaceVector<S0, 10> t2;
    t2.Resize(2);
    ASSERT_EQ(t2[0].constructType, ConstructType::cCopy);
    ASSERT_EQ(t2[1].constructType, ConstructType::cCopy);
    t2 = t0;
    S0 temp1;
    temp1.constructType = ConstructType::cCopy;
    temp1.copyAssigned = true;
    temp1.moveAssigned = false;
    ASSERT_EQ(t2[0], temp1);
    ASSERT_EQ(t2[1], temp1);
    temp1.copyAssigned = false;
    ASSERT_EQ(t2[2], temp1);
    ASSERT_EQ(t2[3], temp1);

    // move assign
    InplaceVector<S0, 10> t3;
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
    InplaceVector<S0, 10> t4 = t3;
    temp1.constructType = ConstructType::cCopy;
    temp1.copyAssigned = false;
    temp1.moveAssigned = false;
    ASSERT_EQ(t4[0], temp1);
    ASSERT_EQ(t4[1], temp1);
    ASSERT_EQ(t4[2], temp1);
    ASSERT_EQ(t4[3], temp1);

    // move construct
    InplaceVector<S0, 10> t5 = std::move(t4);
    temp1.constructType = ConstructType::cMove;
    temp1.copyAssigned = false;
    temp1.moveAssigned = false;
    ASSERT_EQ(t5[0], temp1);
    ASSERT_EQ(t5[1], temp1);
    ASSERT_EQ(t5[2], temp1);
    ASSERT_EQ(t5[3], temp1);
}
