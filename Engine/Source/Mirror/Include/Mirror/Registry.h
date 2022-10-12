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
        return ArgsTuple { args->CastTo<std::tuple_element_t<I, ArgsTuple>>()... };
    }

    template <auto Ptr, typename ArgsTuple, size_t... I>
    auto InvokeFunction(ArgsTuple& args, std::index_sequence<I...>)
    {
        return Ptr(std::get<I>(args)...);
    }
}

namespace Mirror {
    template <typename Registry>
    class MIRROR_API MetaDataRegistry {
    public:
        ~MetaDataRegistry() = default;

        Registry& Meta(const std::string& inKey, const std::string& inValue)
        {
            type.metas[inKey] = inValue;
            return *static_cast<Registry*>(this);
        }

    protected:
        explicit MetaDataRegistry(Type& inType) : type(inType) {}

    private:
        Type& type;
    };

    template <typename C>
    class MIRROR_API ClassRegistry : public MetaDataRegistry<ClassRegistry<C>> {
    public:
        ~ClassRegistry() = default;

        template <auto Ptr>
        ClassRegistry& StaticVariable(const std::string& inName)
        {
            // TODO
        }

        template <auto Ptr>
        ClassRegistry& StaticFunction(const std::string& inName)
        {
            // TODO
        }

        template <auto Ptr>
        ClassRegistry& MemberVariable(const std::string& inName)
        {
            // TODO
        }

        template <auto Ptr>
        ClassRegistry& MemberFunction(const std::string& inName)
        {
            // TODO
        }

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass) : MetaDataRegistry<ClassRegistry<C>>(inClass), clazz(inClass) {}

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry : public MetaDataRegistry<GlobalRegistry> {
    public:
        ~GlobalRegistry() = default;

        template <auto Ptr>
        GlobalRegistry& Variable(const std::string& inName)
        {
            using ValueType = typename VariableTraits<decltype(Ptr)>::ValueType;

            auto iter = globalScope.variables.find(inName);
            Assert(iter != globalScope.variables.end());

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
            using RetType = typename FunctionTraits<decltype(Ptr)>::RetType;
            using ArgsTupleType = typename FunctionTraits<decltype(Ptr)>::ArgsTupleType;

            auto iter = globalScope.functions.find(inName);
            Assert(iter != globalScope.functions.end());

            globalScope.functions.emplace(std::make_pair(inName, Mirror::Function(
                inName,
                [](Any* args, size_t argSize) -> Any {
                    constexpr size_t tupleSize = std::tuple_size_v<ArgsTupleType>;
                    Assert(tupleSize == argSize);

                    auto argsTuple = Internal::CastAnyArrayToArgsTuple<ArgsTupleType>(args, std::make_index_sequence<tupleSize> {});
                    return Any(Internal::InvokeFunction<Ptr, ArgsTupleType>(argsTuple, std::make_index_sequence<tupleSize> {}));
                }
            )));
            return *this;
        }

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope) : MetaDataRegistry<GlobalRegistry>(inGlobalScope), globalScope(inGlobalScope) {}

        GlobalScope& globalScope;
    };

    class MIRROR_API Registry {
    public:
        static Registry Get()
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
        Registry() noexcept = default;

        GlobalScope globalScope;
        std::unordered_map<std::string, Mirror::Class> classes;
    };
}
