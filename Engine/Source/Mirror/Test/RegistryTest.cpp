//
// Created by johnk on 2022/9/29.
//

#include <gtest/gtest.h>

#include <Mirror/Registry.h>

int v0 = 1;

int F0(const int a, const int b)
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
    explicit C1(const int inV0) : v0(inV0) {}

    int GetV0() const
    {
        return v0;
    }

    void SetV0(const int inV0)
    {
        v0 = inV0;
    }

private:
    int v0;
};

struct C2 {
    C2(const int inA, const int inB) : a(inA), b(inB) {}

    int a;
    int b;
};

enum class E0 {
    a,
    b,
    c,
    max
};

struct C3 : C2 {
    C3(const int inA, const int inB, const int inC) : C2(inA, inB), c(inC) {}

    int c;
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
        ASSERT_EQ(value.As<int>(), 1);
        ASSERT_EQ(value.As<const int&>(), 1);
        value.As<int&>() = 2;
        ASSERT_EQ(value.As<int>(), 2);

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
        ASSERT_EQ(result.As<int>(), 3);
    }

    {
        const auto& function = globalScope.GetFunction("F1");
        auto result = function.Invoke();
        ASSERT_EQ(result.As<const int&>(), 5);
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

    Mirror::Registry::Get()
        .Class<C3, C2>("C3")
            .Constructor<int, int, int>("Constructor0")
            .MemberVariable<&C3::c>("c");

    {
        const auto& clazz = Mirror::Class::Get("C0");
        ASSERT_EQ(clazz.GetMeta("TestKey"), "C0");
        {
            const auto& variable = clazz.GetStaticVariable("v0");
            ASSERT_EQ(variable.GetMeta("TestKey"), "v0");
            variable.Set(1);
            ASSERT_EQ(variable.Get().As<int>(), 1);
        }

        {
            const auto& function = clazz.GetStaticFunction("F0");
            ASSERT_EQ(function.GetMeta("TestKey"), "F0");
            auto result = function.Invoke();
            ASSERT_EQ(result.As<int&>(), 1);
        }
    }

    {
        const auto& clazz = Mirror::Class::Get<C1>();
        const auto& constructor = clazz.GetConstructor("Constructor0");
        const auto& setter = clazz.GetMemberFunction("SetV0");
        const auto& getter = clazz.GetMemberFunction("GetV0");

        auto object = constructor.ConstructOnStack(1);
        ASSERT_EQ(getter.Invoke(object.As<C1&>()).As<int>(), 1);
        setter.Invoke(object.As<C1&>(), 2);
        ASSERT_EQ(getter.Invoke(object.As<C1&>()).As<int>(), 2);
    }

    {
        const auto& clazz = Mirror::Class::Get<C2>();
        const auto& constructor = clazz.GetConstructor("Constructor0");
        const auto& destructor = clazz.GetDestructor();
        const auto& a = clazz.GetMemberVariable("a");
        const auto& b = clazz.GetMemberVariable("b");

        auto object = constructor.NewObject(1, 2);
        auto objectRef = Mirror::Any(*object.As<C2*>());
        ASSERT_EQ(a.Get(&objectRef).As<int>(), 1);
        ASSERT_EQ(b.Get(&objectRef).As<int>(), 2);
        destructor.InvokeWith(&objectRef);
    }

    {
        const auto& clazz = Mirror::Class::Get<C3>();
        auto object = clazz.GetConstructor("Constructor0").NewObject(1, 2, 3);
        auto* c2Obj = object.As<C2*>();

        ASSERT_EQ(c2Obj->a, 1);
        ASSERT_EQ(c2Obj->b, 2);
    }
}

TEST(RegistryTest, EnumTest)
{
    Mirror::Registry::Get()
        .Enum<E0>("E0")
            .Element<E0::a>("A")
            .Element<E0::b>("B")
            .Element<E0::c>("C")
            .Element<E0::max>("max");

    const auto& enumInfo = Mirror::Enum::Get<E0>();
    auto a = enumInfo.GetElement("A");
    auto b = enumInfo.GetElement("B");
    auto c = enumInfo.GetElement("C");
    auto max = enumInfo.GetElement("max");

    ASSERT_EQ(a.As<E0>(), E0::a);
    ASSERT_EQ(b.As<E0>(), E0::b);
    ASSERT_EQ(c.As<E0>(), E0::c);
    ASSERT_EQ(max.As<E0>(), E0::max);

    ASSERT_EQ(enumInfo.GetElementName(&a), "A");
    ASSERT_EQ(enumInfo.GetElementName(&b), "B");
    ASSERT_EQ(enumInfo.GetElementName(&c), "C");
    ASSERT_EQ(enumInfo.GetElementName(&max), "max");
}
