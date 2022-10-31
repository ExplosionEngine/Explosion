//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <utility>

#include <Mirror/Type.h>
#include <Mirror/Registry.h>
#include <Common/Debug.h>

namespace Mirror {
    Type::Type(std::string inName) : name(std::move(inName)) {}

    Type::~Type() = default;

    const std::string& Type::GetName() const
    {
        return name;
    }

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

    void Variable::Set(Any* value) const
    {
        setter(value);
    }

    Any Variable::Get() const
    {
        return getter();
    }

    Function::Function(std::string inName, Function::Invoker inInvoker)
        : Type(std::move(inName))
        , invoker(std::move(inInvoker)) {}

    Function::~Function() = default;

    Any Function::InvokeWith(Any* args, size_t argsSize) const
    {
        return invoker(args, argsSize);
    }

    Constructor::Constructor(std::string inName, Constructor::Invoker inStackConstructor, Constructor::Invoker inHeapConstructor)
        : Type(std::move(inName))
        , stackConstructor(std::move(inStackConstructor))
        , heapConstructor(std::move(inHeapConstructor)) {}

    Constructor::~Constructor() = default;

    Any Constructor::ConstructOnStackWith(Any* arguments, size_t argumentsSize) const
    {
        return stackConstructor(arguments, argumentsSize);
    }

    Any Constructor::NewObjectWith(Any* arguments, size_t argumentsSize) const
    {
        return heapConstructor(arguments, argumentsSize);
    }

    Destructor::Destructor(Destructor::Invoker inDestructor)
        : Type(std::string(NamePresets::destructor)), destructor(std::move(inDestructor))
    {
    }

    Destructor::~Destructor() = default;

    void Destructor::InvokeWith(Any* object) const
    {
        destructor(object);
    }

    MemberVariable::MemberVariable(std::string inName, MemberVariable::Setter inSetter, MemberVariable::Getter inGetter)
        : Type(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter))
    {
    }

    MemberVariable::~MemberVariable() = default;

    void MemberVariable::Set(Any* object, Any* value) const
    {
        setter(object, value);
    }

    Any MemberVariable::Get(Any* object) const
    {
        return getter(object);
    }

    MemberFunction::MemberFunction(std::string inName, Invoker inInvoker)
        : Type(std::move(inName))
        , invoker(std::move(inInvoker))
    {
    }

    MemberFunction::~MemberFunction() = default;

    Any MemberFunction::InvokeWith(Any* object, Any* args, size_t argsSize) const
    {
        return invoker(object, args, argsSize);
    }

    GlobalScope::GlobalScope() : Type(std::string(NamePresets::globalScope)) {}

    GlobalScope::~GlobalScope() = default;

    const GlobalScope& GlobalScope::Get()
    {
        return Registry::Get().globalScope;
    }

    const Variable* GlobalScope::FindVariable(const std::string& name) const
    {
        auto iter = variables.find(name);
        return iter == variables.end() ? nullptr : &iter->second;
    }

    const Variable& GlobalScope::GetVariable(const std::string& name) const
    {
        auto iter = variables.find(name);
        Assert(iter != variables.end());
        return iter->second;
    }

    const Function* GlobalScope::FindFunction(const std::string& name) const
    {
        auto iter = functions.find(name);
        return iter == functions.end() ? nullptr : &iter->second;
    }

    const Function& GlobalScope::GetFunction(const std::string& name) const
    {
        auto iter = functions.find(name);
        Assert(iter != functions.end());
        return iter->second;
    }

    Class::Class(std::string name) : Type(std::move(name)) {}

    Class::~Class() = default;

    const Class* Class::Find(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        auto iter = classes.find(name);
        return iter == classes.end() ? nullptr : &iter->second;
    }

    const Class& Class::Get(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        auto iter = classes.find(name);
        Assert(iter != classes.end());
        return iter->second;
    }

    const Destructor& Class::GetDestructor() const
    {
        Assert(destructor.has_value());
        return destructor.value();
    }

    const Constructor* Class::FindConstructor(const std::string& name) const
    {
        auto iter = constructors.find(name);
        return iter == constructors.end() ? nullptr : &iter->second;
    }

    const Constructor& Class::GetConstructor(const std::string& name) const
    {
        auto iter = constructors.find(name);
        Assert(iter != constructors.end());
        return iter->second;
    }

    const Variable* Class::FindStaticVariable(const std::string& name) const
    {
        auto iter = staticVariables.find(name);
        return iter == staticVariables.end() ? nullptr : &iter->second;
    }

    const Variable& Class::GetStaticVariable(const std::string& name) const
    {
        auto iter = staticVariables.find(name);
        Assert(iter != staticVariables.end());
        return iter->second;
    }

    const Function* Class::FindStaticFunction(const std::string& name) const
    {
        auto iter = staticFunctions.find(name);
        return iter == staticFunctions.end() ? nullptr : &iter->second;
    }

    const Function& Class::GetStaticFunction(const std::string& name) const
    {
        auto iter = staticFunctions.find(name);
        Assert(iter != staticFunctions.end());
        return iter->second;
    }

    const MemberVariable* Class::FindMemberVariable(const std::string& name) const
    {
        auto iter = memberVariables.find(name);
        return iter == memberVariables.end() ? nullptr : &iter->second;
    }

    const MemberVariable& Class::GetMemberVariable(const std::string& name) const
    {
        auto iter = memberVariables.find(name);
        Assert(iter != memberVariables.end());
        return iter->second;
    }

    const MemberFunction* Class::FindMemberFunction(const std::string& name) const
    {
        auto iter = memberFunctions.find(name);
        return iter == memberFunctions.end() ? nullptr : &iter->second;
    }

    const MemberFunction& Class::GetMemberFunction(const std::string& name) const
    {
        auto iter = memberFunctions.find(name);
        Assert(iter != memberFunctions.end());
        return iter->second;
    }
}
