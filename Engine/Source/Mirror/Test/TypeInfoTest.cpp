//
// Created by johnk on 2022/9/11.
//

#include <type_traits>

#include <Test/Test.h>
#include <Mirror/Registry.h>

int Add(const int a, const int b)
{
    return a + b;
}

struct TestClass {
    int a;

    int Add(const int b) const
    {
        return a + b;
    }
};

template <typename A, typename B>
void AssertTypeIdEq()
{
    ASSERT_EQ(Mirror::GetTypeInfo<A>()->id, Mirror::GetTypeInfo<B>()->id);
}

template <typename A, typename B>
void AssertTypeIdNe()
{
    ASSERT_NE(Mirror::GetTypeInfo<A>()->id, Mirror::GetTypeInfo<B>()->id);
}

TEST(TypeTest, TypeInfoTest)
{
    AssertTypeIdEq<int, int>();
    AssertTypeIdEq<int, const int>();
    AssertTypeIdEq<int, int&>();
    AssertTypeIdEq<int, const int&>();

    AssertTypeIdEq<const int, int>();
    AssertTypeIdEq<const int, const int>();
    AssertTypeIdEq<const int, int&>();
    AssertTypeIdEq<const int, const int&>();

    AssertTypeIdEq<int*, int*>();
    AssertTypeIdEq<const int*, const int*>();
    AssertTypeIdNe<int, int*>();
    AssertTypeIdNe<int*, const int*>();
}

TEST(TypeTest, TypeTraitsTest)
{
    ASSERT_TRUE((std::is_same_v<Mirror::Internal::FunctionTraits<decltype(&Add)>::RetType, int>));
    ASSERT_TRUE((std::is_same_v<Mirror::Internal::FunctionTraits<decltype(&Add)>::ArgsTupleType, std::tuple<int, int>>));

    ASSERT_TRUE((std::is_same_v<Mirror::Internal::MemberVariableTraits<decltype(&TestClass::a)>::ClassType, TestClass>));
    ASSERT_TRUE((std::is_same_v<Mirror::Internal::MemberVariableTraits<decltype(&TestClass::a)>::ValueType, int>));

    ASSERT_TRUE((std::is_same_v<Mirror::Internal::MemberFunctionTraits<decltype(&TestClass::Add)>::ClassType, const TestClass>));
    ASSERT_TRUE((std::is_same_v<Mirror::Internal::MemberFunctionTraits<decltype(&TestClass::Add)>::RetType, int>));
    ASSERT_TRUE((std::is_same_v<Mirror::Internal::MemberFunctionTraits<decltype(&TestClass::Add)>::ArgsTupleType, std::tuple<int>>));
}

TEST(TypeTest, TypeFlagsTest)
{
    const auto* intInfo = Mirror::GetTypeInfo<int>();
    ASSERT_FALSE(intInfo->isConst);
    ASSERT_FALSE(intInfo->isPointer);
    ASSERT_FALSE(intInfo->isLValueReference);
    ASSERT_FALSE(intInfo->isClass);
    ASSERT_TRUE(intInfo->isArithmetic);
    ASSERT_TRUE(intInfo->isIntegral);
    ASSERT_FALSE(intInfo->isFloatingPoint);

    const auto* constIntRefInfo = Mirror::GetTypeInfo<const int&>();
    ASSERT_FALSE(constIntRefInfo->isConst);
    ASSERT_TRUE(constIntRefInfo->isLValueReference);
    ASSERT_TRUE(constIntRefInfo->isLValueConstReference);
    ASSERT_TRUE(constIntRefInfo->isReference);

    const auto* intPtrInfo = Mirror::GetTypeInfo<int*>();
    ASSERT_TRUE(intPtrInfo->isPointer);
    ASSERT_FALSE(intPtrInfo->isConstPointer);

    const auto* constIntPtrInfo = Mirror::GetTypeInfo<const int*>();
    ASSERT_TRUE(constIntPtrInfo->isPointer);
    ASSERT_TRUE(constIntPtrInfo->isConstPointer);

    const auto* floatInfo = Mirror::GetTypeInfo<float>();
    ASSERT_TRUE(floatInfo->isArithmetic);
    ASSERT_FALSE(floatInfo->isIntegral);
    ASSERT_TRUE(floatInfo->isFloatingPoint);

    const auto* arrayInfo = Mirror::GetTypeInfo<int[3]>();
    ASSERT_TRUE(arrayInfo->isArray);
}

TEST(TypeTest, GetTypeIdTest)
{
    ASSERT_EQ(Mirror::GetTypeId<int>(), Mirror::GetTypeInfo<int>()->id);
    ASSERT_EQ(Mirror::GetTypeId<float>(), Mirror::GetTypeInfo<float>()->id);
    ASSERT_NE(Mirror::GetTypeId<int>(), Mirror::GetTypeId<float>());
}

TEST(TypeTest, RemovePointerTypeTest)
{
    const auto* intPtrInfo = Mirror::GetTypeInfo<int*>();
    ASSERT_EQ(intPtrInfo->removePointerType, Mirror::GetTypeInfo<int>()->id);

    const auto* constIntPtrInfo = Mirror::GetTypeInfo<const int*>();
    ASSERT_EQ(constIntPtrInfo->removePointerType, Mirror::GetTypeInfo<const int>()->id);
}

TEST(TypeTest, EqualComparableFlagTest)
{
    struct NotEq { };
    struct YesEq {
        bool operator==(const YesEq&) const { return true; }
        bool operator!=(const YesEq&) const { return false; }
    };

    ASSERT_TRUE(Mirror::GetTypeInfo<int>()->equalComparable);
    ASSERT_TRUE(Mirror::GetTypeInfo<YesEq>()->equalComparable);
    ASSERT_FALSE(Mirror::GetTypeInfo<NotEq>()->equalComparable);
}
