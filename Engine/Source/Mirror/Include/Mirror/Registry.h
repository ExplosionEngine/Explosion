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
    template <auto Ptr, typename ArgsTuple, size_t... I> decltype(auto) InvokeFunction(ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, auto Ptr, typename ArgsTuple, size_t... I> decltype(auto) InvokeMemberFunction(Class& object, ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, typename ArgsTuple, size_t... I> decltype(auto) InvokeConstructorStack(ArgsTuple& args, std::index_sequence<I...>);
    template <typename Class, typename ArgsTuple, size_t... I> decltype(auto) InvokeConstructorNew(ArgsTuple& args, std::index_sequence<I...>);
}

namespace Mirror {
    template <typename Derived>
    class MetaDataRegistry {
    public:
        virtual ~MetaDataRegistry();

        Derived& MetaData(const Id& inKey, const std::string& inValue);

    protected:
        explicit MetaDataRegistry(ReflNode* inContext);

        Derived& SetContext(ReflNode* inContext);

    private:
        ReflNode* context;
    };

    template <typename C>
    class ClassRegistry final : public MetaDataRegistry<ClassRegistry<C>> {
    public:
        ~ClassRegistry() override;

        template <typename... Args, FieldAccess Access = FieldAccess::faPublic> ClassRegistry& Constructor(const Id& inId);
        template <auto Ptr, FieldAccess Access = FieldAccess::faPublic> ClassRegistry& StaticVariable(const Id& inId);
        template <auto Ptr, FieldAccess Access = FieldAccess::faPublic> ClassRegistry& StaticFunction(const Id& inId);
        template <auto Ptr, FieldAccess Access = FieldAccess::faPublic> ClassRegistry& MemberVariable(const Id& inId);
        // TODO support overload
        // TODO virtual function support
        template <auto Ptr, FieldAccess Access = FieldAccess::faPublic> ClassRegistry& MemberFunction(const Id& inId);

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
        // TODO overload support

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope);

        GlobalScope& globalScope;
    };

    template <typename T>
    class EnumRegistry final : public MetaDataRegistry<EnumRegistry<T>> {
    public:
        ~EnumRegistry() override;

        template <T V> EnumRegistry& Value(const Id& inId);

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

        template <Common::CppClass C, CppBaseClassOrVoid<C> B = void, FieldAccess DefaultCtorAccess = FieldAccess::faPublic, FieldAccess DetorAccess = FieldAccess::faPublic>
        ClassRegistry<C> Class(const Id& inId);

        template <Common::CppEnum T> EnumRegistry<T>
        Enum(const Id& inId);

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
    decltype(auto) InvokeFunction(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Ptr(std::get<I>(args)...);
    }

    template <typename Class, auto Ptr, typename ArgsTuple, size_t... I>
    decltype(auto) InvokeMemberFunction(Class& object, ArgsTuple& args, std::index_sequence<I...>)
    {
        return (object.*Ptr)(std::get<I>(args)...);
    }

    template <typename Class, typename ArgsTuple, size_t... I>
    decltype(auto) InvokeConstructorStack(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Class(std::get<I>(args)...);
    }

    template <typename Class, typename ArgsTuple, size_t... I>
    decltype(auto) InvokeConstructorNew(ArgsTuple& args, std::index_sequence<I...>)
    {
        return new Class(std::get<I>(args)...);
    }
}

namespace Mirror {
    template <typename Derived>
    MetaDataRegistry<Derived>::MetaDataRegistry(ReflNode* inContext)
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
    Derived& MetaDataRegistry<Derived>::SetContext(ReflNode* inContext)
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
    template <typename... Args, FieldAccess Access>
    ClassRegistry<C>& ClassRegistry<C>::Constructor(const Id& inId)
    {
        using ArgsTupleType = std::tuple<Args...>;
        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        const auto iter = clazz.constructors.find(inId);
        Assert(iter == clazz.constructors.end());

        Constructor::ConstructParams params;
        params.id = inId;
        params.owner = clazz.GetId();
        params.access = Access;
        params.argsNum = sizeof...(Args);
        params.argTypeInfos = { GetTypeInfo<Args>()... };
        params.argRemoveRefTypeInfos = { GetTypeInfo<std::remove_reference_t<Args>>()... };
        params.argRemovePointerTypeInfos = { GetTypeInfo<std::remove_pointer_t<Args>>()... };
        params.stackConstructor = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());
            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            return Internal::ForwardAsAny(Internal::InvokeConstructorStack<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
        };
        params.heapConstructor = [](const ArgumentList& args) -> Any {
            Assert(argsTupleSize == args.size());
            auto argsTuple = Internal::ForwardArgumentsListAsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
            return Internal::ForwardAsAny(Internal::InvokeConstructorNew<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceConstructor(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr, FieldAccess Access>
    ClassRegistry<C>& ClassRegistry<C>::StaticVariable(const Id& inId)
    {
        using ValueType = typename Internal::VariableTraits<decltype(Ptr)>::ValueType;

        const auto iter = clazz.staticVariables.find(inId);
        Assert(iter == clazz.staticVariables.end());

        Variable::ConstructParams params;
        params.id = inId;
        params.owner = clazz.GetId();
        params.access = Access;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& value) -> void {
            if constexpr (!std::is_const_v<ValueType>) {
                *Ptr = value.As<const ValueType&>();
            } else {
                QuickFail();
            }
        };
        params.getter = []() -> Any {
            return { std::ref(*Ptr) };
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceStaticVariable(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr, FieldAccess Access>
    ClassRegistry<C>& ClassRegistry<C>::StaticFunction(const Id& inId)
    {
        using ArgsTupleType = typename Internal::FunctionTraits<decltype(Ptr)>::ArgsTupleType;
        using RetType = typename Internal::FunctionTraits<decltype(Ptr)>::RetType;

        const auto iter = clazz.staticFunctions.find(inId);
        Assert(iter == clazz.staticFunctions.end());

        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        Function::ConstructParams params;
        params.id = inId;
        params.owner = clazz.GetId();
        params.access = Access;
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
                return Internal::ForwardAsAny(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
            }
        };

        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceStaticFunction(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr, FieldAccess Access>
    ClassRegistry<C>& ClassRegistry<C>::MemberVariable(const Id& inId)
    {
        using ClassType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ClassType;
        using ValueType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ValueType;

        const auto iter = clazz.memberVariables.find(inId);
        Assert(iter == clazz.memberVariables.end());

        MemberVariable::ConstructParams params;
        params.id = inId;
        params.owner = clazz.GetId();
        params.access = Access;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& object, const Argument& value) -> void {
            Assert(!object.IsConstRef());
            object.As<ClassType&>().*Ptr = value.As<const ValueType&>();
        };
        params.getter = [](const Argument& object) -> Any {
            if (object.IsConstRef()) {
                return { std::ref(object.As<const ClassType&>().*Ptr) };
            }
            return { std::ref(object.As<ClassType&>().*Ptr) };
        };
        return MetaDataRegistry<ClassRegistry>::SetContext(&clazz.EmplaceMemberVariable(inId, std::move(params)));
    }

    template <typename C>
    template <auto Ptr, FieldAccess Access>
    ClassRegistry<C>& ClassRegistry<C>::MemberFunction(const Id& inId)
    {
        // ClassType here contains const, #see Internal::MemberFunctionTraits
        using ClassType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ClassType;
        using ArgsTupleType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ArgsTupleType;
        using RetType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::RetType;

        const auto iter = clazz.memberFunctions.find(inId);
        Assert(iter == clazz.memberFunctions.end());

        constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

        MemberFunction::ConstructParams params;
        params.id = inId;
        params.owner = clazz.GetId();
        params.access = Access;
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
                return Internal::ForwardAsAny(Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object.As<ClassType&>(), argsTuple, std::make_index_sequence<argsTupleSize> {}));
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
        params.id = inId;
        params.owner = Id::null;
        params.access = FieldAccess::max;
        params.memorySize = sizeof(ValueType);
        params.typeInfo = GetTypeInfo<ValueType>();
        params.setter = [](const Argument& argument) -> void {
            if (!std::is_const_v<ValueType>) {
                *Ptr = argument.As<const ValueType&>();
            } else {
                QuickFail();
            }
        };
        params.getter = []() -> Any {
            return { std::ref(*Ptr) };
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
        params.id = inId;
        params.owner = Id::null;
        params.access = FieldAccess::max;
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
                return Internal::ForwardAsAny(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
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
    template <T V>
    EnumRegistry<T>& EnumRegistry<T>::Value(const Id& inId)
    {
        const auto iter = enumInfo.values.find(inId);
        Assert(iter == enumInfo.values.end());

        EnumValue::ConstructParams params;
        params.id = inId;
        params.owner = enumInfo.GetId();
        params.getter = []() -> Any {
            return { V };
        };
        params.integralGetter = []() -> EnumValue::IntegralValue {
            return static_cast<EnumValue::IntegralValue>(V);
        };
        params.setter = [](const Argument& value) -> void {
            value.As<T&>() = V;
        };
        params.comparer = [](const Argument& value) -> bool {
            return value.As<T>() == V;
        };

        return MetaDataRegistry<EnumRegistry<T>>::SetContext(&enumInfo.EmplaceElement(inId, std::move(params)));
    }

    template <Common::CppClass C, CppBaseClassOrVoid<C> B, FieldAccess DefaultCtorAccess, FieldAccess DetorAccess>
    ClassRegistry<C> Registry::Class(const Id& inId)
    {
        const auto typeId = GetTypeInfo<C>()->id;
        Assert(!Class::typeToIdMap.contains(typeId));
        Assert(!classes.contains(inId));

        Class::ConstructParams params;
        params.id = inId;
        params.typeInfo = GetTypeInfo<C>();
        params.memorySize = sizeof(C);
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
            detorParams.owner = inId;
            detorParams.access = DetorAccess;
            detorParams.destructor = [](const Argument& object) -> void {
                Assert(!object.IsConstRef());
                object.As<C&>().~C();
            };
            params.destructorParams = detorParams;
        }
        if constexpr (std::is_default_constructible_v<C>) {
            Constructor::ConstructParams ctorParams;
            ctorParams.id = IdPresets::defaultCtor.name;
            ctorParams.owner = inId;
            ctorParams.access = DefaultCtorAccess;
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
        params.id = inId;

        Enum::typeToIdMap[typeId] = inId;
        return EnumRegistry<T>(EmplaceEnum(inId, std::move(params)));
    }
}
