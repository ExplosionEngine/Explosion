//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Common/Debug.h>
#include <Mirror/Api.h>
#include <Mirror/Mirror.h>

namespace Mirror::Internal {
    template <typename T>
    struct VariableTraits {};

    template <typename T>
    struct FunctionTraits {};

    template <typename T>
    struct MemberVariableTraits {};

    template <typename T>
    struct MemberFunctionTraits {};

    template <typename ArgsTuple, size_t... I>
    auto GetArgTypeInfosByArgsTuple(std::index_sequence<I...>)
    {
        return std::vector<const TypeInfo*> { GetTypeInfo<std::tuple_element_t<I, ArgsTuple>>()... };
    }

    template <typename ArgsTuple, size_t... I>
    auto CastAnyArrayToArgsTuple(Any* args, std::index_sequence<I...>)
    {
        return ArgsTuple { args[I].As<std::tuple_element_t<I, ArgsTuple>>()... };
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
    class MetaDataRegistry {
    public:
        virtual ~MetaDataRegistry() = default;

        Derived& MetaData(const std::string& key, const std::string& value)
        {
            context->metas[key] = value;
            return static_cast<Derived&>(*this);
        }

    protected:
        explicit MetaDataRegistry(Type* inContext) : context(inContext)
        {
            Assert(context);
        }

        Derived& SetContext(Type* inContext)
        {
            Assert(inContext);
            context = inContext;
            return static_cast<Derived&>(*this);
        }

    private:
        Type* context;
    };

    template <typename C>
    class ClassRegistry : public MetaDataRegistry<ClassRegistry<C>> {
    public:
        ~ClassRegistry() override = default;

        template <typename... Args>
        ClassRegistry& Constructor(const std::string& inName)
        {
            using ArgsTupleType = std::tuple<Args...>;
            constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

            auto iter = clazz.constructors.find(inName);
            Assert(iter == clazz.constructors.end());

            Mirror::Constructor::ConstructParams params;
            params.name = inName;
            params.argsNum = sizeof...(Args);
            params.argTypeInfos = std::vector<const TypeInfo*> { GetTypeInfo<Args>()... };
            params.stackConstructor = [](Any* args, uint8_t argSize) -> Any {
                Assert(argsTupleSize == argSize);
                auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
                return Any(Internal::InvokeConstructorStack<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
            };
            params.heapConstructor = [](Any* args, size_t argSize) -> Any {
                Assert(argsTupleSize == argSize);
                auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
                return Any(Internal::InvokeConstructorNew<C, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
            };

            clazz.constructors.emplace(std::make_pair(inName, Mirror::Constructor(std::move(params))));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.constructors.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& StaticVariable(const std::string& inName)
        {
            using ValueType = typename Internal::VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = clazz.staticVariables.find(inName);
            Assert(iter == clazz.staticVariables.end());

            Variable::ConstructParams params;
            params.name = inName;
            params.memorySize = sizeof(ValueType);
            params.typeInfo = GetTypeInfo<ValueType>();
            params.setter = [](Any* inValue) -> void {
                *Ptr = inValue->As<ValueType>();
            };
            params.getter = []() -> Any {
                return Any(std::ref(*Ptr));
            };
            params.serializer = nullptr;
            params.deserializer = nullptr;

            clazz.staticVariables.emplace(std::make_pair(inName, Variable(std::move(params))));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.staticVariables.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& StaticFunction(const std::string& inName)
        {
            using ArgsTupleType = typename Internal::FunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename Internal::FunctionTraits<decltype(Ptr)>::RetType;

            auto iter = clazz.staticFunctions.find(inName);
            Assert(iter == clazz.staticFunctions.end());

            constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

            Function::ConstructParams params;
            params.name = inName;
            params.retTypeInfo = GetTypeInfo<RetType>();
            params.argsNum = argsTupleSize;
            params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
            params.invoker = [](Any* args, size_t argSize) -> Any {
                Assert(argsTupleSize == argSize);

                auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
                if constexpr (std::is_void_v<RetType>) {
                    Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {});
                    return {};
                } else {
                    return Any(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
                }
            };

            clazz.staticFunctions.emplace(std::make_pair(inName, Function(std::move(params))));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.staticFunctions.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& MemberVariable(const std::string& inName)
        {
            using ClassType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ClassType;
            using ValueType = typename Internal::MemberVariableTraits<decltype(Ptr)>::ValueType;

            auto iter = clazz.memberVariables.find(inName);
            Assert(iter == clazz.memberVariables.end());

            Mirror::MemberVariable::ConstructParams params;
            params.name = inName;
            params.memorySize = sizeof(ValueType);
            params.typeInfo = GetTypeInfo<ValueType>();
            params.setter = [](Any* object, Any* value) -> void {
                object->As<ClassType&>().*Ptr = value->As<ValueType>();
            };
            params.getter = [](Any* object) -> Any {
                return std::ref(object->As<ClassType&>().*Ptr);
            };
            params.serializer = [](Common::SerializeStream& stream, const Mirror::MemberVariable& variable, Any* object) -> void {
                if constexpr (Common::Serializer<ValueType>::serializable) {
                    ValueType& value = variable.Get(object).As<ValueType&>();
                    Common::Serializer<ValueType>::Serialize(stream, value);
                } else {
                    Unimplement();
                }
            };
            params.deserializer = [](Common::DeserializeStream& stream, const Mirror::MemberVariable& variable, Any* object) -> void {
                if constexpr (Common::Serializer<ValueType>::serializable) {
                    ValueType value;
                    Common::Serializer<ValueType>::Deserialize(stream, value);
                    Any valueRef = std::ref(value);
                    variable.Set(object, &valueRef);
                } else {
                    Unimplement();
                }
            };

            clazz.memberVariables.emplace(std::make_pair(inName, Mirror::MemberVariable(std::move(params))));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.memberVariables.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& MemberFunction(const std::string& inName)
        {
            using ClassType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ClassType;
            using ArgsTupleType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename Internal::MemberFunctionTraits<decltype(Ptr)>::RetType;

            auto iter = clazz.memberFunctions.find(inName);
            Assert(iter == clazz.memberFunctions.end());

            constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

            Mirror::MemberFunction::ConstructParams params;
            params.name = inName;
            params.retTypeInfo = GetTypeInfo<RetType>();
            params.argsNum = argsTupleSize;
            params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
            params.invoker = [](Any* object, Any* args, size_t argSize) -> Any {
                Assert(argsTupleSize == argSize);

                auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
                if constexpr (std::is_void_v<RetType>) {
                    Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->As<ClassType&>(), argsTuple, std::make_index_sequence<argsTupleSize> {});
                    return {};
                } else {
                    return Any(Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->As<ClassType&>(), argsTuple, std::make_index_sequence<argsTupleSize> {}));
                }
            };

            clazz.memberFunctions.emplace(std::make_pair(inName, Mirror::MemberFunction(std::move(params))));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.memberFunctions.at(inName));
        }

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass) : MetaDataRegistry<ClassRegistry<C>>(&inClass), clazz(inClass)
        {
        }

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry : public MetaDataRegistry<GlobalRegistry> {
    public:
        ~GlobalRegistry() override = default;

        template <auto Ptr>
        GlobalRegistry& Variable(const std::string& inName)
        {
            using ValueType = typename Internal::VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = globalScope.variables.find(inName);
            Assert(iter == globalScope.variables.end());

            Mirror::Variable::ConstructParams params;
            params.name = inName;
            params.memorySize = sizeof(ValueType);
            params.typeInfo = GetTypeInfo<ValueType>();
            params.setter = [](Any* inValue) -> void {
                *Ptr = inValue->As<ValueType>();
            };
            params.getter = []() -> Any {
                return Any(std::ref(*Ptr));
            };
            params.serializer = [](Common::SerializeStream& stream, const Mirror::Variable& variable) -> void {
                if constexpr (Common::Serializer<ValueType>::serializable) {
                    ValueType& value = variable.Get().As<ValueType&>();
                    Common::Serializer<ValueType>::Serialize(stream, value);
                } else {
                    Unimplement();
                }
            };
            params.deserializer = [](Common::DeserializeStream& stream, const Mirror::Variable& variable) -> void {
                if constexpr (Common::Serializer<ValueType>::serializable) {
                    ValueType value;
                    Common::Serializer<ValueType>::Deserialize(stream, value);
                    variable.Set(value);
                } else {
                    Unimplement();
                }
            };

            globalScope.variables.emplace(std::make_pair(inName, Mirror::Variable(std::move(params))));
            return MetaDataRegistry<GlobalRegistry>::SetContext(&globalScope.variables.at(inName));
        }

        template <auto Ptr>
        GlobalRegistry& Function(const std::string& inName)
        {
            using ArgsTupleType = typename Internal::FunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename Internal::FunctionTraits<decltype(Ptr)>::RetType;

            auto iter = globalScope.functions.find(inName);
            Assert(iter == globalScope.functions.end());

            constexpr size_t argsTupleSize = std::tuple_size_v<ArgsTupleType>;

            Mirror::Function::ConstructParams params;
            params.name = inName;
            params.retTypeInfo = GetTypeInfo<RetType>();
            params.argsNum = argsTupleSize;
            params.argTypeInfos = Internal::GetArgTypeInfosByArgsTuple<ArgsTupleType>(std::make_index_sequence<argsTupleSize> {});
            params.invoker = [](Any* args, size_t argSize) -> Any {
                Assert(argsTupleSize == argSize);

                auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<argsTupleSize> {});
                if constexpr (std::is_void_v<RetType>) {
                    Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {});
                    return {};
                } else {
                    return Any(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<argsTupleSize> {}));
                }
            };

            globalScope.functions.emplace(std::make_pair(inName, Mirror::Function(std::move(params))));
            return MetaDataRegistry<GlobalRegistry>::SetContext(&globalScope.functions.at(inName));
        }

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope) : MetaDataRegistry<GlobalRegistry>(&inGlobalScope), globalScope(inGlobalScope) {}

        GlobalScope& globalScope;
    };

    template <typename T>
    class EnumRegistry : public MetaDataRegistry<EnumRegistry<T>> {
    public:
        ~EnumRegistry() override = default;

        template <auto Value>
        EnumRegistry& Element(const std::string& inName)
        {
            auto iter = enumInfo.elements.find(inName);
            Assert(iter == enumInfo.elements.end());

            enumInfo.elements.emplace(std::make_pair(inName, EnumElement(
                inName,
                []() -> Any {
                    return Any(Value);
                },
                [](Any* value) -> bool {
                    return value->As<T>() == Value;
                }
            )));
            return MetaDataRegistry<EnumRegistry<T>>::SetContext(&enumInfo.elements.at(inName));
        }

    private:
        friend class Registry;

        explicit EnumRegistry(Enum& inEnum) : MetaDataRegistry<EnumRegistry<T>>(&inEnum), enumInfo(inEnum) {}

        Enum& enumInfo;
    };

    class MIRROR_API Registry {
    public:
        static Registry& Get()
        {
            static Registry instance;
            return instance;
        }

        ~Registry() = default;

        GlobalRegistry Global()
        {
            return GlobalRegistry(globalScope);
        }

        template <typename C, typename B = void>
        requires std::is_class_v<C> && (std::is_void_v<B> || (std::is_class_v<B> && std::is_base_of_v<B, C>))
        ClassRegistry<C> Class(const std::string& name)
        {
            TypeId typeId = GetTypeInfo<C>()->id;
            Assert(!Class::typeToNameMap.contains(typeId));
            Assert(!classes.contains(name));

            Mirror::Class::ConstructParams params;
            params.name = name;
            params.typeInfo = GetTypeInfo<C>();
            params.baseClassGetter = []() -> const Mirror::Class* {
                if constexpr (std::is_void_v<B>) {
                    return nullptr;
                } else {
                    return &Mirror::Class::Get<B>();
                }
            };
            if constexpr (std::is_default_constructible_v<C>) {
                params.defaultObject = Any(C());
            }
            if constexpr (std::is_destructible_v<C>) {
                Destructor::ConstructParams detorParams;
                detorParams.destructor = [](Any* object) -> void {
                    object->As<C&>().~C();
                };
                params.destructor = Destructor(std::move(detorParams));
            }
            if constexpr (std::is_default_constructible_v<C>) {
                Constructor::ConstructParams ctorParams;
                ctorParams.name = NamePresets::defaultConstructor;
                ctorParams.argsNum = 0;
                ctorParams.argTypeInfos = {};
                ctorParams.stackConstructor = [](Any* args, size_t argSize) -> Any {
                    Assert(argSize == 0);
                    return Any(C());
                };
                ctorParams.heapConstructor = [](Any* args, size_t argSize) -> Any {
                    Assert(argSize == 0);
                    return Any(new C());
                };
                params.defaultConstructor = Constructor(std::move(ctorParams));
            }

            Class::typeToNameMap[typeId] = name;
            classes.emplace(std::make_pair(name, Mirror::Class(std::move(params))));
            return ClassRegistry<C>(classes.at(name));
        }

        template <typename T>
        requires std::is_enum_v<T>
        EnumRegistry<T> Enum(const std::string& name)
        {
            TypeId typeId = GetTypeInfo<T>()->id;
            Assert(!Enum::typeToNameMap.contains(typeId));
            Assert(!enums.contains(name));

            Mirror::Enum::ConstructParams params;
            params.name = name;

            Enum::typeToNameMap[typeId] = name;
            enums.emplace(std::make_pair(name, Mirror::Enum(std::move(params))));
            return EnumRegistry<T>(enums.at(name));
        }

    private:
        friend class GlobalScope;
        friend class Class;
        friend class Enum;

        Registry() noexcept = default;

        GlobalScope globalScope;
        std::unordered_map<std::string, Mirror::Class> classes;
        std::unordered_map<std::string, Mirror::Enum> enums;
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
}
