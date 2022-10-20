//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Mirror/Api.h>
#include <Mirror/Type.h>
#include <Mirror/TypeInfo.h>
#include <Common/Debug.h>

namespace Mirror::Internal {
    template <typename ArgsTuple, size_t... I>
    auto CastAnyArrayToArgsTuple(Any* args, std::index_sequence<I...>)
    {
        return ArgsTuple { args[I].CastTo<std::tuple_element_t<I, ArgsTuple>>()... };
    }

    template <auto Ptr, typename ArgsTuple, size_t... I>
    auto InvokeFunction(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Ptr(std::get<I>(args)...);
    }

    template <typename Class, auto Ptr, typename ArgsTuple, size_t... I>
    auto InvokeMemberFunction(Class& object, ArgsTuple& args, std::index_sequence<I...>)
    {
        return object.*Ptr(std::get<I>(args)...);
    }
}

namespace Mirror {
    template <typename C>
    class MIRROR_API ClassRegistry {
    public:
        ~ClassRegistry() = default;

        template <typename... Args>
        ClassRegistry& Constructor(const std::string& inName)
        {
            // TODO
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
                    *Ptr = inValue->CastTo<ValueType>();
                },
                []() -> Any {
                    return Any(std::ref(*Ptr));
                }
            )));
            return *this;
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
            return *this;
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
                [](Any* object, Any* value) -> void {
                    object->CastTo<ClassType&>().*Ptr = value->CastTo<ValueType>();
                },
                [](Any* object) -> Any {
                    return std::ref(object->CastTo<ClassType&>().*Ptr);
                }
            )));
            return *this;
        }

        template <auto Ptr>
        ClassRegistry& MemberFunction(const std::string& inName)
        {
            using ClassType = typename MemberFunctionTraits<decltype(Ptr)>::ClassType;
            using ArgsTupleType = typename MemberFunctionTraits<decltype(Ptr)>::ArgsTupleType;
            using RetType = typename MemberFunctionTraits<decltype(Ptr)>::RetType;

            auto iter = clazz.memberFunctions.find(inName);
            Assert(iter == clazz.memberFunctions.end());

            clazz.memberFunctions[inName] = Mirror::MemberFunction(
                inName,
                [](Any* object, Any* args, size_t argSize) -> Any {
                    constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;
                    Assert(tupleSize == argSize);

                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    if constexpr (std::is_void_v<RetType>) {
                        Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->CastTo<ClassType&>(), argsTuple, std::make_index_sequence<tupleSize> {});
                        return {};
                    } else {
                        return Any::From(Internal::InvokeMemberFunction<ClassType, Ptr, ArgsTupleType>(object->CastTo<ClassType&>(), argsTuple, std::make_index_sequence<tupleSize> {}));
                    }
                }
            );
            return *this;
        }

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass) : clazz(inClass) {}

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry {
    public:
        ~GlobalRegistry() = default;

        template <auto Ptr>
        GlobalRegistry& Variable(const std::string& inName)
        {
            using ValueType = typename VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = globalScope.variables.find(inName);
            Assert(iter == globalScope.variables.end());

            globalScope.variables.emplace(std::make_pair(inName, Mirror::Variable(
                inName,
                [](Any* inValue) -> void {
                    *Ptr = inValue->CastTo<ValueType>();
                },
                []() -> Any {
                    return Any(std::ref(*Ptr));
                }
            )));
            return *this;
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
                        return Any::From(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                    }
                }
            )));
            return *this;
        }

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope) : globalScope(inGlobalScope) {}

        GlobalScope& globalScope;
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
        ClassRegistry<C> Class(const std::string& name)
        {
            classes[name] = Mirror::Class(name);
            return ClassRegistry<C>(classes[name]);
        }

    private:
        friend class GlobalScope;
        friend class Class;

        Registry() noexcept = default;

        GlobalScope globalScope;
        std::unordered_map<std::string, Mirror::Class> classes;
    };
}
