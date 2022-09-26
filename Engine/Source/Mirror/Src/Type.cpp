//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <utility>

#include <Mirror/Type.h>
#include <Common/Debug.h>

namespace Mirror {
    Type::Type(std::string inName) : name(std::move(inName)) {}

    Type::~Type() = default;

    const std::string& Type::GetMeta(const std::string& key) const
    {
        auto iter = metas.find(key);
        Assert(iter != metas.end());
        return iter->second;
    }

    bool Type::HasMeta(const std::string& key) const
    {
        return metas.find(key) != metas.end();
    }

    Variable::Variable(std::string inName, Variable::Setter inSetter, Variable::Getter inGetter)
        : Type(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter)) {}

    Variable::~Variable() = default;

    void Variable::Set(Any* value)
    {
        setter(value);
    }

    Any Variable::Get()
    {
        return getter();
    }

    Function::Function(std::string inName, Function::Invoker inInvoker)
        : Type(std::move(inName))
        , invoker(std::move(inInvoker)) {}

    Function::~Function() = default;

    Any Function::Invoke(Any* args, size_t argsSize)
    {
        return invoker(args, argsSize);
    }

    MemberVariable::MemberVariable(std::string inName, MemberVariable::Setter inSetter, MemberVariable::Getter inGetter)
        : Type(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter))
    {
    }

    MemberVariable::~MemberVariable() = default;

    void MemberVariable::Set(Any* clazz, Any* value)
    {
        setter(clazz, value);
    }

    Any MemberVariable::Get(Any* clazz)
    {
        return getter(clazz);
    }

    MemberFunction::MemberFunction(std::string inName, Invoker inInvoker)
        : Type(std::move(inName))
        , invoker(std::move(inInvoker))
    {
    }

    MemberFunction::~MemberFunction() = default;

    Any MemberFunction::Invoke(Any* clazz, Any* args, size_t argsSize)
    {
        return invoker(clazz, args, argsSize);
    }
}
