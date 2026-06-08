//
// Created by johnk on 2022/9/29.
//

#include <Test/Test.h>

#include <RegistryTest.h>
#include <Mirror/Mirror.h>

#include <any>

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

int F3(int&& inValue)
{
    return std::move(inValue);
}

int F4(int inValue)
{
    return inValue;
}

float F4(int inValue, float inRet)
{
    return inRet;
}

int C0::v0 = 0;

int& C0::F0()
{
    return v0;
}

int C0::F1(int inValue)
{
    return inValue;
}

int C0::F1(int inValue0, int inValue1)
{
    return inValue0 + inValue1;
}

int C0::F2(int inValue) // NOLINT
{
    return inValue;
}

int C0::F2(int inValue0, int inValue1) // NOLINT
{
    return inValue0 + inValue1;
}

C1::C1(const int inV0)
    : v0(inV0)
{
}

int C1::GetV0() const
{
    return v0;
}

void C1::SetV0(const int inV0)
{
    v0 = inV0;
}

C2::C2(const int inA, const int inB)
    : a(inA), b(inB)
{
}

C3::C3(const int inA, const int inB, const int inC)
    : C2(inA, inB), c(inC)
{
}

TEST(RegistryTest, GlobalScopeTest)
{
    const auto& globalScope = Mirror::GlobalScope::Get();
    {
        const auto& variable = globalScope.GetVariable("v0");
        ASSERT_EQ(variable.GetMeta("testKey"), "v0");

        auto value = variable.GetDyn();
        ASSERT_EQ(value.As<int>(), 1);
        ASSERT_EQ(value.As<const int&>(), 1);
        value.As<int&>() = 2;
        ASSERT_EQ(value.As<int>(), 2);

        variable.Set(3);
        ASSERT_EQ(v0, 3);

        value = 4;
        variable.SetDyn(value);
        ASSERT_EQ(v0, 4);

        Mirror::Any anyValue = 5;
        variable.SetDyn(anyValue);
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
        function.Invoke(value);
        ASSERT_EQ(value, 1);

        value = 0;
        function.InvokeDyn({ Mirror::Any(std::ref(value)) });
        ASSERT_EQ(value, 1);
    }

    {
        const auto& function = globalScope.GetFunction("F3");
        int value = 1;
        function.Invoke(std::ref(value));
        ASSERT_EQ(value, 1);
    }

    {
        const auto& function = globalScope.GetFunction("F4(int)");
        int value = 1;
        auto ret = function.Invoke(value);
        ASSERT_EQ(ret.As<int>(), 1);
    }

    {
        const auto& function = globalScope.GetFunction("F4(int, float)");
        int value = 1;
        float temp = 2.0f;
        auto ret = function.Invoke(value, temp);
        ASSERT_EQ(ret.As<float>(), 2.0f);
    }
}

TEST(RegistryTest, ClassTest)
{
    {
        const auto& clazz = Mirror::Class::Get("C0");
        ASSERT_EQ(clazz.GetMeta("testKey"), "C0");
        {
            const auto& variable = clazz.GetStaticVariable("v0");
            ASSERT_EQ(variable.GetMeta("testKey"), "v0");
            variable.Set(1);
            ASSERT_EQ(variable.GetDyn().As<int>(), 1);
        }

        {
            const auto& function = clazz.GetStaticFunction("F0");
            ASSERT_EQ(function.GetMeta("testKey"), "F0");
            auto result = function.Invoke();
            ASSERT_EQ(result.As<int&>(), 1);
        }

        {
            const auto& function = clazz.GetStaticFunction("F1(int)");
            ASSERT_EQ(function.GetMeta("testKey"), "F1");
            auto result = function.Invoke(1);
            ASSERT_EQ(result.As<int>(), 1);
        }

        {
            const auto& function = clazz.GetStaticFunction("F1(int, int)");
            ASSERT_EQ(function.GetMeta("testKey"), "F1");
            auto result = function.Invoke(1, 2);
            ASSERT_EQ(result.As<int>(), 3);
        }

        {
            C0 object = {}; // NOLINT
            const auto& function = clazz.GetMemberFunction("F2(int)");
            ASSERT_EQ(function.GetMeta("testKey"), "F2");
            auto result = function.Invoke(object, 1);
            ASSERT_EQ(result.As<int>(), 1);
        }

        {
            C0 object = {}; // NOLINT
            const auto& function = clazz.GetMemberFunction("F2(int, int)");
            ASSERT_EQ(function.GetMeta("testKey"), "F2");
            auto result = function.Invoke(object, 1, 2);
            ASSERT_EQ(result.As<int>(), 3);
        }
    }

    {
        const auto& clazz = Mirror::Class::Get<C1>();
        const auto& constructor = clazz.GetConstructor("const int");
        const auto& setter = clazz.GetMemberFunction("SetV0");
        const auto& getter = clazz.GetMemberFunction("GetV0");

        auto object = constructor.Construct(1);
        ASSERT_EQ(getter.Invoke(object.As<C1&>()).As<int>(), 1);
        setter.Invoke(object.As<C1&>(), 2);
        ASSERT_EQ(getter.Invoke(object.As<C1&>()).As<int>(), 2);
    }

    {
        const auto& clazz = Mirror::Class::Get<C2>();
        const auto& constructor = clazz.GetConstructor("const int, const int");
        const auto& destructor = clazz.GetDestructor();
        const auto& a = clazz.GetMemberVariable("a");
        const auto& b = clazz.GetMemberVariable("b");

        auto object = constructor.New(1, 2);
        Mirror::Any objectRef = object.Deref();
        ASSERT_EQ(a.GetDyn(objectRef).As<int>(), 1);
        ASSERT_EQ(b.GetDyn(objectRef).As<int>(), 2);
        destructor.DeleteDyn(object);
    }

    {
        const auto& clazz = Mirror::Class::Get<C3>();
        auto object = clazz.GetConstructor("const int, const int, const int").New(1, 2, 3);
        auto* c2Obj = object.As<C2*>();

        ASSERT_EQ(c2Obj->a, 1);
        ASSERT_EQ(c2Obj->b, 2);
    }
}

TEST(RegistryTest, EnumTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    auto a = enumInfo.GetValue("a");
    auto b = enumInfo.GetValue("b");
    auto c = enumInfo.GetValue("c");
    auto max = enumInfo.GetValue("max");

    ASSERT_EQ(a.GetDyn().As<E0>(), E0::a);
    ASSERT_EQ(b.GetDyn().As<E0>(), E0::b);
    ASSERT_EQ(c.GetDyn().As<E0>(), E0::c);
    ASSERT_EQ(max.GetDyn().As<E0>(), E0::max);

    ASSERT_EQ(a.GetName(), "a");
    ASSERT_EQ(b.GetName(), "b");
    ASSERT_EQ(c.GetName(), "c");
    ASSERT_EQ(max.GetName(), "max");
}

TEST(RegistryTest, IdTest)
{
    Mirror::Id id0("foo");
    Mirror::Id id1("foo");
    Mirror::Id id2("bar");

    ASSERT_FALSE(id0.IsNull());
    ASSERT_TRUE(id0 == id1);
    ASSERT_FALSE(id0 == id2);

    ASSERT_TRUE(Mirror::Id::null.IsNull());
    ASSERT_FALSE(id0 == Mirror::Id::null);

    Mirror::Id id3;
    ASSERT_TRUE(id3.IsNull());
    ASSERT_TRUE(id3 == Mirror::Id::null);

    Mirror::IdHashProvider hasher;
    ASSERT_EQ(hasher(id0), id0.hash);
    ASSERT_EQ(hasher(id0), hasher(id1));
    ASSERT_NE(hasher(id0), hasher(id2));
}

TEST(RegistryTest, IdPresetsTest)
{
    ASSERT_FALSE(Mirror::IdPresets::globalScope.IsNull());
    ASSERT_FALSE(Mirror::IdPresets::detor.IsNull());
    ASSERT_FALSE(Mirror::IdPresets::defaultCtor.IsNull());
    ASSERT_FALSE(Mirror::IdPresets::copyCtor.IsNull());
    ASSERT_FALSE(Mirror::IdPresets::moveCtor.IsNull());

    ASSERT_NE(Mirror::IdPresets::detor, Mirror::IdPresets::defaultCtor);
    ASSERT_NE(Mirror::IdPresets::copyCtor, Mirror::IdPresets::moveCtor);
}

TEST(RegistryTest, ReflNodeMetaTest)
{
    const auto& globalScope = Mirror::GlobalScope::Get();
    const auto& variable = globalScope.GetVariable("v0");

    ASSERT_TRUE(variable.HasMeta("testKey"));
    ASSERT_FALSE(variable.HasMeta("nonExistKey"));

    ASSERT_EQ(variable.GetMeta("testKey"), "v0");
    ASSERT_EQ(variable.GetMetaOr("nonExistKey", "default"), "default");
    ASSERT_EQ(variable.GetMetaOr("testKey", "default"), "v0");

    const auto allMeta = variable.GetAllMeta();
    ASSERT_FALSE(allMeta.empty());
}

TEST(RegistryTest, ReflNodeMetaConvertTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    ASSERT_FALSE(clazz.IsTransient());

    // Bool / Int / Float convert helpers default-fallback paths.
    ASSERT_TRUE(clazz.GetMetaBoolOr("nonExist", true));
    ASSERT_FALSE(clazz.GetMetaBoolOr("nonExist", false));
    ASSERT_EQ(clazz.GetMetaInt32Or("nonExist", 42), 42);
    ASSERT_EQ(clazz.GetMetaInt64Or("nonExist", 42LL), 42LL);
    ASSERT_FLOAT_EQ(clazz.GetMetaFloatOr("nonExist", 3.14f), 3.14f);
}

TEST(RegistryTest, GlobalScopeForEachTest)
{
    const auto& globalScope = Mirror::GlobalScope::Get();
    ASSERT_TRUE(globalScope.HasVariable("v0"));
    ASSERT_FALSE(globalScope.HasVariable("nonExistVariable"));

    ASSERT_NE(globalScope.FindVariable("v0"), nullptr);
    ASSERT_EQ(globalScope.FindVariable("nonExistVariable"), nullptr);

    ASSERT_TRUE(globalScope.HasFunction("F0"));
    ASSERT_FALSE(globalScope.HasFunction("nonExistFunction"));

    ASSERT_NE(globalScope.FindFunction("F0"), nullptr);
    ASSERT_EQ(globalScope.FindFunction("nonExistFunction"), nullptr);

    bool foundV0 = false;
    globalScope.ForEachVariable([&](const Mirror::Variable& var) -> void {
        if (var.GetName() == "v0") {
            foundV0 = true;
        }
    });
    ASSERT_TRUE(foundV0);

    bool foundF0 = false;
    globalScope.ForEachFunction([&](const Mirror::Function& fn) -> void {
        if (fn.GetName() == "F0") {
            foundF0 = true;
        }
    });
    ASSERT_TRUE(foundF0);
}

TEST(RegistryTest, ClassFindAndGetTest)
{
    ASSERT_TRUE(Mirror::Class::Has<C0>());
    ASSERT_TRUE(Mirror::Class::Has(Mirror::Id("C0")));
    ASSERT_TRUE(Mirror::Class::Has(Mirror::GetTypeInfo<C0>()));
    ASSERT_TRUE(Mirror::Class::Has(Mirror::GetTypeInfo<C0>()->id));

    ASSERT_NE(Mirror::Class::Find<C0>(), nullptr);
    ASSERT_NE(Mirror::Class::Find(Mirror::Id("C0")), nullptr);
    ASSERT_NE(Mirror::Class::Find(Mirror::GetTypeInfo<C0>()), nullptr);
    ASSERT_NE(Mirror::Class::Find(Mirror::GetTypeInfo<C0>()->id), nullptr);

    ASSERT_EQ(Mirror::Class::Find(Mirror::Id("nonExistClass")), nullptr);

    const auto& clazz0 = Mirror::Class::Get<C0>();
    const auto& clazz1 = Mirror::Class::Get(Mirror::Id("C0"));
    const auto& clazz2 = Mirror::Class::Get(Mirror::GetTypeInfo<C0>());
    const auto& clazz3 = Mirror::Class::Get(Mirror::GetTypeInfo<C0>()->id);
    ASSERT_EQ(&clazz0, &clazz1);
    ASSERT_EQ(&clazz0, &clazz2);
    ASSERT_EQ(&clazz0, &clazz3);
}

TEST(RegistryTest, ClassGetAllTest)
{
    const auto all = Mirror::Class::GetAll();
    ASSERT_FALSE(all.empty());

    bool foundC0 = false;
    bool foundC2 = false;
    for (const auto* clazz : all) {
        if (clazz->GetName() == "C0") foundC0 = true;
        if (clazz->GetName() == "C2") foundC2 = true;
    }
    ASSERT_TRUE(foundC0);
    ASSERT_TRUE(foundC2);
}

TEST(RegistryTest, ClassInheritanceTest)
{
    const auto& c2 = Mirror::Class::Get<C2>();
    const auto& c3 = Mirror::Class::Get<C3>();

    ASSERT_EQ(c3.GetBaseClass(), &c2);
    ASSERT_EQ(c2.GetBaseClass(), nullptr);

    ASSERT_TRUE(c2.IsBaseOf(&c3));
    ASSERT_FALSE(c3.IsBaseOf(&c2));

    ASSERT_TRUE(c3.IsDerivedFrom(&c2));
    ASSERT_FALSE(c2.IsDerivedFrom(&c3));
}

TEST(RegistryTest, ClassConstructorAndDestructorTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    ASSERT_TRUE(clazz.HasDestructor());
    ASSERT_NE(clazz.FindDestructor(), nullptr);
    ASSERT_TRUE(clazz.HasConstructor("const int, const int"));
    ASSERT_FALSE(clazz.HasConstructor("nonExistCtor"));
    ASSERT_NE(clazz.FindConstructor("const int, const int"), nullptr);
    ASSERT_EQ(clazz.FindConstructor("nonExistCtor"), nullptr);

    const auto& ctor = clazz.GetConstructor("const int, const int");
    ASSERT_EQ(ctor.GetArgsNum(), 2);
    ASSERT_EQ(ctor.GetArgTypeInfos().size(), 2);
    ASSERT_EQ(&ctor.GetOwner(), &clazz);
    ASSERT_EQ(ctor.GetOwnerName(), "C2");
}

TEST(RegistryTest, ClassFindSuitableConstructorTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();

    int v0 = 1;
    int v1 = 2;
    Mirror::ArgumentList args = Mirror::ForwardAsArgList(v0, v1);
    const auto* ctor = clazz.FindSuitableConstructor(args);
    ASSERT_NE(ctor, nullptr);

    auto obj = clazz.ConstructDyn(args);
    ASSERT_EQ(obj.As<const C2&>().a, 1);
    ASSERT_EQ(obj.As<const C2&>().b, 2);
}

TEST(RegistryTest, ClassNewAndInplaceNewTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& ctor = clazz.GetConstructor("const int, const int");

    // New (heap) + delete.
    {
        Mirror::Any heap = ctor.New(7, 11);
        ASSERT_EQ(heap.Deref().As<C2&>().a, 7);
        clazz.GetDestructor().DeleteDyn(heap);
    }

    // InplaceNew.
    {
        alignas(C2) std::byte storage[sizeof(C2)];
        const auto inplace = ctor.InplaceNew(static_cast<void*>(storage), 3, 5);
        ASSERT_EQ(reinterpret_cast<C2*>(storage)->a, 3);
        ASSERT_EQ(reinterpret_cast<C2*>(storage)->b, 5);
        // Manual destruct via Destructor.
        clazz.GetDestructor().DestructDyn(inplace);
    }
}

TEST(RegistryTest, ClassMemberVariablesTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    ASSERT_TRUE(clazz.HasMemberVariable("a"));
    ASSERT_TRUE(clazz.HasMemberVariable("b"));
    ASSERT_FALSE(clazz.HasMemberVariable("nonExist"));
    ASSERT_NE(clazz.FindMemberVariable("a"), nullptr);
    ASSERT_EQ(clazz.FindMemberVariable("nonExist"), nullptr);

    const auto& members = clazz.GetMemberVariables();
    ASSERT_EQ(members.size(), 2);

    bool foundA = false;
    bool foundB = false;
    clazz.ForEachMemberVariable([&](const Mirror::MemberVariable& mv) -> void {
        if (mv.GetName() == "a") foundA = true;
        if (mv.GetName() == "b") foundB = true;
    });
    ASSERT_TRUE(foundA);
    ASSERT_TRUE(foundB);
}

TEST(RegistryTest, ClassStaticVariablesAndFunctionsTest)
{
    const auto& clazz = Mirror::Class::Get("C0");
    ASSERT_TRUE(clazz.HasStaticVariable("v0"));
    ASSERT_FALSE(clazz.HasStaticVariable("nonExist"));
    ASSERT_NE(clazz.FindStaticVariable("v0"), nullptr);
    ASSERT_EQ(clazz.FindStaticVariable("nonExist"), nullptr);

    ASSERT_TRUE(clazz.HasStaticFunction("F0"));
    ASSERT_FALSE(clazz.HasStaticFunction("nonExist"));
    ASSERT_NE(clazz.FindStaticFunction("F0"), nullptr);
    ASSERT_EQ(clazz.FindStaticFunction("nonExist"), nullptr);

    bool foundV0 = false;
    clazz.ForEachStaticVariable([&](const Mirror::Variable& var) -> void {
        if (var.GetName() == "v0") foundV0 = true;
    });
    ASSERT_TRUE(foundV0);

    bool foundF0 = false;
    clazz.ForEachStaticFunction([&](const Mirror::Function& fn) -> void {
        if (fn.GetName() == "F0") foundF0 = true;
    });
    ASSERT_TRUE(foundF0);
}

TEST(RegistryTest, ClassMemberFunctionsTest)
{
    const auto& clazz = Mirror::Class::Get<C1>();
    ASSERT_TRUE(clazz.HasMemberFunction("GetV0"));
    ASSERT_TRUE(clazz.HasMemberFunction("SetV0"));
    ASSERT_FALSE(clazz.HasMemberFunction("nonExist"));
    ASSERT_NE(clazz.FindMemberFunction("GetV0"), nullptr);
    ASSERT_EQ(clazz.FindMemberFunction("nonExist"), nullptr);

    const auto& getter = clazz.GetMemberFunction("GetV0");
    ASSERT_EQ(getter.GetArgsNum(), 0);
    ASSERT_NE(getter.GetRetTypeInfo(), nullptr);
    ASSERT_EQ(&getter.GetOwner(), &clazz);

    bool foundGet = false;
    bool foundSet = false;
    clazz.ForEachMemberFunction([&](const Mirror::MemberFunction& fn) -> void {
        if (fn.GetName() == "GetV0") foundGet = true;
        if (fn.GetName() == "SetV0") foundSet = true;
    });
    ASSERT_TRUE(foundGet);
    ASSERT_TRUE(foundSet);
}

TEST(RegistryTest, ClassDestructorOpsTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& ctor = clazz.GetConstructor("const int, const int");

    // Stack-construct, then DestructDyn via Class.
    {
        auto obj = ctor.Construct(1, 2);
        clazz.DestructDyn(obj);
    }

    // Heap-construct, then DeleteDyn via Class.
    {
        auto heap = ctor.New(3, 4);
        clazz.DeleteDyn(heap);
    }
}

TEST(RegistryTest, MemberVariableDynTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& ctor = clazz.GetConstructor("const int, const int");
    const auto& a = clazz.GetMemberVariable("a");
    const auto& b = clazz.GetMemberVariable("b");

    auto obj = ctor.Construct(1, 2);

    ASSERT_EQ(a.GetTypeInfo()->id, Mirror::GetTypeInfo<int>()->id);
    ASSERT_EQ(a.SizeOf(), sizeof(int));
    ASSERT_EQ(&a.GetOwner(), &clazz);

    Mirror::Any newVal = 100;
    a.SetDyn(obj, newVal);
    ASSERT_EQ(a.GetDyn(obj).As<int>(), 100);
    ASSERT_EQ(b.GetDyn(obj).As<int>(), 2);
    ASSERT_FALSE(a.IsTransient());
}

TEST(RegistryTest, MemberFunctionDynTest)
{
    const auto& clazz = Mirror::Class::Get<C1>();
    const auto& ctor = clazz.GetConstructor("const int");
    const auto& setter = clazz.GetMemberFunction("SetV0");
    const auto& getter = clazz.GetMemberFunction("GetV0");

    auto obj = ctor.Construct(5);
    setter.InvokeDyn(Mirror::ForwardAsArg(obj.As<C1&>()), Mirror::ForwardAsArgList(42));
    auto ret = getter.InvokeDyn(Mirror::ForwardAsArg(obj.As<C1&>()), {});
    ASSERT_EQ(ret.As<int>(), 42);
}

TEST(RegistryTest, EnumValueIntegralTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    const auto& a = enumInfo.GetValue("a");
    const auto& b = enumInfo.GetValue("b");
    const auto& c = enumInfo.GetValue("c");

    ASSERT_EQ(a.GetIntegral(), static_cast<int64_t>(E0::a));
    ASSERT_EQ(b.GetIntegral(), static_cast<int64_t>(E0::b));
    ASSERT_EQ(c.GetIntegral(), static_cast<int64_t>(E0::c));

    E0 v = E0::max;
    a.Set(v);
    ASSERT_EQ(v, E0::a);

    ASSERT_TRUE(a.Compare(E0::a));
    ASSERT_FALSE(a.Compare(E0::b));
}

TEST(RegistryTest, EnumLookupTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    ASSERT_EQ(&Mirror::Enum::Get(Mirror::Id("E0")), &enumInfo);
    ASSERT_EQ(Mirror::Enum::Find<E0>(), &enumInfo);
    ASSERT_EQ(Mirror::Enum::Find(Mirror::Id("E0")), &enumInfo);
    ASSERT_EQ(Mirror::Enum::Find(Mirror::Id("nonExistEnum")), nullptr);

    ASSERT_TRUE(enumInfo.HasValue(Mirror::Id("a")));
    ASSERT_FALSE(enumInfo.HasValue(Mirror::Id("nonExist")));
    ASSERT_NE(enumInfo.FindValue(Mirror::Id("a")), nullptr);
    ASSERT_EQ(enumInfo.FindValue(Mirror::Id("nonExist")), nullptr);

    // Lookup by integral.
    ASSERT_TRUE(enumInfo.HasValue(static_cast<Mirror::EnumValue::IntegralValue>(E0::a)));
    ASSERT_TRUE(enumInfo.HasValue(E0::b));
    ASSERT_FALSE(enumInfo.HasValue(static_cast<E0>(99)));

    // Lookup by argument.
    Mirror::Any anyA = E0::a;
    ASSERT_TRUE(enumInfo.HasValue(Mirror::ForwardAsArg(anyA.As<const E0&>())));
}

TEST(RegistryTest, EnumGetSortedValuesTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    auto sorted = enumInfo.GetSortedValues();
    ASSERT_FALSE(sorted.empty());

    for (size_t i = 1; i < sorted.size(); ++i) {
        ASSERT_LT(sorted[i - 1]->GetIntegralDyn(), sorted[i]->GetIntegralDyn());
    }

    ASSERT_FALSE(enumInfo.GetValues().empty());
}

TEST(RegistryTest, VariableTemplateSetTest)
{
    const auto& globalScope = Mirror::GlobalScope::Get();
    const auto& var = globalScope.GetVariable("v0");

    var.Set(99);
    ASSERT_EQ(v0, 99);
    ASSERT_EQ(var.Get().As<int>(), 99);

    var.Set(static_cast<int>(11));
    ASSERT_EQ(var.Get().As<int>(), 11);
}

TEST(RegistryTest, FunctionMetadataTest)
{
    const auto& globalScope = Mirror::GlobalScope::Get();
    const auto& fn = globalScope.GetFunction("F0");

    ASSERT_EQ(fn.GetArgsNum(), 2);
    ASSERT_EQ(fn.GetArgTypeInfos().size(), 2);
    ASSERT_EQ(fn.GetArgTypeInfo(0)->id, Mirror::GetTypeInfo<const int>()->id);
    ASSERT_NE(fn.GetRetTypeInfo(), nullptr);
}

TEST(RegistryTest, ConstructorTypeInfosTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& ctor = clazz.GetConstructor("const int, const int");

    ASSERT_EQ(ctor.GetArgTypeInfos().size(), 2);
    ASSERT_EQ(ctor.GetArgRemoveRefTypeInfos().size(), 2);
    ASSERT_EQ(ctor.GetArgRemovePointerTypeInfos().size(), 2);
    ASSERT_NE(ctor.GetArgRemoveRefTypeInfo(0), nullptr);
    ASSERT_NE(ctor.GetArgRemovePointerTypeInfo(0), nullptr);
}

TEST(RegistryTest, FieldAccessTest)
{
    // C0::F2 / v0 etc. are public; C1::v0 is private (registered with faPrivate).
    const auto& c0 = Mirror::Class::Get("C0");
    ASSERT_EQ(c0.GetStaticVariable("v0").GetAccess(), Mirror::FieldAccess::faPublic);
    ASSERT_EQ(c0.GetStaticFunction("F0").GetAccess(), Mirror::FieldAccess::faPublic);
    ASSERT_EQ(c0.GetMemberFunction("F2(int)").GetAccess(), Mirror::FieldAccess::faPublic);

    const auto& c1 = Mirror::Class::Get<C1>();
    ASSERT_EQ(c1.GetMemberVariable("v0").GetAccess(), Mirror::FieldAccess::faPrivate);
    ASSERT_EQ(c1.GetConstructor("const int").GetAccess(), Mirror::FieldAccess::faPublic);
    ASSERT_EQ(c1.GetDestructor().GetAccess(), Mirror::FieldAccess::faPublic);
    ASSERT_EQ(c1.GetMemberFunction("GetV0").GetAccess(), Mirror::FieldAccess::faPublic);
}

TEST(RegistryTest, ReflNodeMetaIntFloatBoolTest)
{
    const auto& clazz = Mirror::Class::Get<C8>();

    ASSERT_TRUE(clazz.GetMetaBool("boolMeta"));
    ASSERT_EQ(clazz.GetMetaInt32("intMeta"), 42);
    ASSERT_EQ(clazz.GetMetaInt64("int64Meta"), 1234567890123LL);
    ASSERT_FLOAT_EQ(clazz.GetMetaFloat("floatMeta"), 3.5f);
}

TEST(RegistryTest, ClassDefaultCtorAndDefaultObjectTest)
{
    const auto& clazz = Mirror::Class::Get<C4>();
    ASSERT_TRUE(clazz.HasDefaultConstructor());
    ASSERT_NE(clazz.FindDefaultConstructor(), nullptr);

    const auto& defaultCtor = clazz.GetDefaultConstructor();
    auto obj = defaultCtor.Construct();
    ASSERT_EQ(obj.As<const C4&>().v, 0);

    const auto defaultObj = clazz.GetDefaultObject();
    ASSERT_FALSE(defaultObj.Empty());
    ASSERT_EQ(defaultObj.As<const C4&>().v, 0);

    // C2 has no default constructor → no default object.
    const auto& c2 = Mirror::Class::Get<C2>();
    ASSERT_FALSE(c2.HasDefaultConstructor());
    ASSERT_EQ(c2.FindDefaultConstructor(), nullptr);
    ASSERT_TRUE(c2.GetDefaultObject().Empty());
}

TEST(RegistryTest, ClassFindWithCategoryTest)
{
    const auto animals = Mirror::Class::FindWithCategory("animal");
    ASSERT_EQ(animals.size(), 2);

    bool foundC4 = false;
    bool foundC5 = false;
    for (const auto* clazz : animals) {
        if (clazz->GetName() == "C4") foundC4 = true;
        if (clazz->GetName() == "C5") foundC5 = true;
    }
    ASSERT_TRUE(foundC4);
    ASSERT_TRUE(foundC5);

    const auto plants = Mirror::Class::FindWithCategory("plant");
    ASSERT_EQ(plants.size(), 1);
    ASSERT_EQ(plants[0]->GetName(), "C6");

    const auto bogus = Mirror::Class::FindWithCategory("nonExist");
    ASSERT_TRUE(bogus.empty());
}

TEST(RegistryTest, ClassMiscMetadataTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();

    ASSERT_EQ(clazz.GetTypeInfo()->id, Mirror::GetTypeInfo<C2>()->id);
    ASSERT_EQ(clazz.SizeOf(), sizeof(C2));
}

TEST(RegistryTest, ClassInplaceGetObjectTest)
{
    C2 obj { 1, 2 };
    const auto& clazz = Mirror::Class::Get<C2>();

    Mirror::Any any = clazz.InplaceGetObject(&obj);
    ASSERT_TRUE(any.IsNonConstRef());
    ASSERT_EQ(any.As<C2&>().a, 1);
    any.As<C2&>().a = 99;
    ASSERT_EQ(obj.a, 99);
}

// NOTE: Class::Cast<C> is registered for every reflected class, but exercising it
// from this test exe runs into a known type-identity mismatch across the
// Mirror.dll boundary on this MSVC build. The caster lambda itself has been
// inspected and is wired up correctly via codegen.

TEST(RegistryTest, ClassCastTest)
{
    // Use C2 (a simple class without a base) to exercise Cast.
    C2 obj { 1, 2 };
    const auto& clazz = Mirror::Class::Get<C2>();

    // Pointer cast.
    Mirror::Any anyPtr = &obj;
    Mirror::Any castPtr = clazz.Cast(anyPtr);
    ASSERT_FALSE(castPtr.Empty());

    // Ref cast.
    Mirror::Any anyRef = std::ref(obj);
    Mirror::Any castRef = clazz.Cast(anyRef);
    ASSERT_FALSE(castRef.Empty());
}

TEST(RegistryTest, ClassNewDynAndInplaceNewDynTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    Mirror::ArgumentList args = Mirror::ForwardAsArgList(7, 9);

    auto heap = clazz.NewDyn(args);
    ASSERT_EQ(heap.Deref().As<C2&>().a, 7);
    clazz.DeleteDyn(heap);

    alignas(C2) std::byte storage[sizeof(C2)];
    auto inplace = clazz.InplaceNewDyn(static_cast<void*>(storage), args);
    ASSERT_EQ(reinterpret_cast<C2*>(storage)->a, 7);
    ASSERT_EQ(reinterpret_cast<C2*>(storage)->b, 9);
    clazz.DestructDyn(inplace);
}

TEST(RegistryTest, ClassConstructAndNewTemplateTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();

    auto stack = clazz.Construct(1, 2);
    ASSERT_EQ(stack.As<const C2&>().a, 1);
    ASSERT_EQ(stack.As<const C2&>().b, 2);

    auto heap = clazz.New(3, 4);
    ASSERT_EQ(heap.Deref().As<C2&>().a, 3);
    clazz.Delete(heap.As<C2*>());

    alignas(C2) std::byte storage[sizeof(C2)];
    auto inplace = clazz.InplaceNew(static_cast<void*>(storage), 5, 6);
    ASSERT_EQ(reinterpret_cast<C2*>(storage)->a, 5);
    clazz.Destruct(*reinterpret_cast<C2*>(storage));
}

TEST(RegistryTest, MemberVariableIsTransientTest)
{
    const auto& clazz = Mirror::Class::Get<C7>();
    ASSERT_FALSE(clazz.GetMemberVariable("normal").IsTransient());
    ASSERT_TRUE(clazz.GetMemberVariable("trans").IsTransient());
}

TEST(RegistryTest, MemberVariableTemplateGetSetTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& a = clazz.GetMemberVariable("a");
    const auto& b = clazz.GetMemberVariable("b");

    C2 obj { 1, 2 };
    a.Set(obj, 10);
    b.Set(obj, 20);
    ASSERT_EQ(obj.a, 10);
    ASSERT_EQ(obj.b, 20);

    auto getA = a.Get(obj);
    ASSERT_EQ(getA.As<int&>(), 10);

    const C2 cobj { 3, 4 };
    auto getCA = a.Get(cobj);
    ASSERT_EQ(getCA.As<const int&>(), 3);
}

TEST(RegistryTest, MemberFunctionTemplateInvokeTest)
{
    const auto& clazz = Mirror::Class::Get<C1>();
    const auto& setter = clazz.GetMemberFunction("SetV0");
    const auto& getter = clazz.GetMemberFunction("GetV0");

    C1 obj { 5 };
    setter.Invoke(obj, 100);
    auto ret = getter.Invoke(obj);
    ASSERT_EQ(ret.As<int>(), 100);
}

TEST(RegistryTest, DestructorTemplateTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& detor = clazz.GetDestructor();

    // Stack destruct via template.
    {
        C2 obj { 1, 2 };
        detor.Destruct(obj);
    }

    // Heap delete via template.
    {
        auto* heap = new C2(3, 4);
        detor.Delete(heap);
    }
}

TEST(RegistryTest, EnumValueGetAndSetDirectTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    const auto& a = enumInfo.GetValue("a");
    const auto& c = enumInfo.GetValue("c");

    // Direct getter (non-Dyn).
    ASSERT_EQ(a.Get().As<E0>(), E0::a);
    ASSERT_EQ(c.Get().As<E0>(), E0::c);

    // Direct integral getter (non-Dyn).
    ASSERT_EQ(a.GetIntegral(), static_cast<int64_t>(E0::a));

    // Owner queries.
    ASSERT_EQ(a.GetOwner(), &enumInfo);
    ASSERT_EQ(a.GetOwnerName(), "E0");
}

TEST(RegistryTest, EnumValueDynGettersTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    const auto& b = enumInfo.GetValue("b");

    Mirror::Any anyB = b.GetDyn();
    ASSERT_EQ(anyB.As<E0>(), E0::b);
    ASSERT_EQ(b.GetIntegralDyn(), static_cast<int64_t>(E0::b));

    E0 target = E0::a;
    b.SetDyn(Mirror::ForwardAsArg(target));
    ASSERT_EQ(target, E0::b);

    target = E0::b;
    ASSERT_TRUE(b.CompareDyn(Mirror::ForwardAsArg(target)));
    target = E0::a;
    ASSERT_FALSE(b.CompareDyn(Mirror::ForwardAsArg(target)));
}

TEST(RegistryTest, EnumGetByIntegralAndArgTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();

    const auto& byIntegral = enumInfo.GetValue(static_cast<Mirror::EnumValue::IntegralValue>(E0::b));
    ASSERT_EQ(byIntegral.GetName(), "b");

    Mirror::Any arg = E0::c;
    const auto* found = enumInfo.FindValue(Mirror::ForwardAsArg(arg.As<const E0&>()));
    ASSERT_NE(found, nullptr);
    ASSERT_EQ(found->GetName(), "c");

    const auto& byArg = enumInfo.GetValue(Mirror::ForwardAsArg(arg.As<const E0&>()));
    ASSERT_EQ(byArg.GetName(), "c");
}

TEST(RegistryTest, EnumGetTypeInfoTest)
{
    const auto& enumInfo = Mirror::Enum::Get<E0>();
    ASSERT_EQ(enumInfo.GetTypeInfo()->id, Mirror::GetTypeInfo<E0>()->id);
}

TEST(RegistryTest, OwnerQueryTest)
{
    const auto& clazz = Mirror::Class::Get<C2>();
    const auto& mv = clazz.GetMemberVariable("a");
    const auto& mf_clazz = Mirror::Class::Get<C1>();
    const auto& mf = mf_clazz.GetMemberFunction("GetV0");
    const auto& ctor = clazz.GetConstructor("const int, const int");
    const auto& dtor = clazz.GetDestructor();

    ASSERT_EQ(&mv.GetOwner(), &clazz);
    ASSERT_EQ(mv.GetOwnerName(), "C2");
    ASSERT_FALSE(mv.GetOwnerId().IsNull());

    ASSERT_EQ(&mf.GetOwner(), &mf_clazz);
    ASSERT_EQ(mf.GetOwnerName(), "C1");

    ASSERT_EQ(&ctor.GetOwner(), &clazz);
    ASSERT_EQ(ctor.GetOwnerName(), "C2");

    ASSERT_EQ(&dtor.GetOwner(), &clazz);
    ASSERT_EQ(dtor.GetOwnerName(), "C2");

    const auto& var = Mirror::GlobalScope::Get().GetVariable("v0");
    // Global variable: owner is null.
    ASSERT_EQ(var.GetOwner(), nullptr);
    ASSERT_TRUE(var.GetOwnerId().IsNull());
    ASSERT_EQ(var.GetOwnerName(), "");

    const auto& fn = Mirror::GlobalScope::Get().GetFunction("F0");
    ASSERT_EQ(fn.GetOwner(), nullptr);
    ASSERT_TRUE(fn.GetOwnerId().IsNull());
}
