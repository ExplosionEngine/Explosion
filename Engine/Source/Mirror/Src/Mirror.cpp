//
// Created by johnk on 2022/9/21.
//

#include <ranges>
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
            for (auto i = 0; i < ElementNum(); i++) {
                rtti->detor(Data(i));
            }
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

    Any::Any(const Any& inOther, AnyPolicy inPolicy, uint32_t inIndex)
    {
        PerformCopyConstructForElementWithPolicy(inOther, inPolicy, inIndex);
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
        arrayLength = inOther.arrayLength;
        policy = inOther.policy;
        rtti = inOther.rtti;
        info = inOther.info;

        if (Empty()) {
            Reset();
            return;
        }

        if (IsMemoryHolder()) {
            for (auto i = 0; i < ElementNum(); i++) {
                rtti->copyConstruct(Data(i), inOther.Data(i));
            }
        }
    }

    void Any::PerformCopyConstructWithPolicy(const Any& inOther, const AnyPolicy inPolicy)
    {
        arrayLength = inOther.arrayLength;
        policy = inPolicy;
        rtti = inOther.rtti;

        if (Empty()) {
            Reset();
            return;
        }

        if (IsRef()) {
            info = RefInfo(inOther.Data(), inOther.MemorySize());
        } else {
            info = HolderInfo(inOther.MemorySize());
            for (auto i = 0; i < ElementNum(); i++) {
                rtti->copyConstruct(Data(i), inOther.Data(i));
            }
        }
    }

    void Any::PerformCopyConstructForElementWithPolicy(const Any& inOther, AnyPolicy inPolicy, uint32_t inIndex)
    {
        arrayLength = 0;
        policy = inPolicy;
        rtti = inOther.rtti;

        if (Empty()) {
            Reset();
            return;
        }

        if (IsRef()) {
            info = RefInfo(inOther.Data(inIndex), inOther.ElementSize());
        } else {
            info = HolderInfo(inOther.ElementSize());
            rtti->copyConstruct(Data(), inOther.Data(inIndex));
        }
    }

    void Any::PerformMoveConstruct(Any&& inOther)
    {
        arrayLength = inOther.arrayLength;
        policy = inOther.policy;
        rtti = inOther.rtti;

        if (Empty()) {
            Reset();
            return;
        }

        if (IsRef()) {
            info = RefInfo(inOther.Data(), inOther.MemorySize());
        } else {
            info = HolderInfo(inOther.MemorySize());
            for (auto i = 0; i < ElementNum(); i++) {
                rtti->moveConstruct(Data(i), inOther.Data(i));
            }
        }
    }

    uint32_t Any::ElementNum() const
    {
        return std::max(1u, arrayLength);
    }

    bool Any::IsArray() const
    {
        return arrayLength > 0;
    }

    Any Any::At(uint32_t inIndex)
    {
        Assert(IsArray());
        return { *this, AnyPolicy::nonConstRef, inIndex };
    }

    Any Any::At(uint32_t inIndex) const
    {
        Assert(IsArray());
        return { *this, AnyPolicy::constRef, inIndex };
    }

    size_t Any::ElementSize() const
    {
        return MemorySize() / ElementNum();
    }

    uint32_t Any::ArrayLength() const
    {
        Assert(IsArray());
        return arrayLength;
    }

    Any Any::Ref()
    {
        Assert(!IsArray());
        return { *this, IsMemoryHolder() ? AnyPolicy::nonConstRef : policy };
    }

    Any Any::Ref() const
    {
        Assert(!IsArray());
        return { *this, IsMemoryHolder() ? AnyPolicy::constRef : policy };
    }

    Any Any::ConstRef() const
    {
        Assert(!IsArray());
        return { *this, AnyPolicy::constRef };
    }

    Any Any::Value() const
    {
        Assert(!IsArray());
        return { *this, AnyPolicy::memoryHolder };
    }

    Any Any::Ptr()
    {
        Assert(!IsArray());
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getPtr(Data());
        }
        if (policy == AnyPolicy::nonConstRef) {
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
        Assert(!IsArray());
        if (policy == AnyPolicy::memoryHolder) {
            return rtti->getConstPtr(Data());
        }
        if (policy == AnyPolicy::nonConstRef) {
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
        Assert(!IsArray());
        return rtti->getConstPtr(Data());
    }

    Any Any::Deref() const
    {
        Assert(!IsArray());
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
        return policy == AnyPolicy::nonConstRef || policy == AnyPolicy::constRef;
    }

    bool Any::IsNonConstRef() const
    {
        return policy == AnyPolicy::nonConstRef;
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
        if (policy == AnyPolicy::nonConstRef) {
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
        if (policy == AnyPolicy::nonConstRef) {
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
        if (policy == AnyPolicy::nonConstRef) {
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
        if (policy == AnyPolicy::nonConstRef) {
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
        if (policy == AnyPolicy::nonConstRef) {
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
        if (policy == AnyPolicy::nonConstRef) {
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
        arrayLength = 0;
        policy = AnyPolicy::max;
        rtti = nullptr;
        info = {};
    }

    bool Any::Empty() const
    {
        return rtti == nullptr;
    }

    size_t Any::Serialize(Common::BinarySerializeStream& inStream) const
    {
        Assert(!IsArray() && rtti != nullptr);
        return rtti->serialize(Data(), inStream);
    }

    std::pair<bool, size_t> Any::Deserialize(Common::BinaryDeserializeStream& inStream)
    {
        Assert(!IsArray() && rtti != nullptr && !IsConstRef());
        return rtti->deserialize(Data(), inStream);
    }

    std::pair<bool, size_t> Any::Deserialize(Common::BinaryDeserializeStream& inStream) const
    {
        Assert(!IsArray() && rtti != nullptr && IsNonConstRef());
        return rtti->deserialize(Data(), inStream);
    }

    void Any::JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator) const
    {
        Assert(!IsArray() && rtti != nullptr);
        rtti->jsonSerialize(Data(), outJsonValue, inAllocator);
    }

    void Any::JsonDeserialize(const rapidjson::Value& inJsonValue)
    {
        Assert(!IsArray() && rtti != nullptr && !IsConstRef());
        rtti->jsonDeserialize(Data(), inJsonValue);
    }

    void Any::JsonDeserialize(const rapidjson::Value& inJsonValue) const
    {
        Assert(!IsArray() && rtti != nullptr && IsNonConstRef());
        return rtti->jsonDeserialize(Data(), inJsonValue);
    }

    std::string Any::ToString() const
    {
        Assert(!IsArray());
        return Empty() ? "" : rtti->toString(Data());
    }

    void* Any::Data(uint32_t inIndex) const
    {
        void* ptr = IsRef() ? std::get<RefInfo>(info).Ptr() : std::get<HolderInfo>(info).Ptr();
        return static_cast<uint8_t*>(ptr) + ElementSize() * inIndex;
    }

    size_t Any::MemorySize() const
    {
        return IsRef() ? std::get<RefInfo>(info).Size() : std::get<HolderInfo>(info).Size();
    }

    Any::operator bool() const
    {
        return !Empty();
    }

    Any Any::operator[](uint32_t inIndex)
    {
        return At(inIndex);
    }

    Any Any::operator[](uint32_t inIndex) const
    {
        return At(inIndex);
    }

    bool Any::operator==(const Any& inAny) const
    {
        Assert(!IsArray());
        if (TypeId() != inAny.TypeId()) {
            return false;
        }
        return rtti->equal(Data(), inAny.Data());
    }

    bool Any::operator!=(const Any& inAny) const
    {
        Assert(!IsArray());
        return !operator==(inAny);
    }

    Any::HolderInfo::HolderInfo() = default;

    Any::HolderInfo::HolderInfo(size_t inMemorySize)
    {
        ResizeMemory(inMemorySize);
    }

    void Any::HolderInfo::ResizeMemory(size_t inSize)
    {
        if (inSize <= MaxStackMemorySize) {
            memory = InplaceMemory {};
            std::get<InplaceMemory>(memory).Resize(inSize);
        } else {
            memory = HeapMemory {};
            std::get<HeapMemory>(memory).resize(inSize);
        }
    }

    void* Any::HolderInfo::Ptr() const
    {
        if (memory.index() == 0) {
            return const_cast<uint8_t*>(std::get<InplaceMemory>(memory).Data());
        }
        return const_cast<uint8_t*>(std::get<HeapMemory>(memory).data());
    }

    size_t Any::HolderInfo::Size() const
    {
        if (memory.index() == 0) {
            return std::get<InplaceMemory>(memory).Size();
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

    bool Argument::IsMemoryHolder() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.IsMemoryHolder();
        });
    }

    bool Argument::IsRef() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.IsRef();
        });
    }

    bool Argument::IsNonConstRef() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.IsNonConstRef();
        });
    }

    bool Argument::IsConstRef() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.IsConstRef();
        });
    }

    const TypeInfo* Argument::Type() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.Type();
        });
    }

    const TypeInfo* Argument::RemoveRefType() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.RemoveRefType();
        });
    }

    const TypeInfo* Argument::AddPointerType() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.AddPointerType();
        });
    }

    const TypeInfo* Argument::RemovePointerType() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.RemovePointerType();
        });
    }

    Id Id::null = Id();

    Id::Id()
        : hash(0)
    {
    }

    Id::Id(std::string inName)
        : hash(Common::HashUtils::StrCrc32(inName))
        , name(std::move(inName))
    {
    }

    bool Id::IsNull() const
    {
        return hash == null.hash;
    }

    bool Id::operator==(const Id& inRhs) const
    {
        return hash == inRhs.hash;
    }

    size_t IdHashProvider::operator()(const Id& inId) const noexcept
    {
        return inId.hash;
    }

    const Id IdPresets::globalScope = Id("_globalScope");
    const Id IdPresets::detor = Id("_detor");
    const Id IdPresets::defaultCtor = Id("_defaultCtor");

    ReflNode::ReflNode(Id inId) : id(std::move(inId)) {}

    ReflNode::~ReflNode() = default;

    const Id& ReflNode::GetId() const
    {
        return id;
    }

    const std::string& ReflNode::GetName() const
    {
        return id.name;
    }

    const std::string& ReflNode::GetMeta(const std::string& key) const
    {
        const auto iter = metas.find(key);
        Assert(iter != metas.end());
        return iter->second;
    }

    std::string ReflNode::GetAllMeta() const
    {
        std::stringstream stream;
        uint32_t count = 0;
        for (const auto& [key, value] : metas) {
            stream << fmt::format("{}={}", key.name, value);

            count++;
            if (count != metas.size()) {
                stream << ";";
            }
        }
        return stream.str();
    }

    bool ReflNode::HasMeta(const std::string& key) const
    {
        return metas.contains(key);
    }

    bool ReflNode::GetMetaBool(const std::string& key) const
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

    int32_t ReflNode::GetMetaInt32(const std::string& key) const
    {
        return std::atoi(GetMeta(key).c_str());
    }

    int64_t ReflNode::GetMetaInt64(const std::string& key) const
    {
        return std::atoll(GetMeta(key).c_str());
    }

    float ReflNode::GetMetaFloat(const std::string& key) const
    {
        return std::atof(GetMeta(key).c_str());
    }

    Variable::Variable(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , owner(std::move(params.owner))
        , memorySize(params.memorySize)
        , typeInfo(params.typeInfo)
        , setter(std::move(params.setter))
        , getter(std::move(params.getter))
    {
    }

    Variable::~Variable() = default;

    Any Variable::Get() const
    {
        return GetDyn();
    }

    const std::string& Variable::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& Variable::GetOwnerId() const
    {
        return owner;
    }

    const Class* Variable::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
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

    Function::Function(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , owner(std::move(params.owner))
        , argsNum(params.argsNum)
        , retTypeInfo(params.retTypeInfo)
        , argTypeInfos(std::move(params.argTypeInfos))
        , invoker(std::move(params.invoker))
    {
    }

    Function::~Function() = default;

    const std::string& Function::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& Function::GetOwnerId() const
    {
        return owner;
    }

    const Class* Function::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
    }

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
        : ReflNode(std::move(params.id))
        , owner(std::move(params.owner))
        , argsNum(params.argsNum)
        , argTypeInfos(std::move(params.argTypeInfos))
        , argRemoveRefTypeInfos(std::move(params.argRemoveRefTypeInfos))
        , argRemovePointerTypeInfos(std::move(params.argRemovePointerTypeInfos))
        , stackConstructor(std::move(params.stackConstructor))
        , heapConstructor(std::move(params.heapConstructor))
    {
    }

    Constructor::~Constructor() = default;

    const std::string& Constructor::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& Constructor::GetOwnerId() const
    {
        return owner;
    }

    const Class* Constructor::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
    }

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
        : ReflNode(std::string(IdPresets::detor.name))
        , owner(std::move(params.owner))
        , destructor(std::move(params.destructor))
    {
    }

    Destructor::~Destructor() = default;

    const std::string& Destructor::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& Destructor::GetOwnerId() const
    {
        return owner;
    }

    const Class* Destructor::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
    }

    void Destructor::InvokeDyn(const Argument& argument) const
    {
        destructor(argument);
    }

    MemberVariable::MemberVariable(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , owner(std::move(params.owner))
        , memorySize(params.memorySize)
        , typeInfo(params.typeInfo)
        , setter(std::move(params.setter))
        , getter(std::move(params.getter))
    {
    }

    MemberVariable::~MemberVariable() = default;

    const std::string& MemberVariable::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& MemberVariable::GetOwnerId() const
    {
        return owner;
    }

    const Class* MemberVariable::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
    }

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

    bool MemberVariable::IsTransient() const
    {
        return HasMeta("transient") && GetMetaBool("transient");
    }

    MemberFunction::MemberFunction(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , owner(std::move(params.owner))
        , argsNum(params.argsNum)
        , retTypeInfo(params.retTypeInfo)
        , argTypeInfos(std::move(params.argTypeInfos))
        , invoker(std::move(params.invoker))
    {
    }

    MemberFunction::~MemberFunction() = default;

    const std::string& MemberFunction::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& MemberFunction::GetOwnerId() const
    {
        return owner;
    }

    const Class* MemberFunction::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Class::Find(owner);
    }

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

    GlobalScope::GlobalScope() : ReflNode(std::string(IdPresets::globalScope.name)) {}

    GlobalScope::~GlobalScope() = default;

    Variable& GlobalScope::EmplaceVariable(const Id& inId, Variable::ConstructParams&& inParams)
    {
        Assert(!variables.contains(inId));
        variables.emplace(inId, Variable(std::move(inParams)));
        return variables.at(inId);
    }

    Function& GlobalScope::EmplaceFunction(const Id& inId, Function::ConstructParams&& inParams)
    {
        Assert(!functions.contains(inId));
        functions.emplace(inId, Function(std::move(inParams)));
        return functions.at(inId);
    }

    const GlobalScope& GlobalScope::Get()
    {
        return Registry::Get().globalScope;
    }

    void GlobalScope::ForEachVariable(const VariableTraverser& func) const
    {
        for (const auto& [id, variable] : variables) {
            func(variable);
        }
    }

    void GlobalScope::ForEachFunction(const FunctionTraverser& func) const
    {
        for (const auto& [id, function] : functions) {
            func(function);
        }
    }

    bool GlobalScope::HasVariable(const Id& inId) const
    {
        return variables.contains(inId);
    }

    const Variable* GlobalScope::FindVariable(const Id& inId) const
    {
        const auto iter = variables.find(inId);
        return iter == variables.end() ? nullptr : &iter->second;
    }

    const Variable& GlobalScope::GetVariable(const Id& inId) const
    {
        const auto iter = variables.find(inId);
        Assert(iter != variables.end());
        return iter->second;
    }

    bool GlobalScope::HasFunction(const Id& inId) const
    {
        return functions.contains(inId);
    }

    const Function* GlobalScope::FindFunction(const Id& inId) const
    {
        const auto iter = functions.find(inId);
        return iter == functions.end() ? nullptr : &iter->second;
    }

    const Function& GlobalScope::GetFunction(const Id& inId) const
    {
        const auto iter = functions.find(inId);
        Assert(iter != functions.end());
        return iter->second;
    }

    std::unordered_map<TypeId, Id> Class::typeToIdMap = {};

    Class::Class(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , typeInfo(params.typeInfo)
        , baseClassGetter(std::move(params.baseClassGetter))
    {
        CreateDefaultObject(params.defaultObjectCreator);
        if (params.destructorParams.has_value()) {
            destructor = Destructor(std::move(params.destructorParams.value()));
        }
        if (params.defaultConstructorParams.has_value()) {
            EmplaceConstructor(IdPresets::defaultCtor, std::move(params.defaultConstructorParams.value()));
        }
    }

    bool Class::Has(const Id& inId)
    {
        const auto& classes = Registry::Get().classes;
        return classes.contains(inId);
    }

    const Class* Class::Find(const Id& inId)
    {
        const auto& classes = Registry::Get().classes;
        const auto iter = classes.find(inId);
        return iter == classes.end() ? nullptr : &iter->second;
    }

    const Class& Class::Get(const Id& inId)
    {
        const auto& classes = Registry::Get().classes;
        const auto iter = classes.find(inId);
        Assert(iter != classes.end());
        return iter->second;
    }

    bool Class::Has(const TypeInfo* typeInfo)
    {
        Assert(typeInfo != nullptr && typeInfo->isClass && !typeInfo->isConst);
        return typeToIdMap.contains(typeInfo->id); // NOLINT
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
        const auto iter = typeToIdMap.find(typeId);
        if (iter == typeToIdMap.end()) {
            return false;
        }
        return Has(iter->second);
    }

    const Class* Class::Find(const TypeId typeId)
    {
        const auto iter = typeToIdMap.find(typeId);
        if (iter == typeToIdMap.end()) {
            return nullptr;
        }
        return Find(iter->second);
    }

    const Class& Class::Get(TypeId typeId)
    {
        const auto iter = typeToIdMap.find(typeId);
        AssertWithReason(iter != typeToIdMap.end(), "did you forget add EClass() annotation to class ?");
        return Get(iter->second);
    }

    std::vector<const Class*> Class::GetAll()
    {
        const auto& classes = Registry::Get().classes;
        std::vector<const Class*> result;
        result.reserve(classes.size());
        for (const auto& [id, clazz] : classes) {
            result.emplace_back(&clazz);
        }
        return result;
    }

    std::vector<const Class*> Class::FindWithCategory(const std::string& category)
    {
        const auto& classes = Registry::Get().classes;
        std::vector<const Class*> result;
        result.reserve(classes.size());
        for (const auto& [id, clazz] : classes) {
            if (clazz.HasMeta("category") && clazz.GetMeta("category") == category) {
                result.emplace_back(&clazz);
            }
        }
        return result;
    }

    Class::~Class() = default;

    void Class::ForEachStaticVariable(const VariableTraverser& func) const
    {
        for (const auto& [id, variable] : staticVariables) {
            func(variable);
        }
    }

    void Class::ForEachStaticFunction(const FunctionTraverser& func) const
    {
        for (const auto& [id, function] : staticFunctions) {
            func(function);
        }
    }

    void Class::ForEachMemberVariable(const MemberVariableTraverser& func) const
    {
        for (const auto& [id, memberVariable] : memberVariables) {
            func(memberVariable);
        }
    }

    void Class::ForEachMemberFunction(const MemberFunctionTraverser& func) const
    {
        for (const auto& [id, memberFunction] : memberFunctions) {
            func(memberFunction);
        }
    }

    void Class::CreateDefaultObject(const std::function<Any()>& inCreator)
    {
        if (inCreator) {
            defaultObject = inCreator();
        }
    }

    Destructor& Class::EmplaceDestructor(Destructor::ConstructParams&& inParams)
    {
        Assert(!destructor.has_value());
        destructor = Destructor(std::move(inParams));
        return destructor.value();
    }

    Constructor& Class::EmplaceConstructor(const Id& inId, Constructor::ConstructParams&& inParams)
    {
        Assert(!constructors.contains(inId));
        constructors.emplace(inId, Constructor(std::move(inParams)));
        return constructors.at(inId);
    }

    Variable& Class::EmplaceStaticVariable(const Id& inId, Variable::ConstructParams&& inParams)
    {
        Assert(!staticVariables.contains(inId));
        staticVariables.emplace(inId, Variable(std::move(inParams)));
        return staticVariables.at(inId);
    }

    Function& Class::EmplaceStaticFunction(const Id& inId, Function::ConstructParams&& inParams)
    {
        Assert(!staticFunctions.contains(inId));
        staticFunctions.emplace(inId, Function(std::move(inParams)));
        return staticFunctions.at(inId);
    }

    MemberVariable& Class::EmplaceMemberVariable(const Id& inId, MemberVariable::ConstructParams&& inParams)
    {
        Assert(!memberVariables.contains(inId));
        memberVariables.emplace(inId, MemberVariable(std::move(inParams)));
        return memberVariables.at(inId);
    }

    MemberFunction& Class::EmplaceMemberFunction(const Id& inId, MemberFunction::ConstructParams&& inParams)
    {
        Assert(!memberFunctions.contains(inId));
        memberFunctions.emplace(inId, MemberFunction(std::move(inParams)));
        return memberFunctions.at(inId);
    }

    const TypeInfo* Class::GetTypeInfo() const
    {
        return typeInfo;
    }

    bool Class::HasDefaultConstructor() const
    {
        return HasConstructor(IdPresets::defaultCtor);
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
        return FindConstructor(IdPresets::defaultCtor);
    }

    const Constructor& Class::GetDefaultConstructor() const
    {
        return GetConstructor(IdPresets::defaultCtor);
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

    bool Class::HasConstructor(const Id& inId) const
    {
        return constructors.contains(inId);
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

    const Constructor* Class::FindConstructor(const Id& inId) const
    {
        const auto iter = constructors.find(inId);
        return iter == constructors.end() ? nullptr : &iter->second;
    }

    const Constructor& Class::GetConstructor(const Id& inId) const
    {
        const auto iter = constructors.find(inId);
        Assert(iter != constructors.end());
        return iter->second;
    }

    bool Class::HasStaticVariable(const Id& inId) const
    {
        return staticVariables.contains(inId);
    }

    const Variable* Class::FindStaticVariable(const Id& inId) const
    {
        const auto iter = staticVariables.find(inId);
        return iter == staticVariables.end() ? nullptr : &iter->second;
    }

    const Variable& Class::GetStaticVariable(const Id& inId) const
    {
        const auto iter = staticVariables.find(inId);
        Assert(iter != staticVariables.end());
        return iter->second;
    }

    bool Class::HasStaticFunction(const Id& inId) const
    {
        return staticFunctions.contains(inId);
    }

    const Function* Class::FindStaticFunction(const Id& inId) const
    {
        const auto iter = staticFunctions.find(inId);
        return iter == staticFunctions.end() ? nullptr : &iter->second;
    }

    const Function& Class::GetStaticFunction(const Id& inId) const
    {
        const auto iter = staticFunctions.find(inId);
        Assert(iter != staticFunctions.end());
        return iter->second;
    }

    bool Class::HasMemberVariable(const Id& inId) const
    {
        return memberVariables.contains(inId);
    }

    const MemberVariable* Class::FindMemberVariable(const Id& inId) const
    {
        const auto iter = memberVariables.find(inId);
        return iter == memberVariables.end() ? nullptr : &iter->second;
    }

    const MemberVariable& Class::GetMemberVariable(const Id& inId) const
    {
        const auto iter = memberVariables.find(inId);
        Assert(iter != memberVariables.end());
        return iter->second;
    }

    bool Class::HasMemberFunction(const Id& inId) const
    {
        return memberFunctions.contains(inId);
    }

    const std::unordered_map<Id, MemberVariable, IdHashProvider>& Class::GetMemberVariables() const
    {
        return memberVariables;
    }

    const MemberFunction* Class::FindMemberFunction(const Id& inId) const
    {
        const auto iter = memberFunctions.find(inId);
        return iter == memberFunctions.end() ? nullptr : &iter->second;
    }

    const MemberFunction& Class::GetMemberFunction(const Id& inId) const
    {
        const auto iter = memberFunctions.find(inId);
        Assert(iter != memberFunctions.end());
        return iter->second;
    }

    Any Class::GetDefaultObject() const
    {
        if (!defaultObject.Empty()) {
            return defaultObject.ConstRef();
        }
        return {};
    }

    EnumValue::EnumValue(ConstructParams&& inParams)
        : ReflNode(std::move(inParams.id))
        , owner(std::move(inParams.owner))
        , getter(std::move(inParams.getter))
        , integralGetter(std::move(inParams.integralGetter))
        , setter(std::move(inParams.setter))
        , comparer(std::move(inParams.comparer))
    {
    }

    EnumValue::~EnumValue() = default;

    Any EnumValue::Get() const
    {
        return getter();
    }

    EnumValue::IntegralValue EnumValue::GetIntegral() const
    {
        return GetIntegralDyn();
    }

    const std::string& EnumValue::GetOwnerName() const
    {
        return owner.name;
    }

    const Id& EnumValue::GetOwnerId() const
    {
        return owner;
    }

    const Enum* EnumValue::GetOwner() const
    {
        return owner.IsNull() ? nullptr : Enum::Find(owner);
    }

    Any EnumValue::GetDyn() const
    {
        return getter();
    }

    EnumValue::IntegralValue EnumValue::GetIntegralDyn() const
    {
        return integralGetter();
    }

    void EnumValue::SetDyn(const Argument& arg) const
    {
        setter(arg);
    }

    bool EnumValue::CompareDyn(const Argument& arg) const
    {
        return comparer(arg);
    }

    const Enum* Enum::Find(const Id& inId)
    {
        const auto& enums = Registry::Get().enums;
        const auto iter = enums.find(inId);
        return iter == enums.end() ? nullptr : &iter->second;
    }

    const Enum& Enum::Get(const Id& inId)
    {
        const auto& enums = Registry::Get().enums;
        const auto iter = enums.find(inId);
        Assert(iter != enums.end());
        return iter->second;
    }

    std::unordered_map<TypeId, Id> Enum::typeToIdMap = {};

    Enum::Enum(ConstructParams&& params)
        : ReflNode(std::move(params.id))
        , typeInfo(params.typeInfo)
    {
    }

    Enum::~Enum() = default;

    const TypeInfo* Enum::GetTypeInfo() const
    {
        return typeInfo;
    }

    bool Enum::HasValue(const Id& inId) const
    {
        return values.contains(inId);
    }

    const EnumValue* Enum::FindValue(const Id& inId) const
    {
        const auto iter = values.find(inId);
        return iter == values.end() ? nullptr : &iter->second;
    }

    const EnumValue& Enum::GetValue(const Id& inId) const
    {
        return values.at(inId);
    }

    bool Enum::HasValue(EnumValue::IntegralValue inValue) const
    {
        return FindValue(inValue) != nullptr;
    }

    const EnumValue* Enum::FindValue(EnumValue::IntegralValue inValue) const
    {
        for (const auto& value : values | std::views::values) {
            if (value.GetIntegralDyn() == inValue) {
                return &value;
            }
        }
        return nullptr;
    }

    const EnumValue& Enum::GetValue(EnumValue::IntegralValue inValue) const
    {
        const auto* value = FindValue(inValue);
        Assert(value != nullptr);
        return *value;
    }

    bool Enum::HasValue(const Argument& inArg) const
    {
        return FindValue(inArg) != nullptr;
    }

    const EnumValue* Enum::FindValue(const Argument& inArg) const
    {
        for (const auto& value : values | std::views::values) {
            if (value.CompareDyn(inArg)) {
                return &value;
            }
        }
        return nullptr;
    }

    const EnumValue& Enum::GetValue(const Argument& inArg) const
    {
        const auto* value = FindValue(inArg);
        Assert(value != nullptr);
        return *value;
    }

    const std::unordered_map<Id, EnumValue, IdHashProvider>& Enum::GetValues() const
    {
        return values;
    }

    std::vector<const EnumValue*> Enum::GetSortedValues() const
    {
        std::vector<const EnumValue*> result;
        for (const auto& value : values | std::views::values) {
            result.emplace_back(&value);
        }
        std::ranges::sort(result, [](const EnumValue* lhs, const EnumValue* rhs) { return lhs->GetIntegralDyn() < rhs->GetIntegralDyn(); });
        return result;
    }

    EnumValue& Enum::EmplaceElement(const Id& inId, EnumValue::ConstructParams&& inParams)
    {
        Assert(!values.contains(inId));
        values.emplace(inId, EnumValue(std::move(inParams)));
        return values.at(inId);
    }
}
