//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <utility>
#include <sstream>

#include <Mirror/Type.h>
#include <Mirror/Registry.h>
#include <Common/Debug.h>
#include <Common/String.h>

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

    void Variable::Serialize(Common::SerializeStream& stream) const
    {
        serializer(stream, *this);
    }

    void Variable::Deserialize(Common::DeserializeStream& stream) const
    {
        deserializer(stream, *this);
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

    void MemberVariable::Serialize(Common::SerializeStream& stream, Any* object) const
    {
        serializer(stream, *this, object);
    }

    void MemberVariable::Deserialize(Common::DeserializeStream& stream, Any* object) const
    {
        deserializer(stream, *this, object);
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

    const Constructor& Class::GetDefaultConstructor() const
    {
        return GetConstructor(NamePresets::defaultConstructor);
    }

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
        return destructor.value(); // NOLINT
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

    void Class::Serialize(Common::SerializeStream& stream, Mirror::Any* obj) const
    {
        std::string name = GetName();
        uint64_t memberVariablesNum = memberVariables.size();
        Common::Serializer<std::string>::Serialize(stream, name);
        Common::Serializer<uint64_t>::Serialize(stream, memberVariablesNum);

        for (const auto& memberVariable : memberVariables) {
            Common::Serializer<std::string>::Serialize(stream, memberVariable.first);
            memberVariable.second.Serialize(stream, obj);
        }
    }

    void Class::Deserailize(Common::DeserializeStream& stream, Mirror::Any* obj) const
    {
        std::string className;
        Common::Serializer<std::string>::Deserialize(stream, className);

        uint64_t memberVariableSize;
        Common::Serializer<uint64_t>::Deserialize(stream, memberVariableSize);

        for (auto i = 0; i < memberVariableSize; i++) {
            std::string varName;
            Common::Serializer<std::string>::Deserialize(stream, varName);

            auto iter = memberVariables.find(varName);
            if (iter == memberVariables.end()) {
                continue;
            }
            iter->second.Deserialize(stream, obj);
        }
    }

    EnumElement::EnumElement(std::string inName, EnumElement::Getter inGetter, EnumElement::Comparer inComparer)
        : Type(std::move(inName))
        , getter(std::move(inGetter))
        , comparer(std::move(inComparer))
    {
    }

    EnumElement::~EnumElement() = default;

    Any EnumElement::Get() const
    {
        return getter();
    }

    bool EnumElement::Compare(Any* value) const
    {
        return comparer(value);
    }

    const Enum* Enum::Find(const std::string& name)
    {
        const auto& enums = Registry::Get().enums;
        auto iter = enums.find(name);
        return iter == enums.end() ? nullptr : &iter->second;
    }

    const Enum& Enum::Get(const std::string& name)
    {
        const auto& enums = Registry::Get().enums;
        auto iter = enums.find(name);
        Assert(iter != enums.end());
        return iter->second;
    }

    Enum::Enum(std::string name)
        : Type(std::move(name))
    {
    }

    Enum::~Enum() = default;

    Any Enum::GetElement(const std::string& name) const
    {
        auto iter = elements.find(name);
        Assert(iter != elements.end());
        return iter->second.Get();
    }

    std::string Enum::GetElementName(Any* value) const
    {
        for (const auto& element : elements) {
            if (element.second.Compare(value)) {
                return element.first;
            }
        }
        Assert(false);
        return "";
    }
}
