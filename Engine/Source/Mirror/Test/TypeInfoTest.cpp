//
// Created by johnk on 2022/9/11.
//

#include <type_traits>

#include <gtest/gtest.h>

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
