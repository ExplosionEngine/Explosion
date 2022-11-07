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

struct C0 {
    static int& F0();

    static int v0;
};

int C0::v0 = 0;

int& C0::F0()
{
    return v0;
}

class C1 {
public:
    explicit C1(int inV0) : v0(inV0) {}

    int GetV0() const
    {
        return v0;
    }

    void SetV0(int inV0)
    {
        v0 = inV0;
    }

private:
    int v0;
};

struct C2 {
    C2(int inA, int inB) : a(inA), b(inB) {}

    int a;
    int b;
};

TEST(RegistryTest, GlobalScopeTest)
{
    Mirror::Registry::Get()
        .Global()
            .MetaData("TestKey", "Global")
            .Variable<&v0>("v0")
                .MetaData("TestKey", "v0")
            .Function<&F0>("F0")
            .Function<&F1>("F1")
            .Function<&F2>("F2");

    const auto& globalScope = Mirror::GlobalScope::Get();
    ASSERT_EQ(globalScope.GetMeta("TestKey"), "Global");
    {
        const auto& variable = globalScope.GetVariable("v0");
        ASSERT_EQ(variable.GetMeta("TestKey"), "v0");

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

TEST(RegistryTest, ClassTest)
{
    Mirror::Registry::Get()
        .Class<C0>("C0")
            .MetaData("TestKey", "C0")
            .StaticVariable<&C0::v0>("v0")
                .MetaData("TestKey", "v0")
            .StaticFunction<&C0::F0>("F0")
                .MetaData("TestKey", "F0");

    Mirror::Registry::Get()
        .Class<C1>("C1")
            .Constructor<int>("Constructor0")
            .MemberFunction<&C1::SetV0>("SetV0")
            .MemberFunction<&C1::GetV0>("GetV0");

    Mirror::Registry::Get()
        .Class<C2>("C2")
            .Constructor<int, int>("Constructor0")
            .MemberVariable<&C2::a>("a")
            .MemberVariable<&C2::b>("b");

    {
        const auto& clazz = Mirror::Class::Get("C0");
        ASSERT_EQ(clazz.GetMeta("TestKey"), "C0");
        {
            const auto& variable = clazz.GetStaticVariable("v0");
            ASSERT_EQ(variable.GetMeta("TestKey"), "v0");
            variable.Set(1);
            ASSERT_EQ(variable.Get().CastTo<int>(), 1);
        }

        {
            const auto& function = clazz.GetStaticFunction("F0");
            ASSERT_EQ(function.GetMeta("TestKey"), "F0");
            auto result = function.Invoke();
            ASSERT_EQ(result.CastTo<int&>(), 1);
        }
    }

    {
        const auto& clazz = Mirror::Class::Get("C1");
        const auto& constructor = clazz.GetConstructor("Constructor0");
        const auto& setter = clazz.GetMemberFunction("SetV0");
        const auto& getter = clazz.GetMemberFunction("GetV0");

        auto object = constructor.ConstructOnStack(1);
        ASSERT_EQ(getter.Invoke(object.CastTo<C1&>()).CastTo<int>(), 1);
        setter.Invoke(object.CastTo<C1&>(), 2);
        ASSERT_EQ(getter.Invoke(object.CastTo<C1&>()).CastTo<int>(), 2);
    }

    {
        const auto& clazz = Mirror::Class::Get("C2");
        const auto& constructor = clazz.GetConstructor("Constructor0");
        const auto& destructor = clazz.GetDestructor();
        const auto& a = clazz.GetMemberVariable("a");
        const auto& b = clazz.GetMemberVariable("b");

        auto object = constructor.NewObject(1, 2);
        auto objectRef = Mirror::Any(*object.CastTo<C2*>());
        ASSERT_EQ(a.Get(&objectRef).CastTo<int>(), 1);
        ASSERT_EQ(b.Get(&objectRef).CastTo<int>(), 2);
        destructor.InvokeWith(&objectRef);
    }
}
