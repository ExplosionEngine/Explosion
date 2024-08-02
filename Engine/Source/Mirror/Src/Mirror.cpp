//
// Created by johnk on 2022/9/21.
//

#include <utility>
#include <sstream>

#include <Mirror/Mirror.h>
#include <Mirror/Registry.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <Common/Hash.h>

namespace Mirror {
    TypeId GetTypeIdByName(const std::string& name)
    {
        return Common::HashUtils::CityHash(name.data(), name.size());
    }

    Any::Any()
        : typeInfo(nullptr)
        , rtti(nullptr)
    {
        ResizeMemory(0);
    }

    Any::~Any()
    {
        if (rtti != nullptr) {
            rtti->detor(GetMemory());
        }
    }

    Any::Any(const Any& inAny)
    {
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        ResizeMemory(inAny.memorySize);
        Assert(typeInfo->copyConstructible || typeInfo->copyAssignable);
        rtti->copy(GetMemory(), inAny.GetMemory());
    }

    Any::Any(Any&& inAny) noexcept // NOLINT
    {
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        ResizeMemory(inAny.memorySize);
        Assert(typeInfo->moveConstructible || typeInfo->moveAssignable);
        rtti->move(GetMemory(), inAny.GetMemory());
    }

    Any& Any::operator=(const Any& inAny)
    {
        if (&inAny == this) {
            return *this;
        }
        Reset();
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        ResizeMemory(inAny.memorySize);
        Assert(typeInfo->copyConstructible || typeInfo->copyAssignable);
        rtti->copy(GetMemory(), inAny.GetMemory());
        return *this;
    }

    Any& Any::operator=(Any&& inAny) noexcept
    {
        Reset();
        typeInfo = inAny.typeInfo;
        rtti = inAny.rtti;
        ResizeMemory(inAny.memorySize);
        Assert(typeInfo->moveConstructible || typeInfo->moveAssignable);
        rtti->move(GetMemory(), inAny.GetMemory());
        return *this;
    }

    bool Any::Convertible(const Mirror::TypeInfo* dstType) const
    {
        if (typeInfo->id == dstType->id) {
            return true;
        }

        const Class* srcClass;
        const Class* dstClass;
        if (typeInfo->isPointer && dstType->isPointer) {
            srcClass = Class::Find(typeInfo->removePointerType);
            dstClass = Class::Find(dstType->removePointerType);
        } else {
            srcClass = Class::Find(typeInfo->id);
            dstClass = Class::Find(dstType->id);
        }
        return srcClass != nullptr && dstClass != nullptr && srcClass->IsDerivedFrom(dstClass);
    }

    size_t Any::Size() const
    {
        return memorySize;
    }

    const void* Any::Data() const
    {
        return GetMemory();
    }

    const TypeInfo* Any::TypeInfo() const
    {
        return typeInfo;
    }

    void Any::Reset()
    {
        if (typeInfo != nullptr && rtti != nullptr) {
            rtti->detor(GetMemory());
        }
        typeInfo = nullptr;
        rtti = nullptr;
    }

    bool Any::operator==(const Any& rhs) const
    {
        return typeInfo == rhs.typeInfo
            && rtti->equal(GetMemory(), rhs.Data());
    }

    void Any::ResizeMemory(size_t size)
    {
        memorySize = size;
        if (size <= MaxStackMemorySize) {
            memory = StackMemory {};
        } else {
            memory = HeapMemory {};
            std::get<HeapMemory>(memory).resize(memorySize);
        }
    }

    void* Any::GetMemory() const
    {
        if (memory.index() == 0) {
            return const_cast<uint8_t*>(std::get<StackMemory>(memory).data());
        }
        if (memory.index() == 1) {
            return const_cast<uint8_t*>(std::get<HeapMemory>(memory).data());
        }
        return Assert(false), nullptr;
    }

    Type::Type(std::string inName) : name(std::move(inName)) {}

    Type::~Type() = default;

    const std::string& Type::GetName() const
    {
        return name;
    }

    const std::string& Type::GetMeta(const std::string& key) const
    {
        const auto iter = metas.find(key);
        Assert(iter != metas.end());
        return iter->second;
    }

    std::string Type::GetAllMeta() const
    {
        std::stringstream stream;
        uint32_t count = 0;
        for (const auto& [key, value] : metas) {
            stream << fmt::format("{}={}", key, value);

            count++;
            if (count != metas.size()) {
                stream << ";";
            }
        }
        return stream.str();
    }

    bool Type::HasMeta(const std::string& key) const
    {
        return metas.contains(key);
    }

    bool Type::GetMetaBool(const std::string& key) const
    {
        const auto& value = GetMeta(key);
        if (value == "true") {
            return true;
        }
        if (value == "false") {
            return false;
        }
        return Assert(false), false;
    }

    int32_t Type::GetMetaInt32(const std::string& key) const
    {
        return std::atoi(GetMeta(key).c_str());
    }

    int64_t Type::GetMetaInt64(const std::string& key) const
    {
        return std::atoll(GetMeta(key).c_str());
    }

    float Type::GetMetaFloat(const std::string& key) const
    {
        return std::atof(GetMeta(key).c_str());
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

    const TypeInfo* Function::GetArgTypeInfo(const uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Function::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

    Any Function::InvokeWith(Any* arguments, const uint8_t argumentsSize) const
    {
        return invoker(arguments, argumentsSize);
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

    const TypeInfo* Constructor::GetArgTypeInfo(const uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Constructor::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

    Any Constructor::ConstructOnStackWith(Any* arguments, const uint8_t argumentsSize) const
    {
        return stackConstructor(arguments, argumentsSize);
    }

    Any Constructor::NewObjectWith(Any* arguments, const uint8_t argumentsSize) const
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

    bool MemberVariable::IsTransient() const
    {
        return HasMeta("transient") && GetMetaBool("transient");
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

    const TypeInfo* MemberFunction::GetArgTypeInfo(const uint8_t argIndex) const
    {
        return argTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& MemberFunction::GetArgTypeInfos() const
    {
        return argTypeInfos;
    }

    Any MemberFunction::InvokeWith(Any* object, Any* args, const size_t argsSize) const
    {
        return invoker(object, args, argsSize);
    }

    GlobalScope::GlobalScope() : Type(std::string(NamePresets::globalScope)) {}

    GlobalScope::~GlobalScope() = default;

    Variable& GlobalScope::EmplaceVariable(const std::string& inName, Variable::ConstructParams&& inParams)
    {
        Assert(!variables.contains(inName));
        variables.emplace(inName, Variable(std::move(inParams)));
        return variables.at(inName);
    }

    Function& GlobalScope::EmplaceFunction(const std::string& inName, Function::ConstructParams&& inParams)
    {
        Assert(!functions.contains(inName));
        functions.emplace(inName, Function(std::move(inParams)));
        return functions.at(inName);
    }

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
        const auto iter = variables.find(name);
        return iter == variables.end() ? nullptr : &iter->second;
    }

    const Variable& GlobalScope::GetVariable(const std::string& name) const
    {
        const auto iter = variables.find(name);
        Assert(iter != variables.end());
        return iter->second;
    }

    bool GlobalScope::HasFunction(const std::string& name) const
    {
        return functions.contains(name);
    }

    const Function* GlobalScope::FindFunction(const std::string& name) const
    {
        const auto iter = functions.find(name);
        return iter == functions.end() ? nullptr : &iter->second;
    }

    const Function& GlobalScope::GetFunction(const std::string& name) const
    {
        const auto iter = functions.find(name);
        Assert(iter != functions.end());
        return iter->second;
    }

    std::unordered_map<TypeId, std::string> Class::typeToNameMap = {};

    Class::Class(ConstructParams&& params)
        : Type(std::move(params.name))
        , typeInfo(params.typeInfo)
        , baseClassGetter(std::move(params.baseClassGetter))
    {
        if (params.defaultObjectCreator.has_value()) {
            CreateDefaultObject(params.defaultObjectCreator.value());
        }
        if (params.destructorParams.has_value()) {
            destructor = Destructor(std::move(params.destructorParams.value()));
        }
        if (params.defaultConstructorParams.has_value()) {
            EmplaceConstructor(NamePresets::defaultConstructor, std::move(params.defaultConstructorParams.value()));
        }
    }

    Class::~Class() = default;

    void Class::CreateDefaultObject(const std::function<Any()>& inCreator)
    {
        defaultObject = inCreator();
    }

    Destructor& Class::EmplaceDestructor(Destructor::ConstructParams&& inParams)
    {
        Assert(!destructor.has_value());
        destructor = Destructor(std::move(inParams));
        return destructor.value();
    }

    Constructor& Class::EmplaceConstructor(const std::string& inName, Constructor::ConstructParams&& inParams)
    {
        Assert(!constructors.contains(inName));
        constructors.emplace(inName, Constructor(std::move(inParams)));
        return constructors.at(inName);
    }

    Variable& Class::EmplaceStaticVariable(const std::string& inName, Variable::ConstructParams&& inParams)
    {
        Assert(!staticVariables.contains(inName));
        staticVariables.emplace(inName, Variable(std::move(inParams)));
        return staticVariables.at(inName);
    }

    Function& Class::EmplaceStaticFunction(const std::string& inName, Function::ConstructParams&& inParams)
    {
        Assert(!staticFunctions.contains(inName));
        staticFunctions.emplace(inName, Function(std::move(inParams)));
        return staticFunctions.at(inName);
    }

    MemberVariable& Class::EmplaceMemberVariable(const std::string& inName, MemberVariable::ConstructParams&& inParams)
    {
        Assert(!memberVariables.contains(inName));
        memberVariables.emplace(inName, MemberVariable(std::move(inParams)));
        return memberVariables.at(inName);
    }

    MemberFunction& Class::EmplaceMemberFunction(const std::string& inName, MemberFunction::ConstructParams&& inParams)
    {
        Assert(!memberFunctions.contains(inName));
        memberFunctions.emplace(inName, MemberFunction(std::move(inParams)));
        return memberFunctions.at(inName);
    }

    template <typename F>
    void Class::ForEachClass(F&& func)
    {
        const auto& classes = Registry::Get().classes;
        for (const auto& [name, clazz] : func) {
            func(clazz);
        }
    }

    bool Class::Has(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        return classes.contains(name);
    }

    const Class* Class::Find(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        const auto iter = classes.find(name);
        return iter == classes.end() ? nullptr : &iter->second;
    }

    const Class& Class::Get(const std::string& name)
    {
        const auto& classes = Registry::Get().classes;
        const auto iter = classes.find(name);
        Assert(iter != classes.end());
        return iter->second;
    }

    bool Class::Has(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        return typeToNameMap.contains(typeInfo->id); // NOLINT
    }

    const Class* Class::Find(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        return Find(typeInfo->id); // NOLINT
    }

    const Class& Class::Get(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        return Get(typeInfo->id); // NOLINT
    }

    const Class* Class::Find(const TypeId typeId)
    {
        const auto iter = typeToNameMap.find(typeId);
        if (iter == typeToNameMap.end()) {
            return nullptr;
        }
        return Find(iter->second);
    }

    const Class& Class::Get(TypeId typeId)
    {
        const auto iter = typeToNameMap.find(typeId);
        AssertWithReason(iter != typeToNameMap.end(), "did you forget add EClass() annotation to class ?");
        return Get(iter->second);
    }

    std::vector<const Class*> Class::GetAll()
    {
        const auto& classes = Registry::Get().classes;
        std::vector<const Class*> result;
        result.reserve(classes.size());
        for (const auto& [name, clazz] : classes) {
            result.emplace_back(&clazz);
        }
        return result;
    }

    std::vector<const Class*> Class::FindWithCategory(const std::string& category)
    {
        const auto& classes = Registry::Get().classes;
        std::vector<const Class*> result;
        result.reserve(classes.size());
        for (const auto& [name, clazz] : classes) {
            if (clazz.HasMeta("category") && clazz.GetMeta("category") == category) {
                result.emplace_back(&clazz);
            }
        }
        return result;
    }

    const TypeInfo* Class::GetTypeInfo() const
    {
        return typeInfo;
    }

    bool Class::HasDefaultConstructor() const
    {
        return HasConstructor(NamePresets::defaultConstructor);
    }

    const Class* Class::GetBaseClass() const
    {
        return baseClassGetter();
    }

    bool Class::IsBaseOf(const Class* derivedClass) const
    {
        return derivedClass->IsDerivedFrom(this);
    }

    bool Class::IsDerivedFrom(const Class* baseClass) const
    {
        const Class* tBase = GetBaseClass();
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

    const Constructor* Class::FindSuitableConstructor(const Any* args, const uint8_t argNum) const
    {
        for (const auto& [constructorName, constructor] : constructors) {
            const auto& argTypeInfos = constructor.GetArgTypeInfos();
            if (argTypeInfos.size() != argNum) {
                continue;
            }

            bool bSuitable = true;
            for (auto i = 0; i < argNum; i++) {
                if (args[i].Convertible(argTypeInfos[i])) {
                    continue;
                }

                bSuitable = false;
                break;
            }

            if (bSuitable) {
                return &constructor;
            }
        }
        return nullptr;
    }

    Any Class::ConstructOnStackSuitable(Any* args, uint8_t argNum) const
    {
        const auto* constructor = FindSuitableConstructor(args, argNum);
        Assert(constructor != nullptr);
        return constructor->ConstructOnStackWith(args, argNum);
    }

    Any Class::NewObjectSuitable(Any* args, uint8_t argNum) const
    {
        const auto* constructor = FindSuitableConstructor(args, argNum);
        Assert(constructor != nullptr);
        return constructor->NewObjectWith(args, argNum);
    }

    const Constructor* Class::FindConstructor(const std::string& name) const
    {
        const auto iter = constructors.find(name);
        return iter == constructors.end() ? nullptr : &iter->second;
    }

    const Constructor& Class::GetConstructor(const std::string& name) const
    {
        const auto iter = constructors.find(name);
        Assert(iter != constructors.end());
        return iter->second;
    }

    bool Class::HasStaticVariable(const std::string& name) const
    {
        return staticVariables.contains(name);
    }

    const Variable* Class::FindStaticVariable(const std::string& name) const
    {
        const auto iter = staticVariables.find(name);
        return iter == staticVariables.end() ? nullptr : &iter->second;
    }

    const Variable& Class::GetStaticVariable(const std::string& name) const
    {
        const auto iter = staticVariables.find(name);
        Assert(iter != staticVariables.end());
        return iter->second;
    }

    bool Class::HasStaticFunction(const std::string& name) const
    {
        return staticFunctions.contains(name);
    }

    const Function* Class::FindStaticFunction(const std::string& name) const
    {
        const auto iter = staticFunctions.find(name);
        return iter == staticFunctions.end() ? nullptr : &iter->second;
    }

    const Function& Class::GetStaticFunction(const std::string& name) const
    {
        const auto iter = staticFunctions.find(name);
        Assert(iter != staticFunctions.end());
        return iter->second;
    }

    bool Class::HasMemberVariable(const std::string& name) const
    {
        return memberVariables.contains(name);
    }

    const MemberVariable* Class::FindMemberVariable(const std::string& name) const
    {
        const auto iter = memberVariables.find(name);
        return iter == memberVariables.end() ? nullptr : &iter->second;
    }

    const MemberVariable& Class::GetMemberVariable(const std::string& name) const
    {
        const auto iter = memberVariables.find(name);
        Assert(iter != memberVariables.end());
        return iter->second;
    }

    bool Class::HasMemberFunction(const std::string& name) const
    {
        return memberFunctions.contains(name);
    }

    const MemberFunction* Class::FindMemberFunction(const std::string& name) const
    {
        const auto iter = memberFunctions.find(name);
        return iter == memberFunctions.end() ? nullptr : &iter->second;
    }

    const MemberFunction& Class::GetMemberFunction(const std::string& name) const
    {
        const auto iter = memberFunctions.find(name);
        Assert(iter != memberFunctions.end());
        return iter->second;
    }

    void Class::Serialize(Common::SerializeStream& stream, Any* obj) const // NOLINT
    {
        if (const auto* baseClass = GetBaseClass();
            baseClass != nullptr) {
            baseClass->Serialize(stream, obj);
        }

        AssertWithReason(defaultObject.has_value(), "do you forget add default constructor to EClass() which you want to serialize");

        const auto& name = GetName();
        const auto memberVariablesNum = memberVariables.size();
        Common::Serializer<std::string>::Serialize(stream, name);
        Common::Serializer<uint64_t>::Serialize(stream, memberVariablesNum);

        for (const auto& [name, memberVariable] : memberVariables) {
            if (memberVariable.IsTransient()) {
                continue;
            }

            Common::Serializer<std::string>::Serialize(stream, name);

            const bool sameWithDefaultObject = memberVariable.Get(obj) == defaultObject.value();
            Common::Serializer<bool>::Serialize(stream, sameWithDefaultObject);

            if (sameWithDefaultObject) {
                Common::Serializer<uint32_t>::Serialize(stream, 0);
            } else {
                Common::Serializer<uint32_t>::Serialize(stream, memberVariable.SizeOf());
                memberVariable.Serialize(stream, obj);
            }
        }

        if (HasMemberFunction("OnSerialized")) {
            GetMemberFunction("OnSerialized").InvokeWith(obj, nullptr, 0);
        }
    }

    void Class::Deserailize(Common::DeserializeStream& stream, Any* obj) const // NOLINT
    {
        if (const auto* baseClass = GetBaseClass();
            baseClass != nullptr) {
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

            const auto& memberVariable = iter->second;
            if (memberVariable.IsTransient()) {
                continue;
            }

            bool restoreAsDefaultObject = true;
            Common::Serializer<bool>::Deserialize(stream, restoreAsDefaultObject);

            uint32_t memorySize = 0;
            Common::Serializer<uint32_t>::Deserialize(stream, memorySize);
            if (memorySize == 0 || memorySize != memberVariable.SizeOf()) {
                restoreAsDefaultObject = true;
            }

            if (!restoreAsDefaultObject) {
                memberVariable.Deserialize(stream, obj);
            }
        }

        if (HasMemberFunction("OnDeserialize")) {
            GetMemberFunction("OnDeserialize").InvokeWith(obj, nullptr, 0);
        }
    }

    EnumElement::EnumElement(ConstructParams&& inParams)
        : Type(std::move(inParams.name))
        , getter(std::move(inParams.getter))
        , comparer(std::move(inParams.comparer))
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
        const auto iter = enums.find(name);
        return iter == enums.end() ? nullptr : &iter->second;
    }

    const Enum& Enum::Get(const std::string& name)
    {
        const auto& enums = Registry::Get().enums;
        const auto iter = enums.find(name);
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
        const auto iter = elements.find(name);
        Assert(iter != elements.end());
        return iter->second.Get();
    }

    std::string Enum::GetElementName(Any* value) const
    {
        for (const auto& [name, element] : elements) {
            if (element.Compare(value)) {
                return name;
            }
        }
        QuickFail();
        return "";
    }

    EnumElement& Enum::EmplaceElement(const std::string& inName, EnumElement::ConstructParams&& inParams)
    {
        Assert(!elements.contains(inName));
        elements.emplace(inName, EnumElement(std::move(inParams)));
        return elements.at(inName);
    }
}
