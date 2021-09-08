//
// Created by johnk on 2021/9/4.
//

#include <Mirror/Type.h>
#include <Mirror/Any.h>
#include <Mirror/Ref.h>

namespace {
    template <typename T, typename F, size_t... S>
    void TupleForEachInternal(T&& tuple, F&& consumer, std::index_sequence<S...>)
    {
        std::initializer_list<int> { (consumer(std::get<S>(tuple)), 0)... };
    }

    template <typename T, typename F>
    void TupleForEach(T&& tuple, F&& consumer)
    {
        ForEachTupleInternal(std::forward<T>(tuple), std::forward<F>(consumer), std::make_index_sequence<std::tuple_size<T>::value> {});
    }
}

namespace Explosion::Mirror {
    Type::Type(Internal::TypeInfo *info) : info(info) {}

    size_t Type::GetId()
    {
        return info->id;
    }

    const std::string& Type::GetName()
    {
        return info->name;
    }

    const std::string& Type::Meta(const std::string& key)
    {
        auto& metas = info->metas;
        auto iter = metas.find(key);
        if (iter != metas.end()) {
            throw NoSuchKeyException {};
        }
        return iter->second;
    }

    Variable::Variable(Internal::VariableInfo* info) : Type(info), info(info) {}

    Any Variable::Get(Ref instance)
    {
        return info->getter(instance);
    }

    void Variable::Set(Ref instance, Ref value)
    {
        info->setter(instance, value);
    }

    Function::Function(Internal::FunctionInfo* info) : Type(info), info(info) {}

    template<typename... Args>
    void Function::Invoke(Ref instance, Args&&... args)
    {
        std::vector<Ref> refs;
        TupleForEach(std::forward_as_tuple(args...), [&refs](auto&& value) -> void {
            refs.template emplace_back(std::forward<std::remove_reference_t<decltype(value)>>(value));
        });
        info->invoker(instance, refs);
    }

    Variable Function::GetReturnType()
    {
        return Variable(info->retType);
    }

    std::vector<Variable> Function::GetArgumentTypes()
    {
        std::vector<Variable> types;
        for (auto& type : info->argTypes) {
            types.emplace_back(Variable(type));
        }
        return types;
    }

    Struct::Struct(Internal::StructInfo* info) : Type(info), info(info) {}

    std::vector<Variable> Struct::GetMemberVariableTypes()
    {
        std::vector<Variable> types;
        for (auto& type : info->memberVarTypes) {
            types.emplace_back(Variable(type));
        }
        return types;
    }

    Class::Class(Internal::ClassInfo* info) : Struct(info), info(info) {}

    std::vector<Function> Class::GetMemberFunctionTypes()
    {
        std::vector<Function> types;
        for (auto& type : info->memberFuncTypes) {
            types.emplace_back(Function(type));
        }
        return types;
    }
}
