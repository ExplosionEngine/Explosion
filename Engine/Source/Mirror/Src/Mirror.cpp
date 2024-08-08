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
    bool PointerConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType)
    {
        const auto [srcRaw, srcRemoveRef, srcRemovePointer] = inSrcType;
        const auto [dstRaw, dstRemoveRef, dstRemovePointer] = inDstType;

        if (!srcRaw->isPointer || !dstRaw->isPointer) {
            return false;
        }
        if (srcRemovePointer->id != dstRemovePointer->id) {
            return false;
        }
        return !srcRemovePointer->isConst || dstRemovePointer->isConst;
    }

    bool PolymorphismConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType)
    {
        const auto [srcRaw, srcRemoveRef, srcRemovePointer] = inSrcType;
        const auto [dstRaw, dstRemoveRef, dstRemovePointer] = inDstType;

        const bool checkPointer = srcRaw->isPointer && dstRaw->isPointer;
        const bool checkRef = dstRaw->isLValueReference;

        if (!checkPointer && !checkRef) {
            return false;
        }

        const Mirror::TypeInfo* srcRemoveRefOrPtr = nullptr;
        const Mirror::TypeInfo* dstRemoveRefOrPtr = nullptr;

        if (checkRef) {
            srcRemoveRefOrPtr = srcRemoveRef;
            dstRemoveRefOrPtr = dstRemoveRef;
        } else {
            srcRemoveRefOrPtr = srcRemovePointer;
            dstRemoveRefOrPtr = dstRemovePointer;
        }

        const auto* srcClass = Class::Find(srcRemoveRefOrPtr->id); // NOLINT
        const auto* dstClass = Class::Find(dstRemoveRefOrPtr->id); // NOLINT

        if (srcClass == nullptr || dstClass == nullptr || !dstClass->IsBaseOf(srcClass)) {
            return false;
        }
        return !srcRemoveRefOrPtr->isConst || dstRemoveRefOrPtr->isConst;
    }

    bool Convertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType)
    {
        const auto [srcRaw, srcRemoveRef, srcRemovePointer] = inSrcType;
        const auto [dstRaw, dstRemoveRef, dstRemovePointer] = inDstType;

        if (dstRaw->isRValueReference) {
            return false;
        }

        if (srcRaw->id == dstRaw->id) { // NOLINT
            if (!dstRaw->isLValueReference) {
                return true;
            }
            return !srcRemoveRef->isConst || dstRemoveRef->isConst;
        }
        return PointerConvertible(inSrcType, inDstType) || PolymorphismConvertible(inSrcType, inDstType);
    }

    Any::Any()
    {
        Reset();
    }

    Any::~Any()
    {
        if (IsMemoryHolder() && rtti != nullptr) {
            rtti->detor(Data());
        }
    }

    Any::Any(Any& inOther)
    {
        PerformCopyConstruct(inOther);
    }

    Any::Any(const Any& inOther)
    {
        PerformCopyConstruct(inOther);
    }

    Any::Any(const Any& inOther, const AnyPolicy inPolicy)
    {
        PerformCopyConstructWithPolicy(inOther, inPolicy);
    }

    Any::Any(Any&& inOther) noexcept
    {
        PerformMoveConstruct(std::move(inOther));
    }

    Any& Any::operator=(const Any& inOther)
    {
        if (&inOther == this) {
            return *this;
        }

        Reset();
        PerformCopyConstruct(inOther);
        return *this;
    }

    Any& Any::operator=(Any&& inOther) noexcept
    {
        Reset();
        PerformMoveConstruct(std::move(inOther));
        return *this;
    }

    void Any::PerformCopyConstruct(const Any& inOther)
    {
        policy = inOther.policy;
        rtti = inOther.rtti;
        info = inOther.info;

        if (IsMemoryHolder()) {
            rtti->copyConstruct(Data(), inOther.Data());
        }
    }

    void Any::PerformCopyConstructWithPolicy(const Any& inOther, const AnyPolicy inPolicy)
    {
        policy = inPolicy;
        rtti = inOther.rtti;

        if (IsRef()) {
            info = RefInfo(inOther.Data(), inOther.Size());
        } else {
            info = HolderInfo(inOther.Size());
            rtti->copyConstruct(Data(), inOther.Data());
        }
    }

    void Any::PerformMoveConstruct(Any&& inOther)
    {
        policy = inOther.policy;
        rtti = inOther.rtti;

        if (IsRef()) {
            info = RefInfo(inOther.Data(), inOther.Size());
        } else {
            info = HolderInfo(inOther.Size());
            rtti->moveConstruct(Data(), inOther.Data());
        }
    }

    Any Any::Ref()
    {
        return { *this, IsMemoryHolder() ? AnyPolicy::ref : policy };
    }

    Any Any::Ref() const
    {
        return { *this, IsMemoryHolder() ? AnyPolicy::constRef : policy };
    }

    Any Any::ConstRef() const
    {
        return { *this, AnyPolicy::constRef };
    }

    Any Any::AsValue() const
    {
        return { *this, AnyPolicy::memoryHolder };
    }

    Any Any::Ptr()
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getPtr(Data());
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getPtr(Data());
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstPtr(Data());
        }
        QuickFail();
        return {};
    }

    Any Any::Ptr() const
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getConstPtr(Data());
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getPtr(Data());
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstPtr(Data());
        }
        QuickFail();
        return {};
    }

    Any Any::ConstPtr() const
    {
        return rtti->getConstPtr(Data());
    }

    Any Any::Deref() const
    {
        return rtti->deref(Data());
    }

    AnyPolicy Any::Policy() const
    {
        return policy;
    }

    bool Any::IsMemoryHolder() const
    {
        return policy == AnyPolicy::memoryHolder;
    }

    bool Any::IsRef() const
    {
        return policy == AnyPolicy::ref || policy == AnyPolicy::constRef;
    }

    bool Any::IsConstRef() const
    {
        return policy == AnyPolicy::constRef;
    }

    const TypeInfo* Any::Type()
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getValueType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getRefType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstRefType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::Type() const
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getConstValueType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getRefType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstRefType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::RemoveRefType()
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getValueType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getValueType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstValueType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::RemoveRefType() const
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getConstValueType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getValueType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getConstValueType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::AddPointerType()
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getAddPointerType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getAddPointerType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getAddConstPointerType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::AddPointerType() const
    {
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getAddConstPointerType();
        }
        if (policy == AnyPolicy::ref) {
            return rtti->getAddPointerType();
        }
        if (policy == AnyPolicy::constRef) {
            return rtti->getAddConstPointerType();
        }
        QuickFail();
        return nullptr;
    }

    const TypeInfo* Any::RemovePointerType()
    {
        if (const auto* typeInfo = Type();
            !typeInfo->isPointer) { // NOLINT
            return typeInfo;
        }
        return rtti->getRemovePointerType();
    }

    const TypeInfo* Any::RemovePointerType() const
    {
        if (const auto* typeInfo = Type();
            !typeInfo->isPointer) { // NOLINT
            return typeInfo;
        }
        return rtti->getRemovePointerType();
    }

    Mirror::TypeId Any::TypeId()
    {
        return Type()->id;
    }

    TypeId Any::TypeId() const
    {
        return Type()->id;
    }

    void Any::Reset()
    {
        policy = AnyPolicy::max;
        rtti = nullptr;
        info = {};
    }

    bool Any::Empty() const
    {
        return rtti == nullptr;
    }

    void* Any::Data() const
    {
        return IsRef() ? std::get<RefInfo>(info).Ptr() : std::get<HolderInfo>(info).Ptr();
    }

    size_t Any::Size() const
    {
        return IsRef() ? std::get<RefInfo>(info).Size() : std::get<HolderInfo>(info).Size();
    }

    Any::operator bool() const
    {
        return !Empty();
    }

    bool Any::operator==(const Any& inAny) const
    {
        if (TypeId() != inAny.TypeId()) {
            return false;
        }
        return rtti->equal(Data(), inAny.Data());
    }

    Any::HolderInfo::HolderInfo() = default;

    Any::HolderInfo::HolderInfo(size_t inMemorySize)
    {
        ResizeMemory(inMemorySize);
    }

    void Any::HolderInfo::ResizeMemory(size_t inSize)
    {
        if (inSize <= MaxStackMemorySize) {
            memory = StackMemory {};
            std::get<StackMemory>(memory).Resize(inSize);
        } else {
            memory = HeapMemory {};
            std::get<HeapMemory>(memory).resize(inSize);
        }
    }

    void* Any::HolderInfo::Ptr() const
    {
        if (memory.index() == 0) {
            return const_cast<uint8_t*>(std::get<StackMemory>(memory).Data());
        }
        return const_cast<uint8_t*>(std::get<HeapMemory>(memory).data());
    }

    size_t Any::HolderInfo::Size() const
    {
        if (memory.index() == 0) {
            return std::get<StackMemory>(memory).Size();
        }
        return std::get<HeapMemory>(memory).size();
    }

    Any::RefInfo::RefInfo()
        : ptr(nullptr)
        , memorySize(0)
    {
    }

    Any::RefInfo::RefInfo(void* inPtr, size_t inSize)
        : ptr(inPtr)
        , memorySize(inSize)
    {
    }

    void* Any::RefInfo::Ptr() const
    {
        return ptr;
    }

    size_t Any::RefInfo::Size() const
    {
        return memorySize;
    }

    Argument::Argument() = default;

    Argument::Argument(Any& inAny)
        : any(&inAny)
    {
    }

    Argument::Argument(const Any& inAny)
        : any(&inAny)
    {
    }

    Argument::Argument(Any&& inAny)
        : any(std::move(inAny))
    {
    }

    Argument& Argument::operator=(Any& inAny)
    {
        any = &inAny;
        return *this;
    }

    Argument& Argument::operator=(const Any& inAny)
    {
        any = &inAny;
        return *this;
    }

    Argument& Argument::operator=(Any&& inAny)
    {
        any = std::move(inAny);
        return *this;
    }

    const TypeInfo* Argument::Type() const
    {
        const auto index = any.index();
        if (index == 1) {
            return std::get<Any*>(any)->Type();
        }
        if (index == 2) {
            return std::get<const Any*>(any)->Type();
        }
        if (index == 3) {
            return const_cast<Any&>(std::get<Any>(any)).Type();
        }
        QuickFailWithReason("Argument is empty");
        return std::get<Any*>(any)->Type();
    }

    const TypeInfo* Argument::RemoveRefType() const
    {
        const auto index = any.index();
        if (index == 1) {
            return std::get<Any*>(any)->RemoveRefType();
        }
        if (index == 2) {
            return std::get<const Any*>(any)->RemoveRefType();
        }
        if (index == 3) {
            return const_cast<Any&>(std::get<Any>(any)).RemoveRefType();
        }
        QuickFailWithReason("Argument is empty");
        return std::get<Any*>(any)->RemoveRefType();
    }

    const TypeInfo* Argument::AddPointerType() const
    {
        const auto index = any.index();
        if (index == 1) {
            return std::get<Any*>(any)->AddPointerType();
        }
        if (index == 2) {
            return std::get<const Any*>(any)->AddPointerType();
        }
        if (index == 3) {
            return const_cast<Any&>(std::get<Any>(any)).AddPointerType();
        }
        QuickFailWithReason("Argument is empty");
        return std::get<Any*>(any)->AddPointerType();
    }

    const TypeInfo* Argument::RemovePointerType() const
    {
        const auto index = any.index();
        if (index == 1) {
            return std::get<Any*>(any)->RemovePointerType();
        }
        if (index == 2) {
            return std::get<const Any*>(any)->RemovePointerType();
        }
        if (index == 3) {
            return const_cast<Any&>(std::get<Any>(any)).RemovePointerType();
        }
        QuickFailWithReason("Argument is empty");
        return std::get<Any*>(any)->RemovePointerType();
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

    Any Variable::Get() const
    {
        return GetDyn();
    }

    void Variable::Serialize(Common::SerializeStream& stream) const
    {
        SerializeDyn(stream);
    }

    void Variable::Deserialize(Common::DeserializeStream& stream) const
    {
        DeserializeDyn(stream);
    }

    const TypeInfo* Variable::GetTypeInfo() const
    {
        return typeInfo;
    }

    void Variable::SetDyn(const Argument& inArgument) const
    {
        setter(inArgument);
    }

    Any Variable::GetDyn() const
    {
        return getter();
    }

    void Variable::SerializeDyn(Common::SerializeStream& stream) const
    {
        serializer(stream, *this);
    }

    void Variable::DeserializeDyn(Common::DeserializeStream& stream) const
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

    Any Function::InvokeDyn(const ArgumentList& inArgumentList) const
    {
        return invoker(inArgumentList);
    }

    Constructor::Constructor(ConstructParams&& params)
        : Type(std::move(params.name))
        , argsNum(params.argsNum)
        , argTypeInfos(std::move(params.argTypeInfos))
        , argRemoveRefTypeInfos(std::move(params.argRemoveRefTypeInfos))
        , argRemovePointerTypeInfos(std::move(params.argRemovePointerTypeInfos))
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

    const TypeInfo* Constructor::GetArgRemoveRefTypeInfo(uint8_t argIndex) const
    {
        return argRemoveRefTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Constructor::GetArgRemoveRefTypeInfos() const
    {
        return argRemoveRefTypeInfos;
    }

    const TypeInfo* Constructor::GetArgRemovePointerTypeInfo(uint8_t argIndex) const
    {
        return argRemovePointerTypeInfos[argIndex];
    }

    const std::vector<const TypeInfo*>& Constructor::GetArgRemovePointerTypeInfos() const
    {
        return argRemovePointerTypeInfos;
    }

    Any Constructor::ConstructDyn(const ArgumentList& arguments) const
    {
        return stackConstructor(arguments);
    }

    Any Constructor::NewDyn(const ArgumentList& arguments) const
    {
        return heapConstructor(arguments);
    }

    Destructor::Destructor(ConstructParams&& params)
        : Type(std::string(NamePresets::detor))
        , destructor(std::move(params.destructor))
    {
    }

    Destructor::~Destructor() = default;

    void Destructor::InvokeDyn(const Argument& argument) const
    {
        destructor(argument);
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

    void MemberVariable::SetDyn(const Argument& object, const Argument& value) const
    {
        setter(object, value);
    }

    Any MemberVariable::GetDyn(const Argument& object) const
    {
        return getter(object);
    }

    void MemberVariable::SerializeDyn(Common::SerializeStream& stream, const Argument& object) const
    {
        serializer(stream, *this, object);
    }

    void MemberVariable::DeserializeDyn(Common::DeserializeStream& stream, const Argument& object) const
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

    Any MemberFunction::InvokeDyn(const Argument& object, const ArgumentList& arguments) const
    {
        return invoker(object, arguments);
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

    void GlobalScope::ForEachVariable(const VariableTraverser& func) const
    {
        for (const auto& [name, variable] : variables) {
            func(variable);
        }
    }

    void GlobalScope::ForEachFunction(const FunctionTraverser& func) const
    {
        for (const auto& [name, function] : functions) {
            func(function);
        }
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
            EmplaceConstructor(NamePresets::defaultCtor, std::move(params.defaultConstructorParams.value()));
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

    bool Class::Has(TypeId typeId)
    {
        const auto iter = typeToNameMap.find(typeId);
        if (iter == typeToNameMap.end()) {
            return false;
        }
        return Has(iter->second);
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

    Class::~Class() = default;

    void Class::ForEachStaticVariable(const VariableTraverser& func) const
    {
        for (const auto& [name, variable] : staticVariables) {
            func(variable);
        }
    }

    void Class::ForEachStaticFunction(const FunctionTraverser& func) const
    {
        for (const auto& [name, function] : staticFunctions) {
            func(function);
        }
    }

    void Class::ForEachMemberVariable(const MemberVariableTraverser& func) const
    {
        for (const auto& [name, memberVariable] : memberVariables) {
            func(memberVariable);
        }
    }

    void Class::ForEachMemberFunction(const MemberFunctionTraverser& func) const
    {
        for (const auto& [name, memberFunction] : memberFunctions) {
            func(memberFunction);
        }
    }

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

    const TypeInfo* Class::GetTypeInfo() const
    {
        return typeInfo;
    }

    bool Class::HasDefaultConstructor() const
    {
        return HasConstructor(NamePresets::defaultCtor);
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
        return FindConstructor(NamePresets::defaultCtor);
    }

    const Constructor& Class::GetDefaultConstructor() const
    {
        return GetConstructor(NamePresets::defaultCtor);
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

    const Constructor* Class::FindSuitableConstructor(const ArgumentList& arguments) const
    {
        for (const auto& [constructorName, constructor] : constructors) {
            const auto& argTypeInfos = constructor.GetArgTypeInfos();
            const auto& argRemoveRefTypeInfos = constructor.GetArgRemoveRefTypeInfos();
            const auto& argRemovePointerTypeInfos = constructor.GetArgRemovePointerTypeInfos();

            if (argTypeInfos.size() != arguments.size()) {
                continue;
            }

            bool bSuitable = true;
            for (auto i = 0; i < arguments.size(); i++) {
                const TypeInfoCompact srcType { arguments[i].Type(), arguments[i].RemoveRefType(), arguments[i].RemovePointerType() }; // NOLINT
                const TypeInfoCompact dstType { argTypeInfos[i], argRemoveRefTypeInfos[i], argRemovePointerTypeInfos[i] }; // NOLINT
                if (Convertible(srcType, dstType)) {
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

    Any Class::ConstructDyn(const ArgumentList& arguments) const
    {
        const auto* constructor = FindSuitableConstructor(arguments);
        Assert(constructor != nullptr);
        return constructor->ConstructDyn(arguments);
    }

    Any Class::NewDyn(const ArgumentList& arguments) const
    {
        const auto* constructor = FindSuitableConstructor(arguments);
        Assert(constructor != nullptr);
        return constructor->NewDyn(arguments);
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

    void Class::SerializeDyn(Common::SerializeStream& stream, const Argument& obj) const // NOLINT
    {
        if (const auto* baseClass = GetBaseClass();
            baseClass != nullptr) {
            baseClass->SerializeDyn(stream, obj);
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

            const bool sameWithDefaultObject = memberVariable.GetDyn(obj) == defaultObject.value();
            Common::Serializer<bool>::Serialize(stream, sameWithDefaultObject);

            if (sameWithDefaultObject) {
                Common::Serializer<uint32_t>::Serialize(stream, 0);
            } else {
                Common::Serializer<uint32_t>::Serialize(stream, memberVariable.SizeOf());
                memberVariable.SerializeDyn(stream, obj);
            }
        }

        if (HasMemberFunction("OnSerialized")) {
            (void) GetMemberFunction("OnSerialized").InvokeDyn(obj, {});
        }
    }

    void Class::DeserailizeDyn(Common::DeserializeStream& stream, const Argument& obj) const // NOLINT
    {
        if (const auto* baseClass = GetBaseClass();
            baseClass != nullptr) {
            baseClass->DeserailizeDyn(stream, obj);
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
                memberVariable.DeserializeDyn(stream, obj);
            }
        }

        if (HasMemberFunction("OnDeserialize")) {
            (void) GetMemberFunction("OnDeserialize").InvokeDyn(obj, {});
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

    bool EnumElement::Compare(const Argument& argument) const
    {
        return comparer(argument);
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

    std::string Enum::GetElementName(const Argument& argument) const
    {
        for (const auto& [name, element] : elements) {
            if (element.Compare(argument)) {
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
