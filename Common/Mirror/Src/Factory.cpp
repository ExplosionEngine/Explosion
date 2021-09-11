//
// Created by johnk on 2021/9/10.
//

#include <Mirror/Factory.h>

namespace Explosion::Mirror {
    template<typename Value>
    GlobalFactory& GlobalFactory::Variable(const std::string&, Value* address)
    {
        // TODO
        return *this;
    }

    template<typename Ret, typename... Args>
    GlobalFactory& GlobalFactory::Function(const std::string&, Ret (*func)(Args...))
    {
        // TODO
        return *this;
    }

    template<typename S>
    template<auto T>
    StructFactory<S>& StructFactory<S>::Variable(const std::string& name)
    {
        // TODO
        return *this;
    }

    template<typename C>
    template<auto T>
    ClassFactory<C>& ClassFactory<C>::Function(const std::string& name)
    {
        // TODO
        return *this;
    }
}
