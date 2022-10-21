//
// Created by johnk on 2022/9/29.
//

#include <gtest/gtest.h>

#include <Mirror/Registry.h>

int v0 = 1;

int F0(int a, int b)
{
    return a + b;
}

int& F1()
{
    return v0;
}

void F2(int& outValue)
{
    outValue = 1;
}

TEST(RegistryTest, GlobalScopeTest)
{
    Mirror::Registry::Get()
        .Global()
            .Variable<&v0>("v0")
            .Function<&F0>("F0")
            .Function<&F1>("F1")
            .Function<&F2>("F2");

    const auto& globalScope = Mirror::GlobalScope::Get();
    {
        const auto& variable = globalScope.GetVariable("v0");

        auto value = variable.Get();
        ASSERT_EQ(value.CastTo<int>(), 1);
        ASSERT_EQ(value.CastTo<const int&>(), 1);
        value.CastTo<int&>() = 2;
        ASSERT_EQ(value.CastTo<int>(), 2);

        variable.Set(3);
        ASSERT_EQ(v0, 3);

        value = 4;
        variable.Set(&value);
        ASSERT_EQ(v0, 4);

        Mirror::Any anyValue = 5;
        variable.Set(&anyValue);
        ASSERT_EQ(v0, 5);
    }

    {
        const auto& function = globalScope.GetFunction("F0");
        auto result = function.Invoke(1, 2);
        ASSERT_EQ(result.CastTo<int>(), 3);
    }

    {
        const auto& function = globalScope.GetFunction("F1");
        auto result = function.Invoke();
        ASSERT_EQ(result.CastTo<const int&>(), 5);
    }

    {
        const auto& function = globalScope.GetFunction("F2");
        int value = 0;
        function.Invoke(std::ref(value));
        ASSERT_EQ(value, 1);

        value = 0;
        std::vector<Mirror::Any> arguments = { std::ref(value) };
        function.InvokeWith(arguments.data(), arguments.size());
        ASSERT_EQ(value, 1);
    }
}
