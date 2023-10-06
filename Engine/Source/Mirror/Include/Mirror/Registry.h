//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Common/Debug.h>
#include <Mirror/Api.h>
#include <Mirror/Type.h>
#include <Mirror/TypeInfo.h>

namespace Mirror::Internal {
    template <typename ArgsTuple, size_t... I>
    auto CastAnyArrayToArgsTuple(Any* args, std::index_sequence<I...>)
    {
        return ArgsTuple {args[I].As<std::tuple_element_t<I, ArgsTuple>>()... };
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
            constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;

            auto iter = clazz.constructors.find(inName);
            Assert(iter == clazz.constructors.end());

            clazz.constructors.emplace(std::make_pair(inName, Mirror::Constructor(
                inName,
                [](Any* args, size_t argSize) -> Any {
                    Assert(tupleSize == argSize);
                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    return Any(Internal::InvokeConstructorStack<C, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                },
                [](Any* args, size_t argSize) -> Any {
                    Assert(tupleSize == argSize);
                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    return Any(Internal::InvokeConstructorNew<C, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                }
            )));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.constructors.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& StaticVariable(const std::string& inName)
        {
            using ValueType = typename VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = clazz.staticVariables.find(inName);
            Assert(iter == clazz.staticVariables.end());

            clazz.staticVariables.emplace(std::make_pair(inName, Mirror::Variable(
                inName,
                [](Any* inValue) -> void {
                    *Ptr = inValue->As<ValueType>();
                },
                []() -> Any {
                    return Any(std::ref(*Ptr));
                },
                nullptr,
                nullptr
            )));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.staticVariables.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& StaticFunction(const std::string& inName)
        {
            using ArgsTupleType = typename FunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename FunctionTraits<decltype(Ptr)>::RetType;

            auto iter = clazz.staticFunctions.find(inName);
            Assert(iter == clazz.staticFunctions.end());

            clazz.staticFunctions.emplace(std::make_pair(inName, Mirror::Function(
                inName,
                [](Any* args, size_t argSize) -> Any {
                    constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;
                    Assert(tupleSize == argSize);

                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    if constexpr (std::is_void_v<RetType>) {
                        Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {});
                        return {};
                    } else {
                        return Any(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                    }
                }
            )));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.staticFunctions.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& MemberVariable(const std::string& inName)
        {
            using ClassType = typename MemberVariableTraits<decltype(Ptr)>::ClassType;
            using ValueType = typename MemberVariableTraits<decltype(Ptr)>::ValueType;

            auto iter = clazz.memberVariables.find(inName);
            Assert(iter == clazz.memberVariables.end());

            clazz.memberVariables.emplace(std::make_pair(inName, Mirror::MemberVariable(
                inName,
                sizeof(ValueType),
                [](Any* object, Any* value) -> void {
                    object->As<ClassType&>().*Ptr = value->As<ValueType>();
                },
                [](Any* object) -> Any {
                    return std::ref(object->As<ClassType&>().*Ptr);
                },
                [](Common::SerializeStream& stream, const Mirror::MemberVariable& variable, Any* object) -> void {
                    if constexpr (Common::Serializer<ValueType>::serializable) {
                        ValueType& value = variable.Get(object).As<ValueType&>();
                        Common::Serializer<ValueType>::Serialize(stream, value);
                    } else {
                        Unimplement();
                    }
                },
                [](Common::DeserializeStream& stream, const Mirror::MemberVariable& variable, Any* object) -> void {
                    if constexpr (Common::Serializer<ValueType>::serializable) {
                        ValueType value;
                        Common::Serializer<ValueType>::Deserialize(stream, value);
                        Mirror::Any valueRef = std::ref(value);
                        variable.Set(object, &valueRef);
                    } else {
                        Unimplement();
                    }
                }
            )));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.memberVariables.at(inName));
        }

        template <auto Ptr>
        ClassRegistry& MemberFunction(const std::string& inName)
        {
            using ClassType = typename MemberFunctionTraits<decltype(Ptr)>::ClassType;
            using ArgsTupleType = typename MemberFunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename MemberFunctionTraits<decltype(Ptr)>::RetType;

            auto iter = clazz.memberFunctions.find(inName);
            Assert(iter == clazz.memberFunctions.end());

            clazz.memberFunctions.emplace(std::make_pair(inName, Mirror::MemberFunction(
                inName,
                [](Any* object, Any* args, size_t argSize) -> Any {
                    constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;
                    Assert(tupleSize == argSize);

                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    if constexpr (std::is_void_v<RetType>) {
                        Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->As<ClassType&>(), argsTuple, std::make_index_sequence<tupleSize> {});
                        return {};
                    } else {
                        return Any(Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->As<ClassType&>(), argsTuple, std::make_index_sequence<tupleSize> {}));
                    }
                }
            )));
            return MetaDataRegistry<ClassRegistry<C>>::SetContext(&clazz.memberFunctions.at(inName));
        }

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass) : MetaDataRegistry<ClassRegistry<C>>(&inClass), clazz(inClass)
        {
            if constexpr (std::is_default_constructible_v<C>) {
                clazz.defaultObject = Mirror::Any(C());
            }
            if constexpr (std::is_destructible_v<C>) {
                clazz.destructor = Mirror::Destructor([](Any* object) -> void {
                    object->As<C&>().~C();
                });
            }
        }

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry : public MetaDataRegistry<GlobalRegistry> {
    public:
        ~GlobalRegistry() override = default;

        template <auto Ptr>
        GlobalRegistry& Variable(const std::string& inName)
        {
            using ValueType = typename VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = globalScope.variables.find(inName);
            Assert(iter == globalScope.variables.end());

            globalScope.variables.emplace(std::make_pair(inName, Mirror::Variable(
                inName,
                [](Any* inValue) -> void {
                    *Ptr = inValue->As<ValueType>();
                },
                []() -> Any {
                    return Any(std::ref(*Ptr));
                },
                [](Common::SerializeStream& stream, const Mirror::Variable& variable) -> void {
                    if constexpr (Common::Serializer<ValueType>::serializable) {
                        ValueType& value = variable.Get().As<ValueType&>();
                        Common::Serializer<ValueType>::Serialize(stream, value);
                    } else {
                        Unimplement();
                    }
                },
                [](Common::DeserializeStream& stream, const Mirror::Variable& variable) -> void {
                    if constexpr (Common::Serializer<ValueType>::serializable) {
                        ValueType value;
                        Common::Serializer<ValueType>::Deserialize(stream, value);
                        variable.Set(value);
                    } else {
                        Unimplement();
                    }
                }
            )));
            return MetaDataRegistry<GlobalRegistry>::SetContext(&globalScope.variables.at(inName));
        }

        template <auto Ptr>
        GlobalRegistry& Function(const std::string& inName)
        {
            using ArgsTupleType = typename FunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename FunctionTraits<decltype(Ptr)>::RetType;

            auto iter = globalScope.functions.find(inName);
            Assert(iter == globalScope.functions.end());

            globalScope.functions.emplace(std::make_pair(inName, Mirror::Function(
                inName,
                [](Any* args, size_t argSize) -> Any {
                    constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;
                    Assert(tupleSize == argSize);

                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    if constexpr (std::is_void_v<RetType>) {
                        Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {});
                        return {};
                    } else {
                        return Any(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                    }
                }
            )));
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

        template <typename C>
        requires std::is_class_v<C>
        ClassRegistry<C> Class(const std::string& name)
        {
            TypeId typeId = GetTypeInfo<C>()->id;
            Assert(!Class::typeToNameMap.contains(typeId));
            Assert(!classes.contains(name));

            Class::typeToNameMap[typeId] = name;
            classes.emplace(std::make_pair(name, Mirror::Class(name)));
            return ClassRegistry<C>(classes.at(name));
        }

        template <typename T>
        requires std::is_enum_v<T>
        EnumRegistry<T> Enum(const std::string& name)
        {
            TypeId typeId = GetTypeInfo<T>()->id;
            Assert(!Enum::typeToNameMap.contains(typeId));
            Assert(!enums.contains(name));

            Enum::typeToNameMap[typeId] = name;
            enums.emplace(std::make_pair(name, Mirror::Enum(name)));
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
