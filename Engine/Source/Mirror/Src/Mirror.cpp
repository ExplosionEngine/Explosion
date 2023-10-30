//
// Created by johnk on 2022/9/21.
//

#include <utility>
#include <sstream>

#include <Mirror/Mirror.h>
#include <Mirror/Registry.h>
#include <Common/Debug.h>
#include <Common/String.h>

namespace Mirror::Internal {
    TypeId ComputeTypeId(std::string_view sigName)
    {
        return Common::HashUtils::CityHash(sigName.data(), sigName.size());
    }
}

namespace Mirror {
    Any::~Any()
    {
        if (rtti != nullptr) {
            rtti->detor(data.data());
        }
    }

    Any::Any(const Any& inAny)
    {
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        data.resize(inAny.data.size());
        rtti->copy(data.data(), inAny.data.data());
    }

    Any::Any(Any&& inAny) noexcept
    {
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        data.resize(inAny.data.size());
        rtti->move(data.data(), inAny.data.data());
    }

    Any& Any::operator=(const Any& inAny)
    {
        if (&inAny == this) {
            return *this;
        }
        Reset();
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        rtti->copy(data.data(), inAny.data.data());
        return *this;
    }

    Any& Any::operator=(Mirror::Any&& inAny) noexcept
    {
        Reset();
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        rtti->move(data.data(), inAny.data.data());
        return *this;
    }

    size_t Any::Size() const
    {
        return data.size();
    }

    const void* Any::Data() const
    {
        return data.data();
    }

    const Mirror::TypeInfo* Any::TypeInfo()
    {
        return typeInfo;
    }

    const Mirror::TypeInfo* Any::TypeInfo() const
    {
        return typeInfo->addConst();
    }

    void Any::Reset()
    {
        if (rtti != nullptr) {
            rtti->detor(data.data());
        }
        typeInfo = nullptr;
        rtti = nullptr;
    }

    bool Any::operator==(const Any& rhs) const
    {
        return typeInfo == rhs.typeInfo
            && rtti->equal(data.data(), rhs.Data());
    }

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

    Variable::Variable(ConstructParams&& params)
        : Type(std::move(params.name))
        , memorySize(params.memorySize)
        , typeInfo(params.typeInfo)
        , setter(std::move(params.setter))
        , getter(std::move(params.getter))
        , serializer(std::move(params.serializer))
        , deserializer(std::move(params.deserializer))
    {
    }

    Variable::~Variable() = default;

    const TypeInfo* Variable::GetTypeInfo() const
    {
        return typeInfo;
    }

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

    Function::Function(ConstructParams&& params)
        : Type(std::move(params.name))
        , argsNum(params.argsNum)
        , retTypeInfo(params.retTypeInfo)
        , argTypeInfos(std::move(params.argTypeInfos))
        , invoker(std::move(params.invoker))
    {
    }

    Function::~Function() = default;

    uint8_t Function::GetArgsNum() const
    {
        return argsNum;
    }

    const TypeInfo* Function::GetRetTypeInfo() const
    {
        return retTypeInfo;
    }

    const TypeInfo* Function::GetArgTypeInfo(uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Function::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

    Any Function::InvokeWith(Any* args, uint8_t argsSize) const
    {
        return invoker(args, argsSize);
    }

    Constructor::Constructor(ConstructParams&& params)
        : Type(std::move(params.name))
        , argsNum(params.argsNum)
        , argTypeInfos(std::move(params.argTypeInfos))
        , stackConstructor(std::move(params.stackConstructor))
        , heapConstructor(std::move(params.heapConstructor))
    {
    }

    Constructor::~Constructor() = default;

    uint8_t Constructor::GetArgsNum() const
    {
        return argsNum;
    }

    const TypeInfo* Constructor::GetArgTypeInfo(uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Constructor::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

    Any Constructor::ConstructOnStackWith(Any* arguments, uint8_t argumentsSize) const
    {
        return stackConstructor(arguments, argumentsSize);
    }

    Any Constructor::NewObjectWith(Any* arguments, uint8_t argumentsSize) const
    {
        return heapConstructor(arguments, argumentsSize);
    }

    Destructor::Destructor(ConstructParams&& params)
        : Type(std::string(NamePresets::destructor))
        , destructor(std::move(params.destructor))
    {
    }

    Destructor::~Destructor() = default;

    void Destructor::InvokeWith(Any* object) const
    {
        destructor(object);
    }

    MemberVariable::MemberVariable(ConstructParams&& params)
        : Type(std::move(params.name))
        , memorySize(params.memorySize)
        , typeInfo(params.typeInfo)
        , setter(std::move(params.setter))
        , getter(std::move(params.getter))
        , serializer(std::move(params.serializer))
        , deserializer(std::move(params.deserializer))
    {
    }

    MemberVariable::~MemberVariable() = default;

    uint32_t MemberVariable::SizeOf() const
    {
        return memorySize;
    }

    const TypeInfo* MemberVariable::GetTypeInfo() const
    {
        return typeInfo;
    }

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

    MemberFunction::MemberFunction(ConstructParams&& params)
        : Type(std::move(params.name))
        , argsNum(params.argsNum)
        , retTypeInfo(params.retTypeInfo)
        , argTypeInfos(std::move(params.argTypeInfos))
        , invoker(std::move(params.invoker))
    {
    }

    MemberFunction::~MemberFunction() = default;

    uint8_t MemberFunction::GetArgsNum() const
    {
        return argsNum;
    }

    const TypeInfo* MemberFunction::GetRetTypeInfo() const
    {
        return retTypeInfo;
    }

    const TypeInfo* MemberFunction::GetArgTypeInfo(uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& MemberFunction::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

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

    bool GlobalScope::HasVariable(const std::string& name) const
    {
        return variables.contains(name);
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

    bool GlobalScope::HasFunction(const std::string& name) const
    {
        return functions.contains(name);
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

    std::unordered_map<TypeId, std::string> Class::typeToNameMap = {};

    Class::Class(ConstructParams&& params)
        : Type(std::move(params.name))
        , typeInfo(params.typeInfo)
        , baseClassGetter(std::move(params.baseClassGetter))
        , defaultObject(std::move(params.defaultObject))
        , destructor(std::move(params.destructor))
        , constructors()
    {
        if (params.defaultConstructor.has_value()) {
            constructors.emplace(std::make_pair(NamePresets::defaultConstructor, std::move(params.defaultConstructor.value())));
        }
    }

    Class::~Class() = default;

    bool Class::Has(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        return classes.contains(name);
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

    bool Class::Has(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        return typeToNameMap.contains(typeInfo->id);
    }

    const Class* Class::Find(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        auto iter = typeToNameMap.find(typeInfo->id);
        if (iter == typeToNameMap.end()) {
            return nullptr;
        }
        return Find(iter->second);
    }

    const Class& Class::Get(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        auto iter = typeToNameMap.find(typeInfo->id);
        AssertWithReason(iter != typeToNameMap.end(), "did you forget add EClass() annotation to class ?");
        return Get(iter->second);
    }

    const TypeInfo* Class::GetTypeInfo() const
    {
        return typeInfo;
    }

    bool Class::HasDefaultConstructor() const
    {
        return HasConstructor(NamePresets::defaultConstructor);
    }

    const Mirror::Class* Class::GetBaseClass() const
    {
        return baseClassGetter();
    }

    bool Class::IsBaseOf(const Mirror::Class* derivedClass) const
    {
        return derivedClass->IsDerivedFrom(this);
    }

    bool Class::IsDerivedFrom(const Mirror::Class* baseClass) const
    {
        const Mirror::Class* tBase = GetBaseClass();
        while (tBase != nullptr) {
            if (tBase == baseClass) {
                return true;
            }
            tBase = tBase->GetBaseClass();
        }
        return false;
    }

    const Constructor* Class::FindDefaultConstructor() const
    {
        return FindConstructor(NamePresets::defaultConstructor);
    }

    const Constructor& Class::GetDefaultConstructor() const
    {
        return GetConstructor(NamePresets::defaultConstructor);
    }

    bool Class::HasDestructor() const
    {
        return destructor.has_value();
    }

    const Destructor* Class::FindDestructor() const
    {
        return destructor.has_value() ? &destructor.value() : nullptr;
    }

    const Destructor& Class::GetDestructor() const
    {
        Assert(destructor.has_value());
        return destructor.value(); // NOLINT
    }

    bool Class::HasConstructor(const std::string& name) const
    {
        return constructors.contains(name);
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

    bool Class::HasStaticVariable(const std::string& name) const
    {
        return staticVariables.contains(name);
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

    bool Class::HasStaticFunction(const std::string& name) const
    {
        return staticFunctions.contains(name);
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

    bool Class::HasMemberVariable(const std::string& name) const
    {
        return memberVariables.contains(name);
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

    bool Class::HasMemberFunction(const std::string& name) const
    {
        return memberFunctions.contains(name);
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
        const auto* baseClass = GetBaseClass();
        if (baseClass != nullptr) {
            baseClass->Serialize(stream, obj);
        }

        Assert(defaultObject.has_value());

        std::string name = GetName();
        uint64_t memberVariablesNum = memberVariables.size();
        Common::Serializer<std::string>::Serialize(stream, name);
        Common::Serializer<uint64_t>::Serialize(stream, memberVariablesNum);

        for (const auto& memberVariable : memberVariables) {
            Common::Serializer<std::string>::Serialize(stream, memberVariable.first);

            const bool sameWithDefaultObject = memberVariable.second.Get(obj) == defaultObject.value();
            Common::Serializer<bool>::Serialize(stream, sameWithDefaultObject);

            if (sameWithDefaultObject) {
                Common::Serializer<uint32_t>::Serialize(stream, 0);
            } else {
                Common::Serializer<uint32_t>::Serialize(stream, memberVariable.second.SizeOf());
                memberVariable.second.Serialize(stream, obj);
            }
        }
    }

    void Class::Deserailize(Common::DeserializeStream& stream, Mirror::Any* obj) const
    {
        const auto* baseClass = GetBaseClass();
        if (baseClass != nullptr) {
            baseClass->Deserailize(stream, obj);
        }

        Assert(defaultObject.has_value());

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

            bool restoreAsDefaultObject = true;
            Common::Serializer<bool>::Deserialize(stream, restoreAsDefaultObject);

            uint32_t memorySize = 0;
            Common::Serializer<uint32_t>::Deserialize(stream, memorySize);
            if (memorySize == 0 || memorySize != iter->second.SizeOf()) {
                restoreAsDefaultObject = true;
            }

            if (!restoreAsDefaultObject) {
                iter->second.Deserialize(stream, obj);
            }
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

    std::unordered_map<TypeId, std::string> Enum::typeToNameMap = {};

    Enum::Enum(ConstructParams&& params)
        : Type(std::move(params.name))
        , typeInfo(params.typeInfo)
    {
    }

    Enum::~Enum() = default;

    const TypeInfo* Enum::GetTypeInfo() const
    {
        return typeInfo;
    }

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
