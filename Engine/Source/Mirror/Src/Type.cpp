//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <utility>
#include <sstream>

#include <Mirror/Type.h>
#include <Mirror/Registry.h>
#include <Common/Debug.h>
#include <Common/Format.h>

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

    std::string Type::GetAllMeta() const
    {
        std::stringstream stream;
        uint32_t count = 0;
        for (const auto& iter : metas) {
            stream << fmt::format("{}={}", iter.first, iter.second);

            count++;
            if (count != metas.size()) {
                stream << ";";
            }
        }
        return stream.str();
    }

    bool Type::HasMeta(const std::string& key) const
    {
        return metas.find(key) != metas.end();
    }

    Variable::Variable(std::string inName, Variable::Setter inSetter, Variable::Getter inGetter, VariableSerializer inSerializer, VariableDeserializer inDeserializer)
        : Type(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter))
        , serializer(std::move(inSerializer))
        , deserializer(std::move(inDeserializer))
    {
    }

    Variable::~Variable() = default;

    void Variable::Set(Any* value) const
    {
        setter(value);
    }

    Any Variable::Get() const
    {
        return getter();
    }

    void Variable::Serialize(SerializeStream& stream, const CustomVariableSerializer& customSerializer) const
    {
        if (customSerializer) {
            customSerializer(stream, *this, serializer);
        } else {
            serializer(stream, *this);
        }
    }

    void Variable::Deserialize(DeserializeStream& stream, const CustomVariableDeserializer& customDeserializer) const
    {
        if (customDeserializer) {
            customDeserializer(stream, *this, deserializer);
        } else {
            deserializer(stream, *this);
        }
    }

    Function::Function(std::string inName, Invoker inInvoker)
        : Type(std::move(inName))
        , invoker(std::move(inInvoker)) {}

    Function::~Function() = default;

    Any Function::InvokeWith(Any* args, size_t argsSize) const
    {
        return invoker(args, argsSize);
    }

    Constructor::Constructor(std::string inName, Invoker inStackConstructor, Invoker inHeapConstructor)
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

    Destructor::Destructor(Invoker inDestructor)
        : Type(std::string(NamePresets::destructor)), destructor(std::move(inDestructor))
    {
    }

    Destructor::~Destructor() = default;

    void Destructor::InvokeWith(Any* object) const
    {
        destructor(object);
    }

    MemberVariable::MemberVariable(std::string inName, Setter inSetter, Getter inGetter, MemberVariableSerializer inSerializer, MemberVariableDeserializer inDeserializer)
        : Type(std::move(inName))
        , setter(std::move(inSetter))
        , getter(std::move(inGetter))
        , serializer(std::move(inSerializer))
        , deserializer(std::move(inDeserializer))
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

    void MemberVariable::Serialize(SerializeStream& stream, Any* object, const CustomMemberVariableSerializer& customSerializer) const
    {
        if (customSerializer) {
            customSerializer(stream, *this, object, serializer);
        } else {
            serializer(stream, *this, object);
        }
    }

    void MemberVariable::Deserialize(DeserializeStream& stream, Any* object, const CustomMemberVariableDeserializer& customDeserializer) const
    {
        if (customDeserializer) {
            customDeserializer(stream, *this, object, deserializer);
        } else {
            deserializer(stream, *this, object);
        }
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

    void Class::Serialize(SerializeStream& stream, Mirror::Any* obj, const CustomMemberVariableSerializer& customSerializer) const
    {
        stream << GetName();
        stream << memberVariables.size();

        for (const auto& memberVariable : memberVariables) {
            stream << memberVariable.first;
            memberVariable.second.Serialize(stream, obj, customSerializer);
        }
    }

    void Class::Deserailize(DeserializeStream& stream, Mirror::Any* obj, const CustomMemberVariableDeserializer& customDeserializer) const
    {
        std::string className;
        stream >> className;

        size_t memberVariableSize;
        stream >> memberVariableSize;

        for (auto i = 0; i < memberVariableSize; i++) {
            std::string varName;
            stream >> varName;

            auto iter = memberVariables.find(varName);
            if (iter == memberVariables.end()) {
                continue;
            }
            iter->second.Deserialize(stream, obj, customDeserializer);
        }
    }
}
