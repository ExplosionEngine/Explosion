//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Common/Debug.h>
#include <Mirror/Api.h>
#include <Mirror/Mirror.h>

namespace Mirror::Internal {
    template <typename T> struct VariableTraits {};
    template <typename T> struct FunctionTraits {};
    template <typename T> struct MemberVariableTraits {};
    template <typename T> struct MemberFunctionTraits {};
    template <typename ArgsTuple, size_t... I> auto GetArgTypeInfosByArgsTuple(std::index_sequence<I...>);
    template <typename ArgsTuple, size_t... I> auto ForwardArgumentsListAsTuple(const ArgumentList& args, std::index_sequence<I...>);
    template <auto Ptr, typename ArgsTuple, size_t... I> auto InvokeFunction(ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, auto Ptr, typename ArgsTuple, size_t... I> auto InvokeMemberFunction(Class& object, ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, typename ArgsTuple, size_t... I> auto InvokeConstructorStack(ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, typename ArgsTuple, size_t... I> auto InvokeConstructorNew(ArgsTuple& args, std::index_sequence<I...>);
}

namespace Mirror {
    template <typename Derived>
    class MetaDataRegistry {
    public:
        virtual ~MetaDataRegistry();

        Derived& MetaData(const Id& inKey, const std::string& inValue);

    protected:
        explicit MetaDataRegistry(Type* inContext);

        Derived& SetContext(Type* inContext);

    private:
        Type* context;
    };

    template <typename C>
    class ClassRegistry final : public MetaDataRegistry<ClassRegistry<C>> {
    public:
        ~ClassRegistry() override;

        template <typename... Args> ClassRegistry& Constructor(const Id& inId);
        template <auto Ptr> ClassRegistry& StaticVariable(const Id& inId);
        template <auto Ptr> ClassRegistry& StaticFunction(const Id& inId);
        template <auto Ptr> ClassRegistry& MemberVariable(const Id& inId);
        template <auto Ptr> ClassRegistry& MemberFunction(const Id& inId);

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass);

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry final : public MetaDataRegistry<GlobalRegistry> {
    public:
        ~GlobalRegistry() override;

        template <auto Ptr> GlobalRegistry& Variable(const Id& inId);
        template <auto Ptr> GlobalRegistry& Function(const Id& inId);

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope);

        GlobalScope& globalScope;
    };

    template <typename T>
    class EnumRegistry final : public MetaDataRegistry<EnumRegistry<T>> {
    public:
        ~EnumRegistry() override;

        template <auto Value> EnumRegistry& Element(const Id& inId);

    private:
        friend class Registry;

        explicit EnumRegistry(Enum& inEnum);

        Enum& enumInfo;
    };

    template <typename B, typename C> concept CppBaseClassOrVoid = Common::CppVoid<B> || Common::CppClass<B> && Common::CppClass<C> && std::is_base_of_v<B, C>;

    class MIRROR_API Registry {
    public:
        static Registry& Get();

        ~Registry();

        GlobalRegistry Global();

        template <Common::CppClass C, CppBaseClassOrVoid<C> B = void> ClassRegistry<C> Class(const Id& inId);
        template <Common::CppEnum T> EnumRegistry<T> Enum(const Id& inId);

    private:
        friend class GlobalScope;
        friend class Class;
        friend class Enum;

        Registry() noexcept;

        Mirror::Class& EmplaceClass(const Id& inId, Class::ConstructParams&& inParams);
        Mirror::Enum& EmplaceEnum(const Id& inId, Enum::ConstructParams&& inParams);

        GlobalScope globalScope;
        std::unordered_map<Id, Mirror::Class, IdHashProvider> classes;
        std::unordered_map<Id, Mirror::Enum, IdHashProvider> enums;
    };
}

namespace Mirror::Internal {
    template <typename T>
    struct VariableTraits<T*> {
        using ValueType = T;
    };

    template <typename Ret, typename... Args>
    struct FunctionTraits<Ret(*)(Args...)> {
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };

    template <typename Class, typename T>
    struct MemberVariableTraits<T Class::*> {
        using ClassType = Class;
        using ValueType = T;
    };

    template <typename Class, typename T>
    struct MemberVariableTraits<T Class::* const> {
        using ClassType = const Class;
        using ValueType = T;
    };

    template <typename Class, typename Ret, typename... Args>
    struct MemberFunctionTraits<Ret(Class::*)(Args...)> {
        using ClassType = Class;
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };

    template <typename Class, typename Ret, typename... Args>
    struct MemberFunctionTraits<Ret(Class::*)(Args...) const> {
        using ClassType = const Class;
        using RetType = Ret;
        using ArgsTupleType = std::tuple<Args...>;
    };

    template <typename ArgsTuple, size_t... I>
    auto GetArgTypeInfosByArgsTuple(std::index_sequence<I...>)
    {
        return std::vector<const TypeInfo*> { GetTypeInfo<std::tuple_element_t<I, ArgsTuple>>()... };
    }

    template <typename ArgsTuple, size_t... I>
    auto ForwardArgumentsListAsTuple(const ArgumentList& args, std::index_sequence<I...>)
    {
        return ArgsTuple { args[I].template As<std::tuple_element_t<I, ArgsTuple>>()... };
    }

    template <auto Ptr, typename ArgsTuple, size_t... I>
    auto InvokeFunction(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Ptr(std::get<I>(args)...);
    }

    template <typename Class, auto Ptr, typename ArgsTuple, size_t... I>
    auto InvokeMemberFunction(Class& object, ArgsTuple& args, std::index_sequence<I...>)
    {
        return (object.*Ptr)(std::get<I>(args)...);
    }

    template <typename Class, typename ArgsTuple, size_t... I>
    auto InvokeConstructorStack(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Class(std::get<I>(args)...);
    }

    template <typename Class, typename ArgsTuple, size_t... I>
    auto InvokeConstructorNew(ArgsTuple& args, std::index_sequence<I...>)
    {
        return new Class(std::get<I>(args)...);
    }
}

namespace Mirror {
    template <typename Derived>
    MetaDataRegistry<Derived>::MetaDataRegistry(Type* inContext)
        : context(inContext)
    {
        Assert(context);
    }

    template <typename Derived>
    MetaDataRegistry<Derived>::~MetaDataRegistry() = default;

    template <typename Derived>
    Derived& MetaDataRegistry<Derived>::MetaData(const Id& inKey, const std::string& inValue)
    {
        context->metas[inKey] = inValue;
        return static_cast<Derived&>(*this);
    }

    template <typename Derived>
    Derived& MetaDataRegistry<Derived>::SetContext(Type* inContext)
    {
        Assert(inContext);
        context = inContext;
        return static_cast<Derived&>(*this);
    }

    template <typename C>
    ClassRegistry<C>::ClassRegistry(Class& inClass)
        : MetaDataRegistry<ClassRegistry<C>>(&inClass), clazz(inClass)
    {
    }

    template <typename C>
    ClassRegistry<C>::~ClassRegistry() = default;

    template <typename C>
    template <typename... Args>
    ClassRegistry<C>& ClassRegistry<C>::Constructor(const Id& inId)
    {
        using ArgsTupleType = std::tuple<Args...>;
        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        const auto iter = clazz.constructors.find(inId);
        Assert(iter == clazz.constructors.end());

        Constructor::ConstructParams params;
        params.name = inId.name;
        params.argsNum = sizeof...(Args);
        params.argTypeInfos = { GetTypeInfo<Args>()... };
        params.argRemoveRefTypeInfos = { GetTypeInfo<std::remove_reference_t<Args>>()... };
        params.argRemovePointerTypeInfos = { GetTypeInfo<std::remove_pointer_t<Args>>()... };
        params.stackConstructor = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());
            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            return { Internal::InvokeConstructorStack<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}) };
        };
        params.heapConstructor = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());
            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            return { Internal::InvokeConstructorNew<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}) };
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceConstructor(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr>
    ClassRegistry<C>& ClassRegistry<C>::StaticVariable(const Id& inId)
    {
        using ValueType = typename Internal::VariableTraits<decltype(Ptr)>::ValueType;

        const auto iter = clazz.staticVariables.find(inId);
        Assert(iter == clazz.staticVariables.end());

        Variable::ConstructParams params;
        params.name = inId.name;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& value) -> void {
            *Ptr = value.As<ValueType>();
        };
        params.getter = []() -> Any {
            return { std::ref(*Ptr) };
        };
        params.serializer = nullptr;
        params.deserializer = nullptr;

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceStaticVariable(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr>
    ClassRegistry<C>& ClassRegistry<C>::StaticFunction(const Id& inId)
    {
        using ArgsTupleType = typename Internal::FunctionTraits<decltype(Ptr)>::ArgsTupleType;
        using RetType = typename Internal::FunctionTraits<decltype(Ptr)>::RetType;

        const auto iter = clazz.staticFunctions.find(inId);
        Assert(iter == clazz.staticFunctions.end());

        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        Function::ConstructParams params;
        params.name = inId.name;
        params.retTypeInfo = GetTypeInfo<RetType>();
        params.argsNum = argsTupleSize;
        params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
        params.invoker = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());

            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            if constexpr (std::is_void_v<RetType>) {
                Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {});
                return {};
            } else {
                return { Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}) };
            }
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceStaticFunction(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr>
    ClassRegistry<C>& ClassRegistry<C>::MemberVariable(const Id& inId)
    {
        using ClassType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ClassType;
        using ValueType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ValueType;

        const auto iter = clazz.memberVariables.find(inId);
        Assert(iter == clazz.memberVariables.end());

        MemberVariable::ConstructParams params;
        params.name = inId.name;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& object, const Argument& value) -> void {
            object.As<ClassType&>().*Ptr = value.As<const ValueType&>();
        };
        params.getter = [](const Argument& object) -> Any {
            return { std::ref(object.As<ClassType&>().*Ptr) };
        };
        params.serializer = [](Common::SerializeStream& stream, const Mirror::MemberVariable& variable, const Argument& object) -> void {
            const ValueType& value = variable.GetDyn(object).As<const ValueType&>(); // NOLINT
            Common::Serialize<ValueType>(stream, value);
        };
        params.deserializer = [](Common::DeserializeStream& stream, const Mirror::MemberVariable& variable, const Argument& object) -> void {
            ValueType value;
            Common::Deserialize<ValueType>(stream, value);
            Any valueRef = std::ref(value);
            variable.SetDyn(object, valueRef);
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceMemberVariable(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr>
    ClassRegistry<C>& ClassRegistry<C>::MemberFunction(const Id& inId)
    {
        using ClassType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ClassType;
        using ArgsTupleType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ArgsTupleType;
        using RetType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::RetType;

        const auto iter = clazz.memberFunctions.find(inId);
        Assert(iter == clazz.memberFunctions.end());

        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        MemberFunction::ConstructParams params;
        params.name = inId.name;
        params.retTypeInfo = GetTypeInfo<RetType>();
        params.argsNum = argsTupleSize;
        params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
        params.invoker = [](const Argument& object, const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());

            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            if constexpr (std::is_void_v<RetType>) {
                Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object.As<ClassType&>(), argsTuple, std::make_index_sequence<argsTupleSize> {});
                return {};
            } else {
                return { Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object.As<ClassType&>(), argsTuple, std::make_index_sequence<argsTupleSize> {}) };
            }
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceMemberFunction(inId, std::move(params)));
    }

    template <auto Ptr>
    GlobalRegistry& GlobalRegistry::Variable(const Id& inId)
    {
        using ValueType = typename Internal::VariableTraits<decltype(Ptr)>::ValueType;

        const auto iter = globalScope.variables.find(inId);
        Assert(iter == globalScope.variables.end());

        Variable::ConstructParams params;
        params.name = inId.name;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& argument) -> void {
            *Ptr = argument.As<const ValueType&>();
        };
        params.getter = []() -> Any {
            return { std::ref(*Ptr) };
        };
        params.serializer = [](Common::SerializeStream& stream, const Mirror::Variable& variable) -> void {
            const ValueType& value = variable.GetDyn().As<const ValueType&>(); // NOLINT
            Common::Serialize<ValueType>(stream, value);
        };
        params.deserializer = [](Common::DeserializeStream& stream, const Mirror::Variable& variable) -> void {
            ValueType value;
            Common::Deserialize<ValueType>(stream, value);
            variable.Set(value);
        };

        return SetContext(&globalScope.EmplaceVariable(inId, std::move(params)));
    }

    template <auto Ptr>
    GlobalRegistry& GlobalRegistry::Function(const Id& inId)
    {
        using ArgsTupleType = typename Internal::FunctionTraits<decltype(Ptr)>::ArgsTupleType;
        using RetType = typename Internal::FunctionTraits<decltype(Ptr)>::RetType;

        const auto iter = globalScope.functions.find(inId);
        Assert(iter == globalScope.functions.end());

        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        Function::ConstructParams params;
        params.name = inId.name;
        params.retTypeInfo = GetTypeInfo<RetType>();
        params.argsNum = argsTupleSize;
        params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
        params.invoker = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());

            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            if constexpr (std::is_void_v<RetType>) {
                Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {});
                return {};
            } else {
                return { Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}) };
            }
        };

        return SetContext(&globalScope.EmplaceFunction(inId, std::move(params)));
    }

    template <typename T>
    EnumRegistry<T>::EnumRegistry(Enum& inEnum)
        : MetaDataRegistry<EnumRegistry>(&inEnum), enumInfo(inEnum)
    {
    }

    template <typename T>
    EnumRegistry<T>::~EnumRegistry() = default;

    template <typename T>
    template <auto Value>
    EnumRegistry<T>& EnumRegistry<T>::Element(const Id& inId)
    {
        const auto iter = enumInfo.elements.find(inId);
        Assert(iter == enumInfo.elements.end());

        EnumElement::ConstructParams params;
        params.name = inId.name;
        params.getter = []() -> Any {
            return { Value };
        };
        params.comparer = [](const Argument& value) -> bool {
            return value.As<T>() == Value;
        };

        return MetaDataRegistry<EnumRegistry<T>>::SetContext(&enumInfo.EmplaceElement(inId, std::move(params)));
    }

    template <Common::CppClass C, CppBaseClassOrVoid<C> B>
    ClassRegistry<C> Registry::Class(const Id& inId)
    {
        const auto typeId = GetTypeInfo<C>()->id;
        Assert(!Class::typeToIdMap.contains(typeId));
        Assert(!classes.contains(inId));

        Class::ConstructParams params;
        params.name = inId.name;
        params.typeInfo = GetTypeInfo<C>();
        params.baseClassGetter = []() -> const Mirror::Class* {
            if constexpr (std::is_void_v<B>) {
                return nullptr;
            } else {
                return &Mirror::Class::Get<B>();
            }
        };
        if constexpr (std::is_default_constructible_v<C>) {
            params.defaultObjectCreator = []() -> Any {
                return { C() };
            };
        }
        if constexpr (std::is_destructible_v<C>) {
            Destructor::ConstructParams detorParams;
            detorParams.destructor = [](const Argument& object) -> void {
                object.As<C&>().~C();
            };
            params.destructorParams = detorParams;
        }
        if constexpr (std::is_default_constructible_v<C>) {
            Constructor::ConstructParams ctorParams;
            ctorParams.name = NamePresets::defaultCtor.name;
            ctorParams.argsNum = 0;
            ctorParams.argTypeInfos = {};
            ctorParams.stackConstructor = [](const ArgumentList& args) -> Any {
                Assert(args.size() == 0);
                return { C() };
            };
            ctorParams.heapConstructor = [](const ArgumentList& args) -> Any {
                Assert(args.size() == 0);
                return { new C() };
            };
            params.defaultConstructorParams = ctorParams;
        }

        Class::typeToIdMap[typeId] = inId;
        return ClassRegistry<C>(EmplaceClass(inId, std::move(params)));
    }

    template <Common::CppEnum T>
    EnumRegistry<T> Registry::Enum(const Id& inId)
    {
        const auto typeId = GetTypeInfo<T>()->id;
        Assert(!Enum::typeToIdMap.contains(typeId));
        Assert(!enums.contains(inId));

        Enum::ConstructParams params;
        params.name = inId.name;

        Enum::typeToIdMap[typeId] = inId;
        return EnumRegistry<T>(EmplaceEnum(inId, std::move(params)));
    }
}
