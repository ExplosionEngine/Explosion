//
// Created by johnk on 2021/9/10.
//

#include <tuple>

#include <Mirror/Factory.h>
#include <Mirror/Any.h>
#include <Mirror/Ref.h>

namespace Explosion::Mirror::Internal {
    template <typename... Args>
    std::vector<TypeInfo*> FetchArgTypeInfos(const std::tuple<Args...>&)
    {
        std::vector<TypeInfo*> typeInfos;
        std::initializer_list<int> { (typeInfos.template emplace_back(FetchTypeInfo<Args>()), 0)... };
        return typeInfos;
    }

    template <typename... Args, size_t... I>
    void FillTupleWithRefVector(std::tuple<Args...>& tuple, const std::vector<Ref>& refs, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (std::get<I>(tuple) = *static_cast<Args*>(refs[I].Value()), 0)... };
    }

    template <typename... Args>
    std::tuple<Args...> ForwardRefVectorAsTuple(const std::vector<Ref>& refs)
    {
        auto tuple = std::make_tuple<Args...>();
        FillTupleWithRefVector(tuple, refs, std::make_index_sequence<sizeof...(Args)> {});
        return tuple;
    }

    template <typename T, typename R, typename... Args, size_t... I>
    R InvokeFuncInternal(T&& func, const std::tuple<Args...>& args, std::index_sequence<I...>)
    {
        return func(std::get<I>(args)...);
    }

    template <typename T, typename R, typename... Args>
    R InvokeFunc(T&& func, const std::tuple<Args...>& args)
    {
        return InvokeFuncInternal<T, R>(std::forward<T>(func), args, std::make_index_sequence<sizeof...(Args)> {});
    }

    template <typename C, auto T, typename R, typename... Args, size_t... I>
    R InvokeMemberFuncInternal(C&& instance, const std::tuple<Args...>& args, std::index_sequence<I...>)
    {
        return instance.*T(std::get<I>(args)...);
    }

    template <typename C, auto T, typename R, typename... Args>
    R InvokeMemberFunc(C&& instance, const std::tuple<Args...>& args)
    {
        return InvokeFuncInternal<C, T, R>(std::forward<C>(instance), args, std::make_index_sequence<sizeof...(Args)> {});
    }

    template <typename T>
    struct MemberPointerTraits {};

    template <typename S, typename V>
    struct MemberPointerTraits<V S::*> {
        using ValueType = V;
    };

    template <typename S, typename R, typename... Args>
    struct MemberPointerTraits<R S::*(Args...)> {
        using RetType = R;
        using ArgsTupleType = std::tuple<Args...>;
    };
}

namespace Explosion::Mirror {
    template<typename Value>
    GlobalFactory& GlobalFactory::Variable(const std::string& name, Value* address)
    {
        variables[name] = std::make_unique<Internal::VariableInfo>(
            Internal::FetchTypeInfo<Value>(),
            [address](Ref instance) -> Any { return *address; },
            [address](Ref instance, Ref value) -> void { *address = *static_cast<Value*>(value.Value()); }
        );
        return *this;
    }

    template<typename Ret, typename... Args>
    GlobalFactory& GlobalFactory::Function(const std::string& name, Ret (*func)(Args...))
    {
        functions[name] = std::make_unique<Internal::FunctionInfo>(
            Internal::FetchTypeInfo<Ret>(),
            Internal::FetchArgTypeInfos(std::make_tuple<Args...>()),
            [func](Ref instance, std::vector<Ref> args) -> Any {
                auto argsTuple = Internal::ForwardRefVectorAsTuple<Args...>(args);
                return Internal::InvokeFunc<decltype(func)>(func, argsTuple);
            }
        );
        return *this;
    }

    template<typename S>
    template<auto T>
    StructFactory<S>& StructFactory<S>::Variable(const std::string& name)
    {
        using ValueType = typename Internal::MemberPointerTraits<decltype(T)>::ValueType;

        variables[name] = std::make_unique<Internal::VariableInfo>(
            Internal::FetchTypeInfo<ValueType>(),
            [](Ref instance) -> Any {
                return static_cast<S*>(instance.Value())->*T;
            },
            [](Ref instance, Ref value) -> Any {
                static_cast<S*>(instance.Value())->*T = *static_cast<ValueType*>(value.Value());
            }
        );
        return *this;
    }

    template<typename C>
    template<auto T>
    ClassFactory<C>& ClassFactory<C>::Function(const std::string& name)
    {
        using RetType = typename Internal::MemberPointerTraits<decltype(T)>::RetType;
        using ArgsTupleType = typename Internal::MemberPointerTraits<decltype(T)>::ArgsTupleType;

        functions[name] = std::make_unique<Internal::FunctionInfo>(
            Internal::FetchTypeInfo<RetType>(),
            Internal::FetchArgTypeInfos(ArgsTupleType {}),
            [](Ref instance, std::vector<Ref> args) -> Any {
                ArgsTupleType argsTuple {};
                Internal::FillTupleWithRefVector(argsTuple, args, std::make_index_sequence<ArgsTupleType::size()> {});
                return Internal::InvokeMemberFunc<C, T, RetType>(*static_cast<C*>(instance.Value()), argsTuple);
            }
        );
        return *this;
    }
}
