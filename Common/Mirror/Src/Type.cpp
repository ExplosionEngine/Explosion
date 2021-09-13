//
// Created by johnk on 2021/9/4.
//

#include <Mirror/Type.h>
#include <Mirror/Any.h>
#include <Mirror/Ref.h>

namespace {
    template <typename... Args, size_t ... I>
    void ForwardArgsAsRefVectorInternal(
        std::vector<Explosion::Mirror::Ref>& refs, const std::tuple<Args...>& tuple, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (refs[I] = Explosion::Mirror::Ref(&std::get<I>(tuple)), 0)... };
    }

    template <typename... Args>
    std::vector<Explosion::Mirror::Ref> ForwardArgsAsRefVector(const std::tuple<Args...>& args)
    {
        std::vector<Explosion::Mirror::Ref> refs;
        ForwardArgsAsRefVectorInternal(refs, args, std::make_index_sequence<sizeof...(Args)> {});
    }
}

namespace Explosion::Mirror {
    Type Variable::GetType()
    {
        return Type(info->type);
    }

    Any Variable::Get(Ref instance)
    {
        return info->getter(instance);
    }

    void Variable::Set(Ref instance, Ref value)
    {
        return info->setter(instance, value);
    }

    Type Function::GetType()
    {
        return Type(info->type);
    }

    template<typename... Args>
    Any Function::Invoke(Ref instance, Args&&... args)
    {
        return info->invoker(instance, ForwardArgsAsRefVector(std::forward_as_tuple(args...)));
    }
}
