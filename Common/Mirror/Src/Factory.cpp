//
// Created by johnk on 2021/9/10.
//

#include <tuple>

#include <Mirror/Factory.h>

namespace Explosion::Mirror {
    Explosion::Mirror::Variable GlobalFactory::GetVariable(const std::string& name)
    {
        auto iter = variables.find(name);
        if (iter == variables.end()) {
            throw NoSuchDefinitionException {};
        }
        return Variable(iter->second.get());
    }

    void GlobalFactory::ForEachVariable(const std::function<void(Explosion::Mirror::Variable)>& iter)
    {
        for (auto& v : variables) {
            iter(Variable(v.second.get()));
        }
    }

    Explosion::Mirror::Function GlobalFactory::GetFunction(const std::string& name)
    {
        auto iter = functions.find(name);
        if (iter == functions.end()) {
            throw NoSuchDefinitionException {};
        }
        return Function(iter->second.get());
    }

    void GlobalFactory::ForEachFunction(const std::function<void(Explosion::Mirror::Function)>& iter)
    {
        for (auto& v : functions) {
            iter(Function(v.second.get()));
        }
    }

    template<typename S>
    Variable StructFactory<S>::GetMemberVariable(const std::string& name)
    {
        auto iter = variables.find(name);
        if (iter == variables.end()) {
            throw NoSuchDefinitionException {};
        }
        return Variable(iter->second.get());
    }

    template<typename S>
    void StructFactory<S>::ForEachVariable(const std::function<void(Variable)>& iter)
    {
        for (auto& v : variables) {
            iter(Variable(v.second.get()));
        }
    }

    template<typename C>
    Function ClassFactory<C>::GetMemberFunction(const std::string& name)
    {
        auto iter = functions.find(name);
        if (iter == functions.end()) {
            throw NoSuchDefinitionException {};
        }
        return Function(iter->second.get());
    }

    template<typename C>
    void ClassFactory<C>::ForEachMemberFunction(const std::function<void(Function)>& iter)
    {
        for (auto& v : functions) {
            iter(Function(v.second.get()));
        }
    }
}
