//
// Created by johnk on 2021/9/10.
//

#ifndef EXPLOSION_FACTORY_H
#define EXPLOSION_FACTORY_H

#include <string>
#include <memory>
#include <unordered_map>

#include <Mirror/Type.h>
#include <Mirror/Any.h>
#include <Mirror/Ref.h>

namespace Explosion::Mirror::Internal {
    template <typename... Args>
    std::vector<const TypeInfo*> FetchArgTypeInfos(const std::tuple<Args...>&)
    {
        std::vector<const TypeInfo*> typeInfos;
        std::initializer_list<int> { (typeInfos.emplace_back(FetchTypeInfo<Args>()), 0)... };
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
        auto tuple = std::tuple<Args...> {};
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
        return InvokeMemberFuncInternal<C, T, R>(std::forward<C>(instance), args, std::make_index_sequence<sizeof...(Args)> {});
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
    class GlobalFactory {
    public:
        GlobalFactory() = default;
        GlobalFactory(const GlobalFactory&) = delete;
        GlobalFactory& operator=(const GlobalFactory&) = delete;
        ~GlobalFactory() = default;

        static GlobalFactory& Singleton()
        {
            static GlobalFactory instance;
            return instance;
        }

        template <typename Value>
        GlobalFactory& DefineVariable(const std::string& name, Value* address)
        {
            variables[name] = std::make_unique<Internal::VariableInfo>(Internal::VariableInfo {
                Internal::FetchTypeInfo<Value>(),
                [address](Ref instance) -> Any { return Any(*address); },
                [address](Ref instance, Ref value) -> void { *address = *static_cast<Value*>(value.Value()); }
            });
            return *this;
        }

        template <typename Ret, typename... Args>
        GlobalFactory& DefineFunction(const std::string& name, Ret(*func)(Args...))
        {
            functions[name] = std::make_unique<Internal::FunctionInfo>(Internal::FunctionInfo {
                Internal::FetchTypeInfo<Ret(*)(Args...)>(),
                Internal::FetchTypeInfo<Ret>(),
                Internal::FetchArgTypeInfos(std::tuple<Args...> {}),
                [func](Ref instance, std::vector<Ref> args) -> Any {
                    auto argsTuple = Internal::ForwardRefVectorAsTuple<Args...>(args);
                    return Any(Internal::InvokeFunc<std::function<Ret(Args...)>, Ret, Args...>(func, argsTuple));
                }
            });
            return *this;
        }

        Variable GetVariable(const std::string& name);
        void ForEachVariable(const std::function<void(Variable)>& iter);
        Function GetFunction(const std::string& name);
        void ForEachFunction(const std::function<void(Function)>& iter);

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::VariableInfo>> variables;
        std::unordered_map<std::string, std::unique_ptr<Internal::FunctionInfo>> functions;
    };

    template <typename S>
    class StructFactory {
    public:
        StructFactory() = default;
        StructFactory(const StructFactory<S>&) = delete;
        StructFactory& operator=(const StructFactory<S>&) = delete;
        virtual ~StructFactory() = default;

        static StructFactory<S> Singleton()
        {
            static StructFactory<S> instance;
            return instance;
        }

        template <auto T>
        StructFactory<S>& MemberVariable(const std::string& name)
        {
            using ValueType = typename Internal::MemberPointerTraits<decltype(T)>::ValueType;

            variables[name] = std::make_unique<Internal::VariableInfo>(
                Internal::FetchTypeInfo<ValueType>(),
                [](Ref instance) -> Any {
                    return Any(static_cast<S*>(instance.Value())->*T);
                },
                [](Ref instance, Ref value) -> Any {
                    return Any(static_cast<S*>(instance.Value())->*T = *static_cast<ValueType*>(value.Value()));
                }
            );
            return *this;
        }

        Variable GetMemberVariable(const std::string& name);
        void ForEachVariable(const std::function<void(Variable)>& iter);

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::VariableInfo>> variables;
    };

    template <typename C>
    class ClassFactory : public StructFactory<C> {
    public:
        ClassFactory() = default;
        ClassFactory(const ClassFactory<C>&) = delete;
        ClassFactory& operator=(const ClassFactory<C>&) = delete;
        ~ClassFactory() override = default;

        static ClassFactory<C> Singleton()
        {
            static ClassFactory<C> instance;
            return instance;
        }

        template <auto T>
        ClassFactory<C>& MemberFunction(const std::string& name)
        {
            using RetType = typename Internal::MemberPointerTraits<decltype(T)>::RetType;
            using ArgsTupleType = typename Internal::MemberPointerTraits<decltype(T)>::ArgsTupleType;

            functions[name] = std::make_unique<Internal::FunctionInfo>(
                Internal::FetchTypeInfo<decltype(T)>(),
                Internal::FetchTypeInfo<RetType>(),
                Internal::FetchArgTypeInfos(ArgsTupleType {}),
                [](Ref instance, std::vector<Ref> args) -> Any {
                    ArgsTupleType argsTuple {};
                    Internal::FillTupleWithRefVector(argsTuple, args, std::make_index_sequence<ArgsTupleType::size()> {});
                    return Any(Internal::InvokeMemberFunc<C, T, RetType>(*static_cast<C*>(instance.Value()), argsTuple));
                }
            );
            return *this;
        }

        Function GetMemberFunction(const std::string& name);
        void ForEachMemberFunction(const std::function<void(Function)>& iter);

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::FunctionInfo>> functions;
    };
}

#endif//EXPLOSION_FACTORY_H
