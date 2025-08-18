//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <ranges>
#include <variant>

#include <Common/Serialization.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <Common/Container.h>
#include <Common/Concepts.h>
#include <Mirror/Api.h>
#include <Mirror/Mirror.h>
#include <Mirror/Mirror.h>

#if COMPILER_MSVC
#define functionSignature __FUNCSIG__
#else
#define functionSignature __PRETTY_FUNCTION__
#endif

namespace Mirror {
    using TypeId = uint64_t;

    constexpr TypeId typeIdNull = 0;
}

namespace Mirror {
    struct TypeInfo {
#if BUILD_CONFIG_DEBUG
        // NOTICE: this name is platform relative, so do not use it unless for debug
        const std::string_view debugName;
#endif
        const std::string_view name;
        const TypeId id;
        const TypeId removePointerType;
        const uint32_t isConst : 1;
        const uint32_t isLValueReference : 1;
        const uint32_t isLValueConstReference : 1;
        const uint32_t isRValueReference : 1;
        const uint32_t isReference : 1;
        const uint32_t isPointer : 1;
        const uint32_t isConstPointer : 1;
        const uint32_t isClass : 1;
        const uint32_t isEnum: 1;
        const uint32_t isArray : 1;
        const uint32_t isArithmetic : 1;
        const uint32_t isIntegral : 1;
        const uint32_t isFloatingPoint : 1;
        const uint32_t copyConstructible : 1;
        const uint32_t copyAssignable : 1;
        const uint32_t moveConstructible : 1;
        const uint32_t moveAssignable : 1;
        const uint32_t equalComparable : 1;
    };

    template <typename T> const TypeInfo* GetTypeInfo();
    template <typename T> TypeId GetTypeId();

    struct TypeInfoCompact {
        const TypeInfo* raw;
        const TypeInfo* removeRef;
        const TypeInfo* removePointer;
    };

    class Any;
    class Class;

    MIRROR_API bool PointerConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType);
    MIRROR_API bool PolymorphismConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType, const Class* inSrcDynamicClass);
    MIRROR_API bool Convertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType, const Class* inSrcDynamicClass);

    enum class AnyPolicy : uint8_t {
        memoryHolder,
        nonConstRef,
        constRef,
        max
    };

    using TemplateViewId = uint32_t;
    using TemplateViewRttiPtr = const void*;

    struct AnyRtti {
        using DetorFunc = void(void*) noexcept;
        using CopyConstructFunc = void(void*, const void*);
        using MoveConstructFunc = void(void*, void*) noexcept;
        using CopyAssignFunc = void(void*, const void*);
        using MoveAssignFunc = void(void*, void*) noexcept;
        using EqualFunc = bool(const void*, const void*);
        using GetTypeInfoFunc = const TypeInfo*();
        using GetPtrFunc = Any(void*);
        using GetConstPtrFunc = Any(const void*);
        using DerefFunc = Any(const void*);
        using SerializeFunc = size_t(const void*, Common::BinarySerializeStream&);
        using DeserializeFunc = std::pair<bool, size_t>(void*, Common::BinaryDeserializeStream&);
        using JsonSerializeFunc = void(const void*, rapidjson::Value&, rapidjson::Document::AllocatorType&);
        using JsonDeserializeFunc = void(void*, const rapidjson::Value&);
        using ToStringFunc = std::string(const void*);
        using GetTemplateViewRttiFunc = std::pair<TemplateViewId, TemplateViewRttiPtr>();
        using GetDynamicClassFunc = const Class*(const void*);

        template <typename T> static void Detor(void* inThis) noexcept;
        template <typename T> static void CopyConstruct(void* inThis, const void* inOther);
        template <typename T> static void MoveConstruct(void* inThis, void* inOther) noexcept;
        template <typename T> static void CopyAssign(void* inThis, const void* inOther);
        template <typename T> static void MoveAssign(void* inThis, void* inOther) noexcept;
        template <typename T> static bool Equal(const void* inThis, const void* inOther) noexcept;
        template <typename T> static const TypeInfo* GetValueType();
        template <typename T> static const TypeInfo* GetConstValueType();
        template <typename T> static const TypeInfo* GetRefType();
        template <typename T> static const TypeInfo* GetConstRefType();
        template <typename T> static const TypeInfo* GetRemovePointerType();
        template <typename T> static const TypeInfo* GetAddPointerType();
        template <typename T> static const TypeInfo* GetAddConstPointerType();
        template <typename T> static Any GetPtr(void* inThis);
        template <typename T> static Any GetConstPtr(const void* inThis);
        template <typename T> static Any Deref(const void* inThis);
        template <typename T> static size_t Serialize(const void* inThis, Common::BinarySerializeStream& inStream);
        template <typename T> static std::pair<bool, size_t> Deserialize(void* inThis, Common::BinaryDeserializeStream& inStream);
        template <typename T> static void JsonSerialize(const void* inThis, rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator);
        template <typename T> static void JsonDeserialize(void* inThis, const rapidjson::Value& inJsonValue);
        template <typename T> static std::string ToString(const void* inThis);
        template <typename T> static std::pair<TemplateViewId, TemplateViewRttiPtr> GetTemplateViewRtti();
        template <typename T> static const Class* GetDynamicClass(const void* inThis);

        DetorFunc* detor;
        CopyConstructFunc* copyConstruct;
        MoveConstructFunc* moveConstruct;
        CopyAssignFunc* copyAssign;
        MoveAssignFunc* moveAssign;
        EqualFunc* equal;
        GetTypeInfoFunc* getValueType;
        GetTypeInfoFunc* getConstValueType;
        GetTypeInfoFunc* getRefType;
        GetTypeInfoFunc* getConstRefType;
        GetTypeInfoFunc* getRemovePointerType;
        GetTypeInfoFunc* getAddPointerType;
        GetTypeInfoFunc* getAddConstPointerType;
        GetPtrFunc* getPtr;
        GetConstPtrFunc* getConstPtr;
        DerefFunc* deref;
        SerializeFunc* serialize;
        DeserializeFunc* deserialize;
        JsonSerializeFunc* jsonSerialize;
        JsonDeserializeFunc* jsonDeserialize;
        ToStringFunc* toString;
        GetTemplateViewRttiFunc* getTemplateViewRtti;
        GetDynamicClassFunc* getDynamicClass;
    };

    template <typename T>
    static constexpr AnyRtti anyRttiImpl = {
        &AnyRtti::Detor<T>,
        &AnyRtti::CopyConstruct<T>,
        &AnyRtti::MoveConstruct<T>,
        &AnyRtti::CopyAssign<T>,
        &AnyRtti::MoveAssign<T>,
        &AnyRtti::Equal<T>,
        &AnyRtti::GetValueType<T>,
        &AnyRtti::GetConstValueType<T>,
        &AnyRtti::GetRefType<T>,
        &AnyRtti::GetConstRefType<T>,
        &AnyRtti::GetRemovePointerType<T>,
        &AnyRtti::GetAddPointerType<T>,
        &AnyRtti::GetAddConstPointerType<T>,
        &AnyRtti::GetPtr<T>,
        &AnyRtti::GetConstPtr<T>,
        &AnyRtti::Deref<T>,
        &AnyRtti::Serialize<T>,
        &AnyRtti::Deserialize<T>,
        &AnyRtti::JsonSerialize<T>,
        &AnyRtti::JsonDeserialize<T>,
        &AnyRtti::ToString<T>,
        &AnyRtti::GetTemplateViewRtti<T>,
        &AnyRtti::GetDynamicClass<T>
    };

    template <typename T>
    struct TemplateViewRttiGetter {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    template <typename T>
    concept ValidTemplateView = requires(T object)
    {
        { T::id } -> std::convertible_to<TemplateViewId>;
    };

    class MIRROR_API Any {
    public:
        Any();
        ~Any();
        Any(Any& inOther);
        Any(const Any& inOther);
        Any(const Any& inOther, AnyPolicy inPolicy);
        Any(const Any& inOther, AnyPolicy inPolicy, uint32_t inIndex);
        Any(Any&& inOther) noexcept;

        template <typename T> Any(T&& inValue); // NOLINT
        template <typename T> Any(std::reference_wrapper<T>& inRef); // NOLINT
        template <typename T> Any(std::reference_wrapper<T>&& inRef); // NOLINT
        template <typename T> Any(const std::reference_wrapper<T>& inRef); // NOLINT
        template <typename T, size_t N> Any(T (&inArray)[N]); // NOLINT
        template <typename T, size_t N> Any(T (&&inArray)[N]); // NOLINT
        template <typename T, size_t N> Any(std::reference_wrapper<T[N]>& inArrayRef); // NOLINT
        template <typename T, size_t N> Any(std::reference_wrapper<T[N]>&& inArrayRef); // NOLINT
        template <typename T, size_t N> Any(const std::reference_wrapper<T[N]>& inArrayRef); // NOLINT

        // NOTICE: Any::operator=() is designed to reset this and re-construct from new value
        // if you want to call assign operator, use CopyAssign() and MoveAssign() instead.
        Any& operator=(const Any& inOther);
        Any& operator=(Any&& inOther) noexcept;
        template <typename T> Any& operator=(T&& inValue);
        template <typename T> Any& operator=(std::reference_wrapper<T>& inRef);
        template <typename T> Any& operator=(std::reference_wrapper<T>&& inRef);
        template <typename T> Any& operator=(const std::reference_wrapper<T>& inRef);
        template <typename T, size_t N> Any& operator=(T (&inArray)[N]); // NOLINT
        template <typename T, size_t N> Any& operator=(T (&&inArray)[N]); // NOLINT
        template <typename T, size_t N> Any& operator=(std::reference_wrapper<T[N]>& inArrayRef); // NOLINT
        template <typename T, size_t N> Any& operator=(std::reference_wrapper<T[N]>&& inArrayRef); // NOLINT
        template <typename T, size_t N> Any& operator=(const std::reference_wrapper<T[N]>& inArrayRef); // NOLINT

        Any& CopyAssign(Any& inOther);
        Any& CopyAssign(const Any& inOther);
        Any& CopyAssign(Any&& inOther);
        Any& MoveAssign(Any& inOther) noexcept;
        Any& MoveAssign(const Any& inOther) noexcept;
        Any& MoveAssign(Any&& inOther) noexcept;
        const Any& CopyAssign(Any& inOther) const;
        const Any& CopyAssign(const Any& inOther) const;
        const Any& CopyAssign(Any&& inOther) const;
        const Any& MoveAssign(Any& inOther) const noexcept;
        const Any& MoveAssign(const Any& inOther) const noexcept;
        const Any& MoveAssign(Any&& inOther) const noexcept;

        template <typename T> bool Convertible();
        template <typename T> bool Convertible() const;

        template <typename T> T As();
        template <typename T> T As() const;
        template <Common::CppNotRef T> T* TryAs();
        template <Common::CppNotRef T> T* TryAs() const;

        template <ValidTemplateView V> bool CanAsTemplateView() const;
        TemplateViewId GetTemplateViewId() const;
        TemplateViewRttiPtr GetTemplateViewRtti() const;
        bool HasTemplateView() const;

        bool IsArray() const;
        Any At(uint32_t inIndex);
        Any At(uint32_t inIndex) const;
        size_t ElementSize() const;
        uint32_t ArrayLength() const;

        Any Ref();
        Any Ref() const;
        Any ConstRef() const;
        Any Value() const;
        Any Ptr();
        Any Ptr() const;
        Any ConstPtr() const;
        Any Deref() const;
        AnyPolicy Policy() const;
        bool IsMemoryHolder() const;
        bool IsRef() const;
        bool IsNonConstRef() const;
        bool IsConstRef() const;
        const TypeInfo* Type();
        const TypeInfo* Type() const;
        const TypeInfo* RemoveRefType();
        const TypeInfo* RemoveRefType() const;
        const TypeInfo* AddPointerType();
        const TypeInfo* AddPointerType() const;
        const TypeInfo* RemovePointerType();
        const TypeInfo* RemovePointerType() const;
        Mirror::TypeId TypeId();
        Mirror::TypeId TypeId() const;
        const Class* GetDynamicClass() const;
        void Reset();
        bool Empty() const;
        size_t Serialize(Common::BinarySerializeStream& inStream) const;
        std::pair<bool, size_t> Deserialize(Common::BinaryDeserializeStream& inStream);
        std::pair<bool, size_t> Deserialize(Common::BinaryDeserializeStream& inStream) const;
        void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator) const;
        void JsonDeserialize(const rapidjson::Value& inJsonValue);
        void JsonDeserialize(const rapidjson::Value& inJsonValue) const;
        std::string ToString() const;
        void* Data(uint32_t inIndex = 0) const;
        size_t MemorySize() const;
        explicit operator bool() const;
        Any operator[](uint32_t inIndex);
        Any operator[](uint32_t inIndex) const;
        bool operator==(const Any& inAny) const;
        bool operator!=(const Any& inAny) const;

    private:
        class MIRROR_API HolderInfo {
        public:
            HolderInfo();
            explicit HolderInfo(size_t inMemorySize);

            void ResizeMemory(size_t inSize);
            void* Ptr() const;
            size_t Size() const;

        private:
            static constexpr size_t MaxStackMemorySize = sizeof(std::vector<uint8_t>) - sizeof(size_t);
            using InplaceMemory = Common::InplaceVector<uint8_t, MaxStackMemorySize>;
            using HeapMemory = std::vector<uint8_t>;
            static_assert(sizeof(InplaceMemory) == sizeof(HeapMemory));

            std::variant<InplaceMemory, HeapMemory> memory;
        };

        class MIRROR_API RefInfo {
        public:
            RefInfo();
            RefInfo(void* inPtr, size_t inSize);

            void* Ptr() const;
            size_t Size() const;

        private:
            void* ptr;
            size_t memorySize;
        };

        template <typename T> void ConstructFromValue(T&& inValue);
        template <typename T> void ConstructFromRef(std::reference_wrapper<T> inRef);
        template <typename T, size_t N> void ConstructFromArrayValue(T (&inValue)[N]);
        template <typename T, size_t N> void ConstructFromArrayValue(T (&&inValue)[N]);
        template <typename T, size_t N> void ConstructFromArrayRef(std::reference_wrapper<T[N]> inRef);
        void PerformCopyConstruct(const Any& inOther);
        void PerformCopyConstructWithPolicy(const Any& inOther, AnyPolicy inPolicy);
        void PerformCopyConstructForElementWithPolicy(const Any& inOther, AnyPolicy inPolicy, uint32_t inIndex);
        void PerformMoveConstruct(Any&& inOther) noexcept;
        void PerformCopyAssign(const Any& inOther) const;
        void PerformMoveAssign(const Any& inOther) const noexcept;
        void PerformCopyAssign(Any&& inOther) const;
        void PerformMoveAssign(Any&& inOther) const noexcept;
        uint32_t ElementNum() const;

        uint32_t arrayLength;
        AnyPolicy policy;
        const AnyRtti* rtti;
        std::variant<RefInfo, HolderInfo> info;
    };

    class Variable;
    class MemberVariable;
    class Class;
    class Enum;

    class MIRROR_API Argument {
    public:
        Argument();
        Argument(Any& inAny); // NOLINT
        Argument(const Any& inAny); // NOLINT
        Argument(Any&& inAny); // NOLINT
        Argument& operator=(Any& inAny);
        Argument& operator=(const Any& inAny);
        Argument& operator=(Any&& inAny);

        template <typename T> T As() const;
        template <Common::CppNotRef T> T* TryAs() const;

        template <ValidTemplateView V> bool CanAsTemplateView() const;
        TemplateViewRttiPtr GetTemplateViewRtti() const;

        bool IsMemoryHolder() const;
        bool IsRef() const;
        bool IsNonConstRef() const;
        bool IsConstRef() const;
        const TypeInfo* Type() const;
        const TypeInfo* RemoveRefType() const;
        const TypeInfo* AddPointerType() const;
        const TypeInfo* RemovePointerType() const;
        const Class* GetDynamicClass() const;

    private:
        template <typename F> decltype(auto) Delegate(F&& inFunc) const;

        std::variant<std::monostate, Any*, const Any*, Any> any;
    };

    using ArgumentList = std::vector<Argument>;

    template <typename T> Any ForwardAsAny(T&& value);
    template <typename T> Argument ForwardAsArg(T&& value);
    template <typename... Args> ArgumentList ForwardAsArgList(Args&&... args);
    template <typename T> Any ForwardAsAnyByValue(T&& value);
    template <typename T> Argument ForwardAsArgByValue(T&& value);
    template <typename... Args> ArgumentList ForwardAsArgListByValue(Args&&... args);

    struct MIRROR_API Id {
        static Id null;

        Id();
        template <size_t N> Id(const char (&inName)[N]); // NOLINT
        Id(std::string inName); // NOLINT

        bool IsNull() const;
        bool operator==(const Id& inRhs) const;

        size_t hash;
        std::string name;
    };

    struct MIRROR_API IdHashProvider {
        size_t operator()(const Id& inId) const noexcept;
    };

    struct MIRROR_API IdPresets {
        static const Id globalScope;
        static const Id detor;
        static const Id defaultCtor;
        static const Id copyCtor;
        static const Id moveCtor;
    };

    struct MIRROR_API MetaPresets {
        static constexpr const auto* transient = "transient";
        static constexpr const auto* category = "category";
    };

    class MIRROR_API ReflNode {
    public:
        virtual ~ReflNode();

        const Id& GetId() const;
        const std::string& GetName() const;
        const std::string& GetMeta(const std::string& key) const;
        std::string GetMetaOr(const std::string& key, const std::string& defaultValue) const;
        std::string GetAllMeta() const;
        bool HasMeta(const std::string& key) const;
        bool GetMetaBool(const std::string& key) const;
        bool GetMetaBoolOr(const std::string& key, bool defaultValue) const;
        int32_t GetMetaInt32(const std::string& key) const;
        int32_t GetMetaInt32Or(const std::string& key, int32_t defaultValue) const;
        int64_t GetMetaInt64(const std::string& key) const;
        int64_t GetMetaInt64Or(const std::string& key, int64_t defaultValue) const;
        float GetMetaFloat(const std::string& key) const;
        float GetMetaFloatOr(const std::string& key, float defaultValue) const;

    protected:
        explicit ReflNode(Id inId);

    private:
        template <typename Derived> friend class MetaDataRegistry;

        Id id;
        std::unordered_map<Id, std::string, IdHashProvider> metas;
    };

    enum class FieldAccess : uint8_t {
        faPrivate,
        faProtected,
        faPublic,
        max
    };

    class MIRROR_API Variable final : public ReflNode {
    public:
        ~Variable() override;

        template <typename T> void Set(T&& value) const;
        Any Get() const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class* GetOwner() const;
        FieldAccess GetAccess() const;
        const TypeInfo* GetTypeInfo() const;
        void SetDyn(const Argument& inArgument) const;
        Any GetDyn() const;

    private:
        friend class GlobalRegistry;
        friend class GlobalScope;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(const Argument&)>;
        using Getter = std::function<Any()>;

        struct ConstructParams {
            Id id;
            Id owner;
            FieldAccess access;
            size_t memorySize;
            const TypeInfo* typeInfo;
            Setter setter;
            Getter getter;
        };

        explicit Variable(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        size_t memorySize;
        const TypeInfo* typeInfo;
        Setter setter;
        Getter getter;
    };

    class MIRROR_API Function final : public ReflNode {
    public:
        ~Function() override;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class* GetOwner() const;
        FieldAccess GetAccess() const;
        template <typename... Args> Any Invoke(Args&&... args) const;
        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        Any InvokeDyn(const ArgumentList& inArgumentList) const;

    private:
        friend class GlobalRegistry;
        friend class GlobalScope;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(const ArgumentList&)>;

        struct ConstructParams {
            Id id;
            Id owner;
            FieldAccess access;
            uint8_t argsNum;
            const TypeInfo* retTypeInfo;
            std::vector<const TypeInfo*> argTypeInfos;
            Invoker invoker;
        };

        explicit Function(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        uint8_t argsNum;
        const TypeInfo* retTypeInfo;
        std::vector<const TypeInfo*> argTypeInfos;
        Invoker invoker;
    };

    class MIRROR_API Constructor final : public ReflNode {
    public:
        ~Constructor() override;

        template <typename... Args> Any Construct(Args&&... args) const;
        template <typename... Args> Any New(Args&&... args) const;
        template <typename... Args> Any InplaceNew(Args&&... args) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class& GetOwner() const;
        FieldAccess GetAccess() const;
        uint8_t GetArgsNum() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        const TypeInfo* GetArgRemoveRefTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgRemoveRefTypeInfos() const;
        const TypeInfo* GetArgRemovePointerTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgRemovePointerTypeInfos() const;

        Any ConstructDyn(const ArgumentList& arguments) const;
        Any NewDyn(const ArgumentList& arguments) const;
        Any InplaceNewDyn(void* ptr, const ArgumentList& arguments) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(const ArgumentList&)>;
        using InplaceInvoker = std::function<Any(void*, const ArgumentList&)>;

        struct ConstructParams {
            Id id;
            Id owner;
            FieldAccess access;
            uint8_t argsNum;
            std::vector<const TypeInfo*> argTypeInfos;
            std::vector<const TypeInfo*> argRemoveRefTypeInfos;
            std::vector<const TypeInfo*> argRemovePointerTypeInfos;
            Invoker stackConstructor;
            Invoker heapConstructor;
            InplaceInvoker inplaceConstructor;
        };

        explicit Constructor(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        uint8_t argsNum;
        std::vector<const TypeInfo*> argTypeInfos;
        std::vector<const TypeInfo*> argRemoveRefTypeInfos;
        std::vector<const TypeInfo*> argRemovePointerTypeInfos;
        Invoker stackConstructor;
        Invoker heapConstructor;
        InplaceInvoker inplaceConstructor;
    };

    class MIRROR_API Destructor final : public ReflNode {
    public:
        ~Destructor() override;

        template <typename C> void Destruct(C&& object) const;
        template <typename C> void Delete(C* object) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class& GetOwner() const;
        FieldAccess GetAccess() const;
        void DestructDyn(const Argument& argument) const;
        void DeleteDyn(const Argument& argument) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<void(const Argument&)>;

        struct ConstructParams {
            Id owner;
            FieldAccess access;
            Invoker destructor;
            Invoker deleter;
        };

        explicit Destructor(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        Invoker destructor;
        Invoker deleter;
    };

    class MIRROR_API MemberVariable final : public ReflNode {
    public:
        ~MemberVariable() override;

        template <typename C, typename T> void Set(C&& object, T&& value) const;
        template <typename C> Any Get(C&& object) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class& GetOwner() const;
        FieldAccess GetAccess() const;
        size_t SizeOf() const;
        const TypeInfo* GetTypeInfo() const;
        void SetDyn(const Argument& object, const Argument& value) const;
        Any GetDyn(const Argument& object) const;
        bool IsTransient() const;

    private:
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(const Argument&, const Argument&)>;
        using Getter = std::function<Any(const Argument&)>;

        struct ConstructParams {
            Id id;
            Id owner;
            FieldAccess access;
            size_t memorySize;
            const TypeInfo* typeInfo;
            Setter setter;
            Getter getter;
        };

        explicit MemberVariable(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        size_t memorySize;
        const TypeInfo* typeInfo;
        Setter setter;
        Getter getter;
    };

    class MIRROR_API MemberFunction final : public ReflNode {
    public:
        ~MemberFunction() override;

        template <typename C, typename... Args> Any Invoke(C&& object, Args&&... args) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class& GetOwner() const;
        FieldAccess GetAccess() const;
        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        Any InvokeDyn(const Argument& object, const ArgumentList& arguments) const;

    private:
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(const Argument&, const ArgumentList&)>;

        struct ConstructParams {
            Id id;
            Id owner;
            FieldAccess access;
            uint8_t argsNum;
            const TypeInfo* retTypeInfo;
            std::vector<const TypeInfo*> argTypeInfos;
            Invoker invoker;
        };

        explicit MemberFunction(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        uint8_t argsNum;
        const TypeInfo* retTypeInfo;
        std::vector<const TypeInfo*> argTypeInfos;
        Invoker invoker;
    };

    using VariableTraverser = std::function<void(const Variable&)>;
    using FunctionTraverser = std::function<void(const Function&)>;
    using MemberVariableTraverser = std::function<void(const MemberVariable&)>;
    using MemberFunctionTraverser = std::function<void(const MemberFunction&)>;

    class MIRROR_API GlobalScope final : public ReflNode {
    public:
        ~GlobalScope() override;

        static const GlobalScope& Get();

        void ForEachVariable(const VariableTraverser& func) const;
        void ForEachFunction(const FunctionTraverser& func) const;
        bool HasVariable(const Id& inId) const;
        const Variable* FindVariable(const Id& inId) const;
        const Variable& GetVariable(const Id& inId) const;
        bool HasFunction(const Id& inId) const;
        const Function* FindFunction(const Id& inId) const;
        const Function& GetFunction(const Id& inId) const;

    private:
        friend class Registry;
        friend class GlobalRegistry;

        Variable& EmplaceVariable(const Id& inId, Variable::ConstructParams&& inParams);
        Function& EmplaceFunction(const Id& inId, Function::ConstructParams&& inParams);

        GlobalScope();

        Common::StableUnorderedMap<Id, Variable, 128, IdHashProvider> variables;
        Common::StableUnorderedMap<Id, Function, 128, IdHashProvider> functions;
    };

    class MIRROR_API Class final : public ReflNode {
    public:
        template <Common::CppClass C> static bool Has();
        template <Common::CppClass C> static const Class* Find();
        template <Common::CppClass C> static const Class& Get();

        static bool Has(const Id& inId);
        static const Class* Find(const Id& inId);
        static const Class& Get(const Id& inId);
        static bool Has(const TypeInfo* typeInfo);
        static const Class* Find(const TypeInfo* typeInfo);
        static const Class& Get(const TypeInfo* typeInfo);
        static bool Has(TypeId typeId);
        static const Class* Find(TypeId typeId);
        static const Class& Get(TypeId typeId);
        static std::vector<const Class*> GetAll();
        static std::vector<const Class*> FindWithCategory(const std::string& category);

        ~Class() override;

        template <typename... Args> Any Construct(Args&&... args) const;
        template <typename... Args> Any New(Args&&... args) const;
        template <typename... Args> Any InplaceNew(void* ptr, Args&&... args) const;
        template <typename C> void Destruct(C&& object) const;
        template <typename C> void Delete(C* object) const;
        Any InplaceGetObject(void* ptr) const;

        void ForEachStaticVariable(const VariableTraverser& func) const;
        void ForEachStaticFunction(const FunctionTraverser& func) const;
        void ForEachMemberVariable(const MemberVariableTraverser& func) const;
        void ForEachMemberFunction(const MemberFunctionTraverser& func) const;
        const TypeInfo* GetTypeInfo() const;
        size_t SizeOf() const;
        bool HasDefaultConstructor() const;
        const Class* GetBaseClass() const;
        bool IsBaseOf(const Class* derivedClass) const;
        bool IsDerivedFrom(const Class* baseClass) const;
        const Constructor* FindDefaultConstructor() const;
        const Constructor& GetDefaultConstructor() const;
        bool HasDestructor() const;
        const Destructor* FindDestructor() const;
        const Destructor& GetDestructor() const;
        bool HasConstructor(const Id& inId) const;
        const Constructor* FindSuitableConstructor(const ArgumentList& arguments) const;
        const Constructor* FindConstructor(const Id& inId) const;
        const Constructor& GetConstructor(const Id& inId) const;
        bool HasStaticVariable(const Id& inId) const;
        const Variable* FindStaticVariable(const Id& inId) const;
        const Variable& GetStaticVariable(const Id& inId) const;
        bool HasStaticFunction(const Id& inId) const;
        const Function* FindStaticFunction(const Id& inId) const;
        const Function& GetStaticFunction(const Id& inId) const;
        bool HasMemberVariable(const Id& inId) const;
        const MemberVariable* FindMemberVariable(const Id& inId) const;
        const MemberVariable& GetMemberVariable(const Id& inId) const;
        bool HasMemberFunction(const Id& inId) const;
        const std::unordered_map<Id, MemberVariable, IdHashProvider>& GetMemberVariables() const;
        const MemberFunction* FindMemberFunction(const Id& inId) const;
        const MemberFunction& GetMemberFunction(const Id& inId) const;
        Any GetDefaultObject() const;
        bool IsTransient() const;
        Any ConstructDyn(const ArgumentList& arguments) const;
        Any NewDyn(const ArgumentList& arguments) const;
        Any InplaceNewDyn(void* ptr, const ArgumentList& arguments) const;
        void DestructDyn(const Argument& argument) const;
        void DeleteDyn(const Argument& argument) const;
        Any Cast(const Argument& objPtrOrRef) const;

    private:
        static std::unordered_map<TypeId, Id> typeToIdMap;

        friend class Registry;
        template <typename T> friend class ClassRegistry;

        using BaseClassGetter = std::function<const Class*()>;
        using InplaceGetter = std::function<Any(void*)>;
        using DefaultObjectCreator = std::function<Any()>;
        using Caster = std::function<Any(const Mirror::Argument&)>;

        struct ConstructParams {
            Id id;
            const TypeInfo* typeInfo;
            size_t memorySize;
            BaseClassGetter baseClassGetter;
            InplaceGetter inplaceGetter;
            Caster caster;
            DefaultObjectCreator defaultObjectCreator;
            std::optional<Destructor::ConstructParams> destructorParams;
            std::optional<Constructor::ConstructParams> defaultConstructorParams;
            std::optional<Constructor::ConstructParams> moveConstructorParams;
            std::optional<Constructor::ConstructParams> copyConstructorParams;
        };

        explicit Class(ConstructParams&& params);

        void CreateDefaultObject(const DefaultObjectCreator& inCreator);
        Destructor& EmplaceDestructor(Destructor::ConstructParams&& inParams);
        Constructor& EmplaceConstructor(const Id& inId, Constructor::ConstructParams&& inParams);
        Variable& EmplaceStaticVariable(const Id& inId, Variable::ConstructParams&& inParams);
        Function& EmplaceStaticFunction(const Id& inId, Function::ConstructParams&& inParams);
        MemberVariable& EmplaceMemberVariable(const Id& inId, MemberVariable::ConstructParams&& inParams);
        MemberFunction& EmplaceMemberFunction(const Id& inId, MemberFunction::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        size_t memorySize;
        BaseClassGetter baseClassGetter;
        InplaceGetter inplaceGetter;
        Caster caster;
        Any defaultObject;
        std::optional<Destructor> destructor;
        std::unordered_map<Id, Constructor, IdHashProvider> constructors;
        std::unordered_map<Id, Variable, IdHashProvider> staticVariables;
        std::unordered_map<Id, Function, IdHashProvider> staticFunctions;
        std::unordered_map<Id, MemberVariable, IdHashProvider> memberVariables;
        std::unordered_map<Id, MemberFunction, IdHashProvider> memberFunctions;
    };

    class MIRROR_API EnumValue final : public ReflNode {
    public:
        using IntegralValue = int64_t;

        ~EnumValue() override;

        Any Get() const;
        IntegralValue GetIntegral() const;
        template <Common::CppEnum E> void Set(E& value) const;
        template <Common::CppEnum E> bool Compare(const E& value) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Enum* GetOwner() const;
        Any GetDyn() const;
        IntegralValue GetIntegralDyn() const;
        void SetDyn(const Argument& arg) const;
        bool CompareDyn(const Argument& arg) const;

    private:
        friend class Registry;
        friend class Enum;
        template <typename T> friend class EnumRegistry;

        friend class Enum;

        using Getter = std::function<Any()>;
        using IntegralGetter = std::function<IntegralValue()>;
        using Setter = std::function<void(const Argument&)>;
        using Comparer = std::function<bool(const Argument&)>;

        struct ConstructParams {
            Id id;
            Id owner;
            Getter getter;
            IntegralGetter integralGetter;
            Setter setter;
            Comparer comparer;
        };

        explicit EnumValue(ConstructParams&& inParams);

        Id owner;
        Getter getter;
        IntegralGetter integralGetter;
        Setter setter;
        Comparer comparer;
    };

    class MIRROR_API Enum final : public ReflNode {
    public:
        template <Common::CppEnum T> static const Enum* Find();
        template <Common::CppEnum T> static const Enum& Get();
        static const Enum* Find(const Id& inId);
        static const Enum& Get(const Id& inId);

        ~Enum() override;

        const TypeInfo* GetTypeInfo() const;
        bool HasValue(const Id& inId) const;
        const EnumValue* FindValue(const Id& inId) const;
        const EnumValue& GetValue(const Id& inId) const;
        bool HasValue(EnumValue::IntegralValue inValue) const;
        const EnumValue* FindValue(EnumValue::IntegralValue inValue) const;
        const EnumValue& GetValue(EnumValue::IntegralValue inValue) const;
        template <Common::CppEnum E> bool HasValue(E inValue) const;
        template <Common::CppEnum E> const EnumValue* FindValue(E inValue) const;
        template <Common::CppEnum E> const EnumValue& GetValue(E inValue) const;
        bool HasValue(const Argument& inArg) const;
        const EnumValue* FindValue(const Argument& inArg) const;
        const EnumValue& GetValue(const Argument& inArg) const;
        const std::unordered_map<Id, EnumValue, IdHashProvider>& GetValues() const;
        std::vector<const EnumValue*> GetSortedValues() const;

    private:
        static std::unordered_map<TypeId, Id> typeToIdMap;

        friend class Registry;
        template <typename T> friend class EnumRegistry;

        struct ConstructParams {
            Id id;
            const TypeInfo* typeInfo;
        };

        explicit Enum(ConstructParams&& params);

        EnumValue& EmplaceElement(const Id& inId, EnumValue::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        std::unordered_map<Id, EnumValue, IdHashProvider> values;
    };

    template <typename T> concept MetaClass = requires(T inValue)
    {
        { T::GetStaticClass() } -> std::same_as<const Class&>;
        { inValue.GetClass() } -> std::same_as<const Class&>;
    };
}

namespace Mirror {
    // ---------------- begin std::optional<T> ----------------
    struct StdOptionalViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdOptionalView");

        using GetElementTypeFunc = const TypeInfo*();
        using ResetFunc = void(const Any&);
        using EmplaceFunc = Any(const Any&, const Argument&);
        using EmplaceDefaultFunc = Any(const Any&);
        using HasValueFunc = bool(const Any&);
        using GetValueFunc = Any(const Any&);
        using GetConstValueFunc = Any(const Any&);

        template <typename T> static const TypeInfo* GetElementType();
        template <typename T> static void Reset(const Any& inRef);
        template <typename T> static Any Emplace(const Any& inRef, const Argument& inArg);
        template <typename T> static Any EmplaceDefault(const Any& inRef);
        template <typename T> static bool HasValue(const Any& inRef);
        template <typename T> static Any GetValue(const Any& inRef);
        template <typename T> static Any GetConstValue(const Any& inRef);

        GetElementTypeFunc* getElementType;
        ResetFunc* reset;
        EmplaceFunc* emplace;
        EmplaceDefaultFunc* emplaceDefault;
        HasValueFunc* hasValue;
        GetValueFunc* getValue;
        GetConstValueFunc* getConstValue;
    };

    template <typename T>
    static constexpr StdOptionalViewRtti stdOptionalViewRttiImpl = {
        &StdOptionalViewRtti::GetElementType<T>,
        &StdOptionalViewRtti::Reset<T>,
        &StdOptionalViewRtti::Emplace<T>,
        &StdOptionalViewRtti::EmplaceDefault<T>,
        &StdOptionalViewRtti::HasValue<T>,
        &StdOptionalViewRtti::GetValue<T>,
        &StdOptionalViewRtti::GetConstValue<T>
    };

    template <typename T>
    struct TemplateViewRttiGetter<std::optional<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdOptionalView {
    public:
        static constexpr TemplateViewId id = StdOptionalViewRtti::id;

        explicit StdOptionalView(const Any& inRef);
        NonCopyable(StdOptionalView)
        NonMovable(StdOptionalView)

        const TypeInfo* ElementType() const;
        void Reset() const;
        Any Emplace(const Argument& inTempObj) const;
        Any EmplaceDefault() const;
        bool HasValue() const;
        Any Value() const;
        Any ConstValue() const;

    private:
        Any ref;
        const StdOptionalViewRtti* rtti;
    };
    // ---------------- end std::optional<T>-- ----------------

    // ---------------- begin std::pair<K, V> -----------------
    struct StdPairViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdPairView");

        using GetKeyTypeFunc = const TypeInfo*();
        using GetValueTypeFunc = const TypeInfo*();
        using GetKeyFunc = Any(const Any&);
        using GetValueFunc = Any(const Any&);
        using ResetFunc = void(const Any&);

        template <typename K, typename V> static const TypeInfo* GetKeyType();
        template <typename K, typename V> static const TypeInfo* GetValueType();
        template <typename K, typename V> static Any GetKey(const Any& inRef);
        template <typename K, typename V> static Any GetValue(const Any& inRef);
        template <typename K, typename V> static Any GetConstKey(const Any& inRef);
        template <typename K, typename V> static Any GetConstValue(const Any& inRef);
        template <typename K, typename V> static void Reset(const Any& inRef);

        GetKeyTypeFunc* getKeyType;
        GetValueTypeFunc* getValueType;
        GetKeyFunc* getKey;
        GetValueFunc* getValue;
        GetKeyFunc* getConstKey;
        GetValueFunc* getConstValue;
        ResetFunc* reset;
    };

    template <typename K, typename V>
    static constexpr StdPairViewRtti stdPairViewRttiImpl = {
        &StdPairViewRtti::GetKeyType<K, V>,
        &StdPairViewRtti::GetValueType<K, V>,
        &StdPairViewRtti::GetKey<K, V>,
        &StdPairViewRtti::GetValue<K, V>,
        &StdPairViewRtti::GetConstKey<K, V>,
        &StdPairViewRtti::GetConstValue<K, V>,
        &StdPairViewRtti::Reset<K, V>,
    };

    template <typename K, typename V>
    struct TemplateViewRttiGetter<std::pair<K, V>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdPairView {
    public:
        static constexpr TemplateViewId id = StdPairViewRtti::id;

        explicit StdPairView(const Any& inRef);
        NonCopyable(StdPairView)
        NonMovable(StdPairView)

        const TypeInfo* KeyType() const;
        const TypeInfo* ValueType() const;
        Any Key() const;
        Any Value() const;
        Any ConstKey() const;
        Any ConstValue() const;
        void Reset() const;

    private:
        Any ref;
        const StdPairViewRtti* rtti;
    };
    // ---------------- end std::pair<K, V> -------------------

    // ---------------- begin std::array<T> -------------------
    struct StdArrayViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdArrayView");

        using GetElementTypeFunc = const TypeInfo*();
        using GetSizeFunc = size_t();
        using GetElementFunc = Any(const Any&, size_t);
        using GetConstElementFunc = Any(const Any&, size_t);

        template <typename T, size_t N> static const TypeInfo* GetElementType();
        template <typename T, size_t N> static size_t GetSize();
        template <typename T, size_t N> static Any GetElement(const Any& inRef, size_t inIndex);
        template <typename T, size_t N> static Any GetConstElement(const Any& inRef, size_t inIndex);

        GetElementTypeFunc* getElementType;
        GetSizeFunc* getSize;
        GetElementFunc* getElement;
        GetConstElementFunc* getConstElement;
    };

    template <typename T, size_t N>
    static constexpr StdArrayViewRtti stdArrayViewRttiImpl = {
        &StdArrayViewRtti::GetElementType<T, N>,
        &StdArrayViewRtti::GetSize<T, N>,
        &StdArrayViewRtti::GetElement<T, N>,
        &StdArrayViewRtti::GetConstElement<T, N>,
    };

    template <typename T, size_t N>
    struct TemplateViewRttiGetter<std::array<T, N>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdArrayView {
    public:
        static constexpr TemplateViewId id = StdArrayViewRtti::id;

        explicit StdArrayView(const Any& inRef);
        NonCopyable(StdArrayView)
        NonMovable(StdArrayView)

        const TypeInfo* ElementType() const;
        size_t Size() const;
        Any At(size_t inIndex) const;
        Any ConstAt(size_t inIndex) const;

    private:
        Any ref;
        const StdArrayViewRtti* rtti;
    };
    // ---------------- end std::array<T> ---------------------

    // ---------------- begin std::vector<T> ------------------
    struct StdVectorViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdVectorView");

        using GetElementTypeFunc = const TypeInfo*();
        using GetSizeFunc = size_t(const Any&);
        using ReserveFunc = void(const Any&, size_t);
        using ResizeFunc = void(const Any&, size_t);
        using ClearFunc = void(const Any&);
        using GetElementFunc = Any(const Any&, size_t);
        using GetConstElementFunc = Any(const Any&, size_t);
        using EmplaceBackFunc = Any(const Any&, const Argument&);
        using EmplaceDefaultBackFunc = Any(const Any&);
        using EraseFunc = void(const Any&, size_t);

        template <typename T> static const TypeInfo* GetElementType();
        template <typename T> static size_t GetSize(const Any& inRef);
        template <typename T> static void Reserve(const Any& inRef, size_t inSize);
        template <typename T> static void Resize(const Any& inRef, size_t inSize);
        template <typename T> static void Clear(const Any& inRef);
        template <typename T> static Any GetElement(const Any& inRef, size_t inIndex);
        template <typename T> static Any GetConstElement(const Any& inRef, size_t inIndex);
        template <typename T> static Any EmplaceBack(const Any& inRef, const Argument& inTempObj);
        template <typename T> static Any EmplaceDefaultBack(const Any& inRef);
        template <typename T> static void Erase(const Any& inRef, size_t inIndex);

        GetElementTypeFunc* getElementType;
        GetSizeFunc* getSize;
        ReserveFunc* reserve;
        ResizeFunc* resize;
        ClearFunc* clear;
        GetElementFunc* getElement;
        GetConstElementFunc* getConstElement;
        EmplaceBackFunc* emplaceBack;
        EmplaceDefaultBackFunc* emplaceDefaultBack;
        EraseFunc* erase;
    };

    template <typename T>
    static constexpr StdVectorViewRtti stdVectorViewRttiImpl = {
        &StdVectorViewRtti::GetElementType<T>,
        &StdVectorViewRtti::GetSize<T>,
        &StdVectorViewRtti::Reserve<T>,
        &StdVectorViewRtti::Resize<T>,
        &StdVectorViewRtti::Clear<T>,
        &StdVectorViewRtti::GetElement<T>,
        &StdVectorViewRtti::GetConstElement<T>,
        &StdVectorViewRtti::EmplaceBack<T>,
        &StdVectorViewRtti::EmplaceDefaultBack<T>,
        &StdVectorViewRtti::Erase<T>
    };

    template <typename T>
    struct TemplateViewRttiGetter<std::vector<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdVectorView {
    public:
        static constexpr TemplateViewId id = StdVectorViewRtti::id;

        explicit StdVectorView(const Any& inRef);
        NonCopyable(StdVectorView)
        NonMovable(StdVectorView)

        const TypeInfo* ElementType() const;
        size_t Size() const;
        void Reserve(size_t inSize) const;
        void Resize(size_t inSize) const;
        void Clear() const;
        Any At(size_t inIndex) const;
        Any ConstAt(size_t inIndex) const;
        Any EmplaceBack(const Argument& inTempObj) const;
        Any EmplaceDefaultBack() const;
        void Erase(size_t inIndex) const;

    private:
        Any ref;
        const StdVectorViewRtti* rtti;
    };
    // ---------------- end std::vector<T> --------------------

    // ---------------- begin std::list<T> --------------------
    struct StdListViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdListViewRtti");

        using GetElementTypeFunc = const TypeInfo*();
        using GetSizeFunc = size_t(const Any&);
        using ClearFunc = void(const Any&);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using EmplaceFrontFunc = Any(const Any&, const Argument&);
        using EmplaceBackFunc = Any(const Any&, const Argument&);
        using EmplaceDefaultFrontFunc = Any(const Any&);
        using EmplaceDefaultBackFunc = Any(const Any&);

        template <typename T> static const TypeInfo* GetElementType();
        template <typename T> static size_t GetSize(const Any& inRef);
        template <typename T> static void Clear(const Any& inRef);
        template <typename T> static void Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static Any EmplaceFront(const Any& inRef, const Argument& inTempObj);
        template <typename T> static Any EmplaceBack(const Any& inRef, const Argument& inTempObj);
        template <typename T> static Any EmplaceDefaultFront(const Any& inRef);
        template <typename T> static Any EmplaceDefaultBack(const Any& inRef);

        GetElementTypeFunc* getElementType;
        GetSizeFunc* getSize;
        ClearFunc* clear;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        EmplaceFrontFunc* emplaceFront;
        EmplaceBackFunc* emplaceBack;
        EmplaceDefaultFrontFunc* emplaceDefaultFront;
        EmplaceDefaultBackFunc* emplaceDefaultBack;
    };

    template <typename T>
    static constexpr StdListViewRtti stdListViewRttiImpl = {
        &StdListViewRtti::GetElementType<T>,
        &StdListViewRtti::GetSize<T>,
        &StdListViewRtti::Clear<T>,
        &StdListViewRtti::Traverse<T>,
        &StdListViewRtti::ConstTraverse<T>,
        &StdListViewRtti::EmplaceFront<T>,
        &StdListViewRtti::EmplaceBack<T>,
        &StdListViewRtti::EmplaceDefaultFront<T>,
        &StdListViewRtti::EmplaceDefaultBack<T>
    };

    template <typename T>
    struct TemplateViewRttiGetter<std::list<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdListView {
    public:
        using ElementTraverser = std::function<void(const Any&)>;
        static constexpr TemplateViewId id = StdListViewRtti::id;

        explicit StdListView(const Any& inRef);
        NonCopyable(StdListView)
        NonMovable(StdListView)

        const TypeInfo* ElementType() const;
        size_t Size() const;
        void Clear() const;
        void Traverse(const ElementTraverser& inTraverser) const;
        void ConstTraverse(const ElementTraverser& inTraverser) const;
        Any EmplaceFront(const Argument& inTempObj) const;
        Any EmplaceBack(const Argument& inTempObj) const;
        Any EmplaceDefaultFront() const;
        Any EmplaceDefaultBack() const;

    private:
        Any ref;
        const StdListViewRtti* rtti;
    };
    // ---------------- end std::list<T> ----------------------

    // ------------- begin std::unordered_set<T> --------------
    struct StdUnorderedSetViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdUnorderedSetView");

        using GetElementTypeFunc = const TypeInfo*();
        using CreateElementFunc = Any();
        using GetSizeFunc = size_t(const Any&);
        using ReserveFunc = void(const Any&, size_t);
        using ClearFunc = void(const Any&);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ContainsFunc = bool(const Any&, const Argument&);
        using EmplaceFunc = void(const Any&, const Argument&);
        using EraseFunc = void(const Any&, const Argument&);

        template <typename T> static const TypeInfo* GetElementType();
        template <typename T> static Any CreateElement();
        template <typename T> static size_t GetSize(const Any& inRef);
        template <typename T> static void Reserve(const Any& inRef, size_t inSize);
        template <typename T> static void Clear(const Any& inRef);
        template <typename T> static void Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static bool Contains(const Any& inRef, const Argument& inElement);
        template <typename T> static void Emplace(const Any& inRef, const Argument& inTempObj);
        template <typename T> static void Erase(const Any& inRef, const Argument& inElement);

        GetElementTypeFunc* getElementType;
        CreateElementFunc* createElement;
        GetSizeFunc* getSize;
        ReserveFunc* reserve;
        ClearFunc* clear;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        ContainsFunc* contains;
        EmplaceFunc* emplace;
        EraseFunc* erase;
    };

    template <typename T>
    static constexpr StdUnorderedSetViewRtti stdUnorderedSetViewRttiImpl = {
        &StdUnorderedSetViewRtti::GetElementType<T>,
        &StdUnorderedSetViewRtti::CreateElement<T>,
        &StdUnorderedSetViewRtti::GetSize<T>,
        &StdUnorderedSetViewRtti::Reserve<T>,
        &StdUnorderedSetViewRtti::Clear<T>,
        &StdUnorderedSetViewRtti::Traverse<T>,
        &StdUnorderedSetViewRtti::ConstTraverse<T>,
        &StdUnorderedSetViewRtti::Contains<T>,
        &StdUnorderedSetViewRtti::Emplace<T>,
        &StdUnorderedSetViewRtti::Erase<T>,
    };

    template <typename T>
    struct TemplateViewRttiGetter<std::unordered_set<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdUnorderedSetView {
    public:
        using ElementTraverser = std::function<void(const Any&)>;
        static constexpr TemplateViewId id = StdUnorderedSetViewRtti::id;

        explicit StdUnorderedSetView(const Any& inRef);
        NonCopyable(StdUnorderedSetView)
        NonMovable(StdUnorderedSetView)

        const TypeInfo* ElementType() const;
        Any CreateElement() const;
        size_t Size() const;
        void Reserve(size_t inSize) const;
        void Clear() const;
        void Traverse(const ElementTraverser& inTraverser) const;
        void ConstTraverse(const ElementTraverser& inTraverser) const;
        bool Contains(const Argument& inElement) const;
        void Emplace(const Argument& inTempObj) const;
        void Erase(const Argument& inElement) const;

    private:
        Any ref;
        const StdUnorderedSetViewRtti* rtti;
    };
    // ------------- end std::unordered_set<T> ----------------

    // ----------------- begin std::set<T> --------------------
    struct StdSetViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdSetView");

        using GetElementTypeFunc = const TypeInfo*();
        using CreateElementFunc = Any();
        using GetSizeFunc = size_t(const Any&);
        using ClearFunc = void(const Any&);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ContainsFunc = bool(const Any&, const Argument&);
        using EmplaceFunc = void(const Any&, const Argument&);
        using EraseFunc = void(const Any&, const Argument&);

        template <typename T> static const TypeInfo* GetElementType();
        template <typename T> static Any CreateElement();
        template <typename T> static size_t GetSize(const Any& inRef);
        template <typename T> static void Clear(const Any& inRef);
        template <typename T> static void Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser);
        template <typename T> static bool Contains(const Any& inRef, const Argument& inElement);
        template <typename T> static void Emplace(const Any& inRef, const Argument& inTempObj);
        template <typename T> static void Erase(const Any& inRef, const Argument& inElement);

        GetElementTypeFunc* getElementType;
        CreateElementFunc* createElement;
        GetSizeFunc* getSize;
        ClearFunc* clear;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        ContainsFunc* contains;
        EmplaceFunc* emplace;
        EraseFunc* erase;
    };

    template <typename T>
    static constexpr StdSetViewRtti stdSetViewRttiImpl = {
        &StdSetViewRtti::GetElementType<T>,
        &StdSetViewRtti::CreateElement<T>,
        &StdSetViewRtti::GetSize<T>,
        &StdSetViewRtti::Clear<T>,
        &StdSetViewRtti::Traverse<T>,
        &StdSetViewRtti::ConstTraverse<T>,
        &StdSetViewRtti::Contains<T>,
        &StdSetViewRtti::Emplace<T>,
        &StdSetViewRtti::Erase<T>
    };

    template <typename T>
    struct TemplateViewRttiGetter<std::set<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdSetView {
    public:
        using ElementTraverser = std::function<void(const Any&)>;
        static constexpr TemplateViewId id = StdSetViewRtti::id;

        explicit StdSetView(const Any& inRef);
        NonCopyable(StdSetView)
        NonMovable(StdSetView)

        const TypeInfo* ElementType() const;
        Any CreateElement() const;
        size_t Size() const;
        void Clear() const;
        void Traverse(const ElementTraverser& inTraverser) const;
        void ConstTraverse(const ElementTraverser& inTraverser) const;
        bool Contains(const Argument& inElement) const;
        void Emplace(const Argument& inTempObj) const;
        void Erase(const Argument& inElement) const;

    private:
        Any ref;
        const StdSetViewRtti* rtti;
    };
    // ----------------- end std::set<T> ----------------------

    // ------------- begin std::unordered_map<K, V> -----------
    struct StdUnorderedMapViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdUnorderedMapView");

        using GetKeyTypeFunc = const TypeInfo*();
        using GetValueTypeFunc = const TypeInfo*();
        using CreateKeyFunc = Any();
        using CreateValueFunc = Any();
        using GetSizeFunc = size_t(const Any&);
        using ReserveFunc = void(const Any&, size_t);
        using ClearFunc = void(const Any&);
        using AtFunc = Any(const Any&, const Argument&);
        using ConstAtFunc = Any(const Any&, const Argument&);
        using GetOrAddFunc = Any(const Any&, const Argument&);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&, const Any&)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&, const Any&)>&);
        using ContainsFunc = bool(const Any&, const Argument&);
        using EmplaceFunc = void(const Any&, const Argument&, const Argument&);
        using EraseFunc = void(const Any&, const Argument&);

        template <typename K, typename V> static const TypeInfo* GetKeyType();
        template <typename K, typename V> static const TypeInfo* GetValueType();
        template <typename K, typename V> static Any CreateKey();
        template <typename K, typename V> static Any CreateValue();
        template <typename K, typename V> static size_t GetSize(const Any& inRef);
        template <typename K, typename V> static void Reserve(const Any& inRef, size_t inSize);
        template <typename K, typename V> static void Clear(const Any& inRef);
        template <typename K, typename V> static Any At(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static Any ConstAt(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static Any GetOrAdd(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static void Traverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser);
        template <typename K, typename V> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser);
        template <typename K, typename V> static bool Contains(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static void Emplace(const Any& inRef, const Argument& inTempKey, const Argument& inTempValue);
        template <typename K, typename V> static void Erase(const Any& inRef, const Argument& inKey);

        GetKeyTypeFunc* getKeyType;
        GetValueTypeFunc* getValueType;
        CreateKeyFunc* createKey;
        CreateValueFunc* createValue;
        GetSizeFunc* getSize;
        ReserveFunc* reserve;
        ClearFunc* clear;
        AtFunc* at;
        ConstAtFunc* constAt;
        GetOrAddFunc* getOrAdd;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        ContainsFunc* contains;
        EmplaceFunc* emplace;
        EraseFunc* erase;
    };

    template <typename K, typename V>
    static constexpr StdUnorderedMapViewRtti stdUnorderedMapViewRttiImpl = {
        &StdUnorderedMapViewRtti::GetKeyType<K, V>,
        &StdUnorderedMapViewRtti::GetValueType<K, V>,
        &StdUnorderedMapViewRtti::CreateKey<K, V>,
        &StdUnorderedMapViewRtti::CreateValue<K, V>,
        &StdUnorderedMapViewRtti::GetSize<K, V>,
        &StdUnorderedMapViewRtti::Reserve<K, V>,
        &StdUnorderedMapViewRtti::Clear<K, V>,
        &StdUnorderedMapViewRtti::At<K, V>,
        &StdUnorderedMapViewRtti::ConstAt<K, V>,
        &StdUnorderedMapViewRtti::GetOrAdd<K, V>,
        &StdUnorderedMapViewRtti::Traverse<K, V>,
        &StdUnorderedMapViewRtti::ConstTraverse<K, V>,
        &StdUnorderedMapViewRtti::Contains<K, V>,
        &StdUnorderedMapViewRtti::Emplace<K, V>,
        &StdUnorderedMapViewRtti::Erase<K, V>
    };

    template <typename  K, typename V>
    struct TemplateViewRttiGetter<std::unordered_map<K, V>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };;

    class MIRROR_API StdUnorderedMapView {
    public:
        using PairTraverser = std::function<void(const Any&, const Any&)>;
        static constexpr TemplateViewId id = StdUnorderedMapViewRtti::id;

        explicit StdUnorderedMapView(const Any& inRef);
        NonCopyable(StdUnorderedMapView)
        NonMovable(StdUnorderedMapView)

        const TypeInfo* KeyType() const;
        const TypeInfo* ValueType() const;
        Any CreateKey() const;
        Any CreateValue() const;
        size_t Size() const;
        void Reserve(size_t inSize) const;
        void Clear() const;
        Any At(const Argument& inKey) const;
        Any ConstAt(const Argument& inKey) const;
        Any GetOrAdd(const Argument& inKey) const;
        void Traverse(const PairTraverser& inTraverser) const;
        void ConstTraverse(const PairTraverser& inTraverser) const;
        bool Contains(const Argument& inKey) const;
        void Emplace(const Argument& inTempKey, const Argument& inTempValue) const;
        void Erase(const Argument& inKey) const;

    private:
        Any ref;
        const StdUnorderedMapViewRtti* rtti;
    };
    // ------------- end std::unordered_map<K, V> -------------

    // ----------------- begin std::map<K, V> -----------------
    struct StdMapViewRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdMapView");

        using GetKeyTypeFunc = const TypeInfo*();
        using GetValueTypeFunc = const TypeInfo*();
        using CreateKeyFunc = Any();
        using CreateValueFunc = Any();
        using GetSizeFunc = size_t(const Any&);
        using ClearFunc = void(const Any&);
        using AtFunc = Any(const Any&, const Argument&);
        using ConstAtFunc = Any(const Any&, const Argument&);
        using GetOrAddFunc = Any(const Any&, const Argument&);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&, const Any&)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&, const Any&)>&);
        using ContainsFunc = bool(const Any&, const Argument&);
        using EmplaceFunc = void(const Any&, const Argument&, const Argument&);
        using EraseFunc = void(const Any&, const Argument&);

        template <typename K, typename V> static const TypeInfo* GetKeyType();
        template <typename K, typename V> static const TypeInfo* GetValueType();
        template <typename K, typename V> static Any CreateKey();
        template <typename K, typename V> static Any CreateValue();
        template <typename K, typename V> static size_t GetSize(const Any& inRef);
        template <typename K, typename V> static void Clear(const Any& inRef);
        template <typename K, typename V> static Any At(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static Any ConstAt(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static Any GetOrAdd(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static void Traverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser);
        template <typename K, typename V> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser);
        template <typename K, typename V> static bool Contains(const Any& inRef, const Argument& inKey);
        template <typename K, typename V> static void Emplace(const Any& inRef, const Argument& inTempKey, const Argument& inTempValue);
        template <typename K, typename V> static void Erase(const Any& inRef, const Argument& inKey);

        GetKeyTypeFunc* getKeyType;
        GetValueTypeFunc* getValueType;
        CreateKeyFunc* createKey;
        CreateValueFunc* createValue;
        GetSizeFunc* getSize;
        ClearFunc* clear;
        AtFunc* at;
        ConstAtFunc* constAt;
        GetOrAddFunc* getOrAdd;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        ContainsFunc* contains;
        EmplaceFunc* emplace;
        EraseFunc* erase;
    };

    template <typename K, typename V>
    static constexpr StdMapViewRtti stdMapViewRttiImpl = {
        &StdMapViewRtti::GetKeyType<K, V>,
        &StdMapViewRtti::GetValueType<K, V>,
        &StdMapViewRtti::CreateKey<K, V>,
        &StdMapViewRtti::CreateValue<K, V>,
        &StdMapViewRtti::GetSize<K, V>,
        &StdMapViewRtti::Clear<K, V>,
        &StdMapViewRtti::At<K, V>,
        &StdMapViewRtti::ConstAt<K, V>,
        &StdMapViewRtti::GetOrAdd<K, V>,
        &StdMapViewRtti::Traverse<K, V>,
        &StdMapViewRtti::ConstTraverse<K, V>,
        &StdMapViewRtti::Contains<K, V>,
        &StdMapViewRtti::Emplace<K, V>,
        &StdMapViewRtti::Erase<K, V>
    };

    template <typename K, typename V>
    struct TemplateViewRttiGetter<std::map<K, V>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdMapView {
    public:
        using PairTraverser = std::function<void(const Any&, const Any&)>;
        static constexpr TemplateViewId id = StdMapViewRtti::id;

        explicit StdMapView(const Any& inRef);
        NonCopyable(StdMapView)
        NonMovable(StdMapView)

        const TypeInfo* KeyType() const;
        const TypeInfo* ValueType() const;
        Any CreateKey() const;
        Any CreateValue() const;
        size_t Size() const;
        void Clear() const;
        Any At(const Argument& inKey) const;
        Any ConstAt(const Argument& inKey) const;
        Any GetOrAdd(const Argument& inKey) const;
        void Traverse(const PairTraverser& inTraverser) const;
        void ConstTraverse(const PairTraverser& inTraverser) const;
        bool Contains(const Argument& inKey) const;
        void Emplace(const Argument& inTempKey, const Argument& inTempValue) const;
        void Erase(const Argument& inKey) const;

    private:
        Any ref;
        const StdMapViewRtti* rtti;
    };
    // ----------------- end std::map<K, V> -------------------

    // --------------- begin std::tuple<T...> -----------------
    struct StdTupleRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdTupleView");

        using GetSizeFunc = size_t();
        using GetElementTypeFunc = const TypeInfo*(size_t);
        using CreateElementFunc = Any(size_t);
        using GetElementFunc = Any(const Any&, size_t);
        using GetConstElementFunc = Any(const Any&, size_t);
        using TraverseFunc = void(const Any&, const std::function<void(const Any&, size_t)>&);
        using ConstTraverseFunc = void(const Any&, const std::function<void(const Any&, size_t)>&);

        template <typename... T> static size_t GetSize();
        template <typename... T> static const TypeInfo* GetElementType(size_t inIndex);
        template <typename... T> static Any CreateElement(size_t inIndex);
        template <typename... T> static Any GetElement(const Any& inRef, size_t inIndex);
        template <typename... T> static Any GetConstElement(const Any& inRef, size_t inIndex);
        template <typename... T> static void Traverse(const Any& inRef, const std::function<void(const Any&, size_t)>& inVisitor);
        template <typename... T> static void ConstTraverse(const Any& inRef, const std::function<void(const Any&, size_t)>& inVisitor);

        GetSizeFunc* getSize;
        GetElementTypeFunc* getElementType;
        CreateElementFunc* createElement;
        GetElementFunc* getElement;
        GetConstElementFunc* getConstElement;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
    };

    template <typename... T>
    static constexpr StdTupleRtti stdTupleRttiImpl = {
        &StdTupleRtti::GetSize<T...>,
        &StdTupleRtti::GetElementType<T...>,
        &StdTupleRtti::CreateElement<T...>,
        &StdTupleRtti::GetElement<T...>,
        &StdTupleRtti::GetConstElement<T...>,
        &StdTupleRtti::Traverse<T...>,
        &StdTupleRtti::ConstTraverse<T...>
    };

    template <typename... T>
    struct TemplateViewRttiGetter<std::tuple<T...>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdTupleView {
    public:
        using Traverser = std::function<void(const Any&, size_t)>;
        static constexpr TemplateViewId id = StdTupleRtti::id;

        explicit StdTupleView(const Any& inRef);
        NonCopyable(StdTupleView)
        NonMovable(StdTupleView)

        size_t Size() const;
        const TypeInfo* ElementType(size_t inIndex) const;
        Any CreateElement(size_t inIndex) const;
        Any Get(size_t inIndex) const;
        void Traverse(const Traverser& inVisitor) const;
        void ConstTraverse(const Traverser& inVisitor) const;

    private:
        Any ref;
        const StdTupleRtti* rtti;
    };
    // --------------- end std::tuple<T...> -------------------

    // --------------- begin std::variant<T...> ---------------
    struct StdVariantRtti {
        static constexpr TemplateViewId id = Common::HashUtils::StrCrc32("Mirror::StdVariantView");

        using TypeNumFunc = size_t();
        using TypeByIndexFunc = const TypeInfo*(size_t);
        using CreateElementFunc = Any(size_t);
        using IndexFunc = size_t(const Any&);
        using GetElementFunc = Any(const Any&, size_t);
        using GetConstElementFunc = Any(const Any&, size_t);
        using VisitFunc = void(const Any&, const std::function<void(const Any&)>&);
        using ConstVisitFunc = void(const Any&, const std::function<void(const Any&)>&);
        using EmplaceFunc = Any(const Any&, size_t, const Argument&);

        template <typename... T> static size_t TypeNum();
        template <typename... T> static const TypeInfo* TypeByIndex(size_t inIndex);
        template <typename... T> static Any CreateElement(size_t inIndex);
        template <typename... T> static size_t Index(const Any& inRef);
        template <typename... T> static Any GetElement(const Any& inRef, size_t inIndex);
        template <typename... T> static Any GetConstElement(const Any& inRef, size_t inIndex);
        template <typename... T> static void Visit(const Any& inRef, const std::function<void(const Any&)>& inVisitor);
        template <typename... T> static void ConstVisit(const Any& inRef, const std::function<void(const Any&)>& inVisitor);
        template <typename... T> static Any Emplace(const Any& inRef, size_t inIndex, const Argument& inTempObj);

        TypeNumFunc* typeNum;
        TypeByIndexFunc* typeByIndex;
        CreateElementFunc* createElement;
        IndexFunc* index;
        GetElementFunc* getElement;
        GetConstElementFunc* getConstElement;
        VisitFunc* visit;
        ConstVisitFunc* constVisit;
        EmplaceFunc* emplace;
    };

    template <typename... T>
    static constexpr StdVariantRtti stdVariantRttiImpl = {
        &StdVariantRtti::TypeNum<T...>,
        &StdVariantRtti::TypeByIndex<T...>,
        &StdVariantRtti::CreateElement<T...>,
        &StdVariantRtti::Index<T...>,
        &StdVariantRtti::GetElement<T...>,
        &StdVariantRtti::GetConstElement<T...>,
        &StdVariantRtti::Visit<T...>,
        &StdVariantRtti::ConstVisit<T...>,
        &StdVariantRtti::Emplace<T...>
    };

    template <typename... T>
    struct TemplateViewRttiGetter<std::variant<T...>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    class MIRROR_API StdVariantView {
    public:
        using Visitor = std::function<void(const Any&)>;
        static constexpr TemplateViewId id = StdVariantRtti::id;

        explicit StdVariantView(const Any& inRef);
        NonCopyable(StdVariantView)
        NonMovable(StdVariantView)

        size_t TypeNum() const;
        const TypeInfo* TypeByIndex(size_t inIndex) const;
        Any CreateElement(size_t inIndex) const;
        size_t Index() const;
        Any GetElement(size_t inIndex) const;
        Any GetConstElement(size_t inIndex) const;
        void Visit(const Visitor& inVisitor) const;
        Any Emplace(size_t inIndex, const Argument& inTempObj) const;

    private:
        Any ref;
        const StdVariantRtti* rtti;
    };
    // --------------- end std::variant<T...> -----------------
}

namespace Mirror::Internal {
    template <typename T>
    void StaticCheckArgumentType()
    {
        using RawType = std::remove_cvref_t<T>;
        static_assert(
            !std::is_same_v<RawType, Any> && !std::is_same_v<RawType, Any*> && !std::is_same_v<RawType, const Any*> && !std::is_same_v<RawType, Argument>,
            "static version reflection method do no support use Any/Any*/const Any*/Argument as argument, please check you arguments");
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any()>, sizeof...(T)> BuildTupleDynElementCreatorArray(std::index_sequence<I...>)
    {
        return {
            []() -> Any {
                return T();
            }...
        };
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any(const Argument&)>, sizeof...(T)> BuildTupleDynGetterArray(std::index_sequence<I...>)
    {
        return {
            [](const Argument& inObj) -> Any {
                return std::get<I>(inObj.As<std::tuple<T...>&>());
            }...
        };
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any(const Argument&)>, sizeof...(T)> BuildTupleDynConstGetterArray(std::index_sequence<I...>)
    {
        return {
            [](const Argument& inObj) -> Any {
                return std::get<I>(inObj.As<const std::tuple<T...>&>());
            }...
        };
    }

    template <typename... T, size_t... I>
    static void TraverseTupleDyn(const Argument& inObj, const std::function<void(const Any&, size_t)>& inVisitor, std::index_sequence<I...>)
    {
        auto& tuple = inObj.As<std::tuple<T...>&>();
        (void) std::initializer_list<int> { ([&]() -> void {
            inVisitor(std::ref(std::get<I>(tuple)), I);
        }(), 0)... };
    }

    template <typename... T, size_t... I>
    static void ConstTraverseTupleDyn(const Argument& inObj, const std::function<void(const Any&, size_t)>& inVisitor, std::index_sequence<I...>)
    {
        const auto& tuple = inObj.As<const std::tuple<T...>&>();
        (void) std::initializer_list<int> { ([&]() -> void {
            inVisitor(std::ref(std::get<I>(tuple)), I);
        }(), 0)... };
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any(const Argument&)>, sizeof...(T)> BuildVariantDynGetterArray(std::index_sequence<I...>)
    {
        return {
            [](const Argument& inObj) -> Any {
                return std::get<I>(inObj.As<std::variant<T...>&>());
            }...
        };
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any(const Argument&)>, sizeof...(T)> BuildVariantDynConstGetterArray(std::index_sequence<I...>)
    {
        return {
            [](const Argument& inObj) -> Any {
                return std::get<I>(inObj.As<const std::variant<T...>&>());
            }...
        };
    }

    template <typename... T, size_t... I>
    static std::array<std::function<Any(const Argument&, const Argument&)>, sizeof...(T)> BuildVariantDynEmplaceFuncArray(std::index_sequence<I...>)
    {
        return {
            [](const Argument& inObj, const Argument& inTempObj) -> Any {
                return inObj.As<std::variant<T...>&>().template emplace<T>(std::move(inTempObj.As<T&>()));
            }...
        };
    }
}

namespace Common { // NOLINT
    template <Mirror::MetaClass T>
    struct Serializer<T> {
        static constexpr size_t typeId = HashUtils::StrCrc32("_MetaObject");

        // struct
        // std::string className                  : classNameSize
        // size_t baseContentSize                 : sizeof(size_t)
        // void* baseContent                      : baseContentSize
        // size_t memberVariableCount             : sizeof(size_t)
        // size_t[] memberVariableContentEnds     : sizeof(size_t) * memberVariableCount
        // void*[] memberVariableContent          : memberVariablesContentSize
        //     |- std::string memberVariableName  : memberVariableNameSize
        //     |- bool sameAsDefaultObject        : sizeof(bool)
        //     |- void* memberVariableContent     : memberVariableEnd - memberVariableLastEnd

        static size_t SerializeDyn(BinarySerializeStream& stream, const Mirror::Class& clazz, const Mirror::Argument& obj)
        {
            Assert(!clazz.IsTransient());
            const auto& className = clazz.GetName();
            const auto* baseClass = clazz.GetBaseClass();
            const auto& memberVariables = clazz.GetMemberVariables();
            const auto defaultObject = clazz.GetDefaultObject();

            const auto classNameSize = Serializer<std::string>::Serialize(stream, className);

            uint64_t baseClassContentSize = 0;
            stream.Seek(sizeof(uint64_t));
            if (baseClass != nullptr) {
                baseClassContentSize = SerializeDyn(stream, *baseClass, obj);
            }

            stream.Seek(-static_cast<int64_t>(baseClassContentSize) - static_cast<int64_t>(sizeof(uint64_t)));
            Serializer<uint64_t>::Serialize(stream, baseClassContentSize);
            stream.Seek(static_cast<int64_t>(baseClassContentSize));

            const uint64_t memberVariableCount = memberVariables.size();
            std::vector<uint64_t> memberVariableContentEnds;
            memberVariableContentEnds.reserve(memberVariableCount);

            stream.Seek(static_cast<int64_t>(sizeof(uint64_t) * (memberVariableCount + 1)));
            uint64_t memberVariableContentSize = 0;
            for (const auto& memberVariable : memberVariables | std::views::values) {
                if (memberVariable.IsTransient()) {
                    continue;
                }

                const bool sameAsDefaultObject = defaultObject.Empty() || !memberVariable.GetTypeInfo()->equalComparable
                    ? false
                    : memberVariable.GetDyn(obj) == memberVariable.GetDyn(defaultObject);

                memberVariableContentSize += Serializer<std::string>::Serialize(stream, memberVariable.GetName());
                memberVariableContentSize += Serializer<bool>::Serialize(stream, sameAsDefaultObject);
                if (!sameAsDefaultObject) {
                    memberVariableContentSize += memberVariable.GetDyn(obj).Serialize(stream);
                }
                memberVariableContentEnds.emplace_back(memberVariableContentSize);
            }

            stream.Seek(-static_cast<int64_t>(memberVariableContentSize) - static_cast<int64_t>(sizeof(uint64_t) * (memberVariableCount + 1)));
            Serializer<uint64_t>::Serialize(stream, memberVariableCount);
            for (const auto& end : memberVariableContentEnds) {
                Serializer<uint64_t>::Serialize(stream, end);
            }
            stream.Seek(static_cast<int64_t>(memberVariableContentSize));
            return classNameSize + baseClassContentSize + sizeof(uint64_t) * (memberVariableCount + 2) + memberVariableContentSize; // NOLINT
        }

        static size_t DeserializeDyn(BinaryDeserializeStream& stream, const Mirror::Class& clazz, const Mirror::Argument& obj)
        {
            Assert(!clazz.IsTransient());
            const auto& className = clazz.GetName();
            const auto* baseClass = clazz.GetBaseClass();
            const auto defaultObject = clazz.GetDefaultObject();

            std::string name;
            const auto nameSize = Serializer<std::string>::Deserialize(stream, name);
            if (name != className) {
                return nameSize;
            }

            uint64_t aspectBaseClassContentSize = 0;
            Serializer<uint64_t>::Deserialize(stream, aspectBaseClassContentSize);
            if (aspectBaseClassContentSize != 0 && baseClass != nullptr) {
                const auto actualBaseClassContentSize = DeserializeDyn(stream, *baseClass, obj);
                stream.Seek(static_cast<int64_t>(aspectBaseClassContentSize) - static_cast<int64_t>(actualBaseClassContentSize));
            }

            uint64_t memberVariableCount = 0;
            Serializer<uint64_t>::Deserialize(stream, memberVariableCount);

            std::vector<uint64_t> memberVariableEnds;
            memberVariableEnds.resize(memberVariableCount);
            for (auto& offset : memberVariableEnds) {
                Serializer<uint64_t>::Deserialize(stream, offset);
            }

            uint64_t memberVariableContentCur = 0;
            for (const auto& end : memberVariableEnds) {
                std::string memberVariableName;
                memberVariableContentCur += Serializer<std::string>::Deserialize(stream, memberVariableName);

                if (!clazz.HasMemberVariable(memberVariableName)) {
                    stream.Seek(static_cast<int64_t>(end) - static_cast<int64_t>(memberVariableContentCur));
                    memberVariableContentCur = end;
                    continue;
                }
                const auto& memberVariable = clazz.GetMemberVariable(memberVariableName);

                bool sameAsDefaultObject = false;
                memberVariableContentCur += Serializer<bool>::Deserialize(stream, sameAsDefaultObject);
                if (sameAsDefaultObject) {
                    if (!defaultObject.Empty()) {
                        memberVariable.SetDyn(obj, memberVariable.GetDyn(defaultObject));
                    }
                    continue;
                }

                memberVariableContentCur += memberVariable.GetDyn(obj).Deserialize(stream).second;
                stream.Seek(static_cast<int64_t>(end) - static_cast<int64_t>(memberVariableContentCur));
                memberVariableContentCur = end;
            }
            return nameSize + aspectBaseClassContentSize + sizeof(uint64_t) * (memberVariableCount + 2) + memberVariableContentCur;
        }

        static size_t Serialize(BinarySerializeStream& stream, const T& value)
        {
            return SerializeDyn(stream, Mirror::Class::Get<T>(), Mirror::ForwardAsArg(value));
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, T& value)
        {
            return DeserializeDyn(stream, Mirror::Class::Get<T>(), Mirror::ForwardAsArg(value));
        }
    };

    template <CppEnum E>
    struct Serializer<E> {
        static constexpr size_t typeId = HashUtils::StrCrc32("_MetaEnum");

        static size_t Serialize(BinarySerializeStream& stream, const E& value)
        {
            size_t serialized = 0;
            const Mirror::Enum* metaEnum = Mirror::Enum::Find<E>();
            serialized += Serializer<bool>::Serialize(stream, metaEnum != nullptr);

            if (metaEnum == nullptr) {
                serialized += Serializer<std::underlying_type_t<E>>::Serialize(stream, static_cast<std::underlying_type_t<E>>(value));
            } else {
                const Mirror::EnumValue& metaEnumValue = metaEnum->GetValue(value);
                serialized += Serializer<std::string>::Serialize(stream, metaEnum->GetName());
                serialized += Serializer<std::string>::Serialize(stream, metaEnumValue.GetName());
            }
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, E& value)
        {
            size_t deserialized = 0;
            bool isMetaEnum = false;
            deserialized += Serializer<bool>::Deserialize(stream, isMetaEnum);

            if (isMetaEnum) {
                std::string metaEnumName;
                std::string metaEnumValueName;
                deserialized += Serializer<std::string>::Deserialize(stream, metaEnumName);
                deserialized += Serializer<std::string>::Deserialize(stream, metaEnumValueName);

                const Mirror::Enum* aspectMetaEnum = Mirror::Enum::Find(metaEnumName);
                const Mirror::Enum* metaEnum = Mirror::Enum::Find<E>();
                if (aspectMetaEnum != metaEnum || metaEnum == nullptr) {
                    return deserialized;
                }

                const auto* metaEnumValue = metaEnum->FindValue(metaEnumValueName);
                if (metaEnumValue == nullptr) {
                    return deserialized;
                }
                metaEnumValue->Set(value);
            } else {
                std::underlying_type_t<E> underlyingValue;
                deserialized += Serializer<std::underlying_type_t<E>>::Deserialize(stream, underlyingValue);
                value = static_cast<E>(underlyingValue);
            }
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Variable*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Variable*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Variable* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Variable*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            if (const Mirror::Class* owner = Mirror::Class::Find(ownerName);
                owner != nullptr) {
                value = owner->FindStaticVariable(name);
            } else {
                value = Mirror::GlobalScope::Get().FindVariable(name);
            }
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Function*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Function*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Function* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Function*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            if (const Mirror::Class* owner = Mirror::Class::Find(ownerName);
                owner != nullptr) {
                value = owner->FindStaticFunction(name);
            } else {
                value = Mirror::GlobalScope::Get().FindFunction(name);
            }
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Constructor*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Constructor*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Constructor* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Constructor*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            value = owner != nullptr ? owner->FindConstructor(name) : nullptr;
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Destructor*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Destructor*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Destructor* value)
        {
            const std::string ownerName = value != nullptr ? value->GetOwnerName() : "";
            return Serializer<std::string>::Serialize(stream, ownerName);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Destructor*& value)
        {
            std::string ownerName;
            const size_t deserialized = Serializer<std::string>::Deserialize(stream, ownerName);
            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            value = owner != nullptr ? &owner->GetDestructor() : nullptr;
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::MemberVariable*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::MemberVariable*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::MemberVariable* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::MemberVariable*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            value = owner != nullptr ? owner->FindMemberVariable(name) : nullptr;
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::MemberFunction*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::MemberFunction*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::MemberFunction* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::MemberFunction*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            value = owner != nullptr ? owner->FindMemberFunction(name) : nullptr;
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Class*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Class*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Class* value)
        {
            const std::string name = value != nullptr ? value->GetName() : "";
            return Serializer<std::string>::Serialize(stream, name);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Class*& value)
        {
            std::string name;
            const size_t deserialized = Serializer<std::string>::Deserialize(stream, name);
            value = Mirror::Class::Find(name);
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::EnumValue*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::EnumValue*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::EnumValue* value)
        {
            std::string ownerName;
            std::string name;

            if (value != nullptr) {
                ownerName = value->GetOwnerName();
                name = value->GetName();
            }

            size_t serialized = 0;
            serialized += Serializer<std::string>::Serialize(stream, ownerName);
            serialized += Serializer<std::string>::Serialize(stream, name);
            return serialized;
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::EnumValue*& value)
        {
            std::string ownerName;
            std::string name;

            size_t deserialized = 0;
            deserialized += Serializer<std::string>::Deserialize(stream, ownerName);
            deserialized += Serializer<std::string>::Deserialize(stream, name);

            const Mirror::Enum* owner = Mirror::Enum::Find(ownerName);
            value = owner != nullptr ? owner->FindValue(name) : nullptr;
            return deserialized;
        }
    };

    template <>
    struct Serializer<const Mirror::Enum*> {
        static constexpr size_t typeId = HashUtils::StrCrc32("const Mirror::Enum*");

        static size_t Serialize(BinarySerializeStream& stream, const Mirror::Enum* value)
        {
            const std::string name = value != nullptr ? value->GetName() : "";
            return Serializer<std::string>::Serialize(stream, name);
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, const Mirror::Enum*& value)
        {
            std::string name;
            const size_t deserialized = Serializer<std::string>::Deserialize(stream, name);
            value = Mirror::Enum::Find(name);
            return deserialized;
        }
    };

    template <Mirror::MetaClass T>
    struct JsonSerializer<T> {
        static void JsonSerializeDyn(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Class& clazz, const Mirror::Argument& inObj)
        {
            const auto* baseClass = clazz.GetBaseClass();
            const auto& memberVariables = clazz.GetMemberVariables();
            const auto defaultObject = clazz.GetDefaultObject();

            if (!outJsonValue.IsObject()) {
                outJsonValue.SetObject();
            }

            if (baseClass != nullptr) {
                rapidjson::Value baseContentValue;
                JsonSerializeDyn(baseContentValue, inAllocator, *baseClass, inObj);
                outJsonValue.AddMember("_base", baseContentValue, inAllocator);
            }

            for (const auto& memberVariable : memberVariables | std::views::values) {
                if (memberVariable.IsTransient()) {
                    continue;
                }

                bool sameAsDefault = defaultObject.Empty() || !memberVariable.GetTypeInfo()->equalComparable
                    ? false
                    : memberVariable.GetDyn(inObj) == memberVariable.GetDyn(defaultObject);

                rapidjson::Value memberNameJson;
                JsonSerializer<std::string>::JsonSerialize(memberNameJson, inAllocator, memberVariable.GetName());

                if (sameAsDefault) {
                    continue;
                }
                rapidjson::Value memberContentJson;
                memberVariable.GetDyn(inObj).JsonSerialize(memberContentJson, inAllocator);
                outJsonValue.AddMember(memberNameJson, memberContentJson, inAllocator);
            }
        }

        static void JsonDeserializeDyn(const rapidjson::Value& inJsonValue, const Mirror::Class& clazz, const Mirror::Argument& outObj)
        {
            const auto* baseClass = clazz.GetBaseClass();
            const auto defaultObject = clazz.GetDefaultObject();

            if (!inJsonValue.IsObject()) {
                return;
            }

            if (baseClass != nullptr && inJsonValue.HasMember("_base")) {
                JsonDeserializeDyn(inJsonValue["_base"], *baseClass, outObj);
            }

            for (const auto& memberVariable : clazz.GetMemberVariables() | std::views::values) {
                const auto& memberName = memberVariable.GetName();
                if (inJsonValue.HasMember(memberName.c_str())) {
                    memberVariable.GetDyn(outObj).JsonDeserialize(inJsonValue[memberName.c_str()]);
                } else {
                    if (!defaultObject.Empty()) {
                        memberVariable.SetDyn(outObj, memberVariable.GetDyn(defaultObject));
                    }
                }
            }
        }

        static void JsonSerialize(rapidjson::Value& outValue, rapidjson::Document::AllocatorType& inAllocator, const T& inValue)
        {
            JsonSerializeDyn(outValue, inAllocator, Mirror::Class::Get<T>(), Mirror::ForwardAsArg(inValue));
        }

        static void JsonDeserialize(const rapidjson::Value& inValue, T& outValue)
        {
            JsonDeserializeDyn(inValue, Mirror::Class::Get<T>(), Mirror::ForwardAsArg(outValue));
        }
    };

    template <CppEnum E>
    struct JsonSerializer<E> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const E& inValue)
        {
            if (const Mirror::Enum* metaEnum = Mirror::Enum::Find<E>();
                metaEnum == nullptr) {
                JsonSerializer<std::underlying_type_t<E>>::JsonSerialize(outJsonValue, inAllocator, static_cast<std::underlying_type_t<E>>(inValue));
            } else {
                const Mirror::EnumValue& metaEnumValue = metaEnum->GetValue(inValue);

                rapidjson::Value enumNameJson;
                JsonSerializer<std::string>::JsonSerialize(enumNameJson, inAllocator, metaEnum->GetName());

                rapidjson::Value valueNameJson;
                JsonSerializer<std::string>::JsonSerialize(valueNameJson, inAllocator, metaEnumValue.GetName());

                outJsonValue.SetArray();
                outJsonValue.PushBack(enumNameJson, inAllocator);
                outJsonValue.PushBack(valueNameJson, inAllocator);
            }
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, E& outValue)
        {
            if (inJsonValue.IsArray()) {
                if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                    return;
                }

                std::string metaEnumName;
                std::string metaEnumValueName;
                JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], metaEnumName);
                JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], metaEnumValueName);

                const Mirror::Enum* aspectMetaEnum = Mirror::Enum::Find(metaEnumName);
                const Mirror::Enum* metaEnum = Mirror::Enum::Find<E>();
                if (aspectMetaEnum != metaEnum || metaEnum == nullptr) {
                    return;
                }

                const auto* metaEnumValue = metaEnum->FindValue(metaEnumValueName);
                if (metaEnumValue == nullptr) {
                    return;
                }
                metaEnumValue->Set(outValue);
            } else {
                std::underlying_type_t<E> unlderlyingValue;
                JsonSerializer<std::underlying_type_t<E>>::JsonDeserialize(inJsonValue, unlderlyingValue);
                outValue = static_cast<E>(unlderlyingValue);
            }
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Variable*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Variable* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Variable*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? owner->FindStaticVariable(name) : Mirror::GlobalScope::Get().FindVariable(name);
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Function*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Function* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Function*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? owner->FindStaticFunction(name) : Mirror::GlobalScope::Get().FindFunction(name);
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Constructor*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Constructor* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Constructor*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? owner->FindConstructor(name) : nullptr;
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Destructor*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Destructor* inValue)
        {
            const std::string ownerName = inValue != nullptr ? inValue->GetOwnerName() : "";
            JsonSerializer<std::string>::JsonSerialize(outJsonValue, inAllocator, ownerName);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Destructor*& outValue)
        {
            std::string ownerName;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue, ownerName);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? &owner->GetDestructor() : nullptr;
        }
    };

    template <>
    struct JsonSerializer<const Mirror::MemberVariable*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::MemberVariable* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::MemberVariable*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? owner->FindMemberVariable(name) : nullptr;
        }
    };

    template <>
    struct JsonSerializer<const Mirror::MemberFunction*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::MemberFunction* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::MemberFunction*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Class* owner = Mirror::Class::Find(ownerName);
            outValue = owner != nullptr ? owner->FindMemberFunction(name) : nullptr;
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Class*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Class* inValue)
        {
            const std::string name = inValue != nullptr ? inValue->GetName() : "";
            JsonSerializer<std::string>::JsonSerialize(outJsonValue, inAllocator, name);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Class*& outValue)
        {
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue, name);
            outValue = Mirror::Class::Find(name);
        }
    };

    template <>
    struct JsonSerializer<const Mirror::EnumValue*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::EnumValue* inValue)
        {
            std::string ownerName;
            std::string name;

            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }

            rapidjson::Value ownerNameJson;
            JsonSerializer<std::string>::JsonSerialize(ownerNameJson, inAllocator, ownerName);

            rapidjson::Value nameJson;
            JsonSerializer<std::string>::JsonSerialize(nameJson, inAllocator, name);

            outJsonValue.SetArray();
            outJsonValue.PushBack(ownerNameJson, inAllocator);
            outJsonValue.PushBack(nameJson, inAllocator);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::EnumValue*& outValue)
        {
            if (!inJsonValue.IsArray() || inJsonValue.Size() != 2) {
                return;
            }

            std::string ownerName;
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[0], ownerName);
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue[1], name);

            const Mirror::Enum* owner = Mirror::Enum::Find(ownerName);
            outValue = owner != nullptr ? owner->FindValue(name) : nullptr;
        }
    };

    template <>
    struct JsonSerializer<const Mirror::Enum*> {
        static void JsonSerialize(rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator, const Mirror::Enum* inValue)
        {
            const std::string name = inValue != nullptr ? inValue->GetName() : "";
            JsonSerializer<std::string>::JsonSerialize(outJsonValue, inAllocator, name);
        }

        static void JsonDeserialize(const rapidjson::Value& inJsonValue, const Mirror::Enum*& outValue)
        {
            std::string name;
            JsonSerializer<std::string>::JsonDeserialize(inJsonValue, name);
            outValue = Mirror::Enum::Find(name);
        }
    };

    template <Mirror::MetaClass T>
    struct StringConverter<T> {
        static std::string ToStringDyn(const Mirror::Class& clazz, const Mirror::Argument& argument)
        {
            const auto& memberVariables = clazz.GetMemberVariables();
            const Mirror::Class* baseClass = clazz.GetBaseClass();

            std::stringstream stream;
            stream << "{ ";
            if (baseClass != nullptr) {
                stream << std::format("_base: {}", ToStringDyn(*baseClass, argument));
            }

            auto count = 0;
            for (const auto& [id, var] : memberVariables) {
                stream << std::format("{}: {}", id.name, var.GetDyn(argument).ToString());
                if (count++ != memberVariables.size() - 1) {
                    stream << ", ";
                }
            }
            stream << "}";
            return stream.str();
        }

        static std::string ToString(const T& inValue)
        {
            return ToStringDyn(Mirror::Class::Get<T>(), Mirror::ForwardAsArg(inValue));
        }
    };

    template <CppEnum E>
    struct StringConverter<E> {
        static std::string ToString(const E& inValue)
        {
            if (const Mirror::Enum* metaEnum = Mirror::Enum::Find<E>();
                metaEnum != nullptr) {
                return std::format(
                    "{}::{}",
                    StringConverter<std::string>::ToString(metaEnum->GetName()),
                    StringConverter<std::string>::ToString(metaEnum->GetValue(inValue).GetName()));
            } else {
                return StringConverter<std::underlying_type_t<E>>::ToString(static_cast<std::underlying_type_t<E>>(inValue));
            }
        }
    };

    template <>
    struct StringConverter<const Mirror::Variable*> {
        static std::string ToString(const Mirror::Variable* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}{}{}", ownerName, ownerName.empty() ? "" : ":", name);
        }
    };

    template <>
    struct StringConverter<const Mirror::Function*> {
        static std::string ToString(const Mirror::Variable* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}{}{}{}", ownerName, ownerName.empty() ? "" : ":", name, name.empty() ? "" : "()");
        }
    };

    template <>
    struct StringConverter<const Mirror::Constructor*> {
        static std::string ToString(const Mirror::Constructor* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}::{}({})", ownerName, StringUtils::AfterLast(ownerName, "::"), name);
        }
    };

    template <>
    struct StringConverter<const Mirror::Destructor*> {
        static std::string ToString(const Mirror::Destructor* inValue)
        {
            const std::string ownerName = inValue != nullptr ? inValue->GetOwnerName() : "";
            return std::format("{}::~{}()", ownerName, StringUtils::AfterLast(ownerName, "::"));
        }
    };

    template <>
    struct StringConverter<const Mirror::MemberVariable*> {
        static std::string ToString(const Mirror::MemberVariable* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}::{}", ownerName, name);
        }
    };

    template <>
    struct StringConverter<const Mirror::MemberFunction*> {
        static std::string ToString(const Mirror::MemberFunction* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}::{}()", ownerName, name);
        }
    };

    template <>
    struct StringConverter<const Mirror::Class*> {
        static std::string ToString(const Mirror::Class* inValue)
        {
            return inValue != nullptr ? inValue->GetName() : "";
        }
    };

    template <>
    struct StringConverter<const Mirror::EnumValue*> {
        static std::string ToString(const Mirror::EnumValue* inValue)
        {
            std::string ownerName;
            std::string name;
            if (inValue != nullptr) {
                ownerName = inValue->GetOwnerName();
                name = inValue->GetName();
            }
            return std::format("{}::{}", ownerName, name);
        }
    };

    template <>
    struct StringConverter<const Mirror::Enum*> {
        static std::string ToString(const Mirror::Enum* inValue)
        {
            return inValue != nullptr ? inValue->GetName() : "";
        }
    };
}

namespace Mirror {
    template <typename T>
    const TypeInfo* GetTypeInfo()
    {
        static TypeInfo typeInfo = {
#if BUILD_CONFIG_DEBUG
            functionSignature,
#endif
            typeid(T).name(),
            typeid(T).hash_code(),
            typeid(std::remove_pointer_t<T>).hash_code(),
            Common::CppConst<T>,
            Common::CppLValueRef<T>,
            Common::CppLValueConstRef<T>,
            Common::CppRValueRef<T>,
            Common::CppRef<T>,
            Common::CppPointer<T>,
            Common::CppConstPointer<T>,
            Common::CppClass<T>,
            Common::CppEnum<T>,
            Common::CppArray<T>,
            Common::CppArithmetic<T>,
            Common::CppIntegral<T>,
            Common::CppFloatingPoint<T>,
            Common::CppCopyConstructible<T>,
            Common::CppCopyAssignable<T>,
            Common::CppMoveConstructible<T>,
            Common::CppMoveAssignable<T>,
            Common::EqualComparable<T>
        };
        return &typeInfo;
    }

    template <typename T>
    TypeId GetTypeId()
    {
        return GetTypeInfo<T>()->id;
    }

    template <typename T>
    void AnyRtti::Detor(void* inThis) noexcept
    {
        static_cast<T*>(inThis)->~T();
    }

    template <typename T>
    void AnyRtti::CopyConstruct(void* inThis, const void* inOther)
    {
        if constexpr (std::is_copy_constructible_v<T>) {
            new(inThis) T(*static_cast<const T*>(inOther));
        } else {
            QuickFailWithReason(std::format("type {} is no support copy construct", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    void AnyRtti::MoveConstruct(void* inThis, void* inOther) noexcept
    {
        if constexpr (std::is_move_constructible_v<T>) {
            new(inThis) T(std::move(*static_cast<T*>(inOther)));
        } else {
            QuickFailWithReason(std::format("type {} is no support move construct", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    void AnyRtti::CopyAssign(void* inThis, const void* inOther)
    {
        if constexpr (std::is_copy_assignable_v<T>) {
            *static_cast<T*>(inThis) = *static_cast<const T*>(inOther);
        } else {
            QuickFailWithReason(std::format("type {} is no support copy assign", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    void AnyRtti::MoveAssign(void* inThis, void* inOther) noexcept
    {
        if constexpr (std::is_move_assignable_v<T>) {
            *static_cast<T*>(inThis) = std::move(*static_cast<T*>(inOther));
        } else {
            QuickFailWithReason(std::format("type {} is no support move assign", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    bool AnyRtti::Equal(const void* inThis, const void* inOther) noexcept
    {
        if constexpr (Common::EqualComparable<T>) {
            return *static_cast<const T*>(inThis) == *static_cast<const T*>(inOther);
        } else {
            QuickFailWithReason(std::format("type {} is no support equal compare", GetTypeInfo<T>()->name));
            return false;
        }
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetValueType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetConstValueType()
    {
        return GetTypeInfo<const T>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetRefType()
    {
        return GetTypeInfo<T&>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetConstRefType()
    {
        return GetTypeInfo<const T&>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetRemovePointerType()
    {
        return GetTypeInfo<std::remove_pointer_t<T>>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetAddPointerType()
    {
        return GetTypeInfo<std::add_pointer_t<T>>();
    }

    template <typename T>
    const TypeInfo* AnyRtti::GetAddConstPointerType()
    {
        return GetTypeInfo<std::add_pointer_t<std::add_const_t<T>>>();
    }

    template <typename T>
    Any AnyRtti::GetPtr(void* inThis)
    {
        if constexpr (!std::is_pointer_v<T>) {
            return { static_cast<T*>(inThis) };
        } else {
            QuickFailWithReason("AnyRtti::GetPtr() only support non-pointer type");
            return {};
        }
    }

    template <typename T>
    Any AnyRtti::GetConstPtr(const void* inThis)
    {
        if constexpr (!std::is_pointer_v<T>) {
            return { static_cast<const T*>(inThis) };
        } else {
            QuickFailWithReason("AnyRtti::GetConstPtr() only support non-pointer type");
            return {};
        }
    }

    template <typename T>
    Any AnyRtti::Deref(const void* inThis)
    {
        if constexpr (std::is_pointer_v<T>) {
            return { std::ref(**static_cast<const T*>(inThis)) };
        } else {
            QuickFailWithReason("AnyRtti::Dref() only support pointer type");
            return {};
        }
    }

    template <typename T>
    size_t AnyRtti::Serialize(const void* inThis, Common::BinarySerializeStream& inStream)
    {
        return Common::Serialize<T>(inStream, *static_cast<const T*>(inThis));
    }

    template <typename T>
    std::pair<bool, size_t> AnyRtti::Deserialize(void* inThis, Common::BinaryDeserializeStream& inStream)
    {
        return Common::Deserialize<T>(inStream, *static_cast<T*>(inThis));
    }

    template <typename T>
    void AnyRtti::JsonSerialize(const void* inThis, rapidjson::Value& outJsonValue, rapidjson::Document::AllocatorType& inAllocator)
    {
        Common::JsonSerialize(outJsonValue, inAllocator, *static_cast<const T*>(inThis));
    }

    template <typename T>
    void AnyRtti::JsonDeserialize(void* inThis, const rapidjson::Value& inJsonValue)
    {
        Common::JsonDeserialize(inJsonValue, *static_cast<T*>(inThis));
    }

    template <typename T>
    std::string AnyRtti::ToString(const void* inThis)
    {
        return Common::ToString(*static_cast<const T*>(inThis));
    }

    template <typename T>
    std::pair<TemplateViewId, TemplateViewRttiPtr> AnyRtti::GetTemplateViewRtti()
    {
        return {
            TemplateViewRttiGetter<T>::Id(),
            TemplateViewRttiGetter<T>::Get()};
    }

    template <typename T>
    const Class* AnyRtti::GetDynamicClass(const void* inThis)
    {
        if constexpr (std::is_pointer_v<T>) {
            if constexpr (MetaClass<std::decay_t<std::remove_pointer_t<T>>>) {
                return &(*static_cast<const T*>(inThis))->GetClass();
            } else {
                return nullptr;
            }
        } else {
            if constexpr (MetaClass<T>) {
                return &static_cast<const T*>(inThis)->GetClass();
            } else {
                return nullptr;
            }
        }
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<T>::Id()
    {
        return 0;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<T>::Get()
    {
        return nullptr;
    }

    template <typename T>
    Any::Any(T&& inValue)
    {
        ConstructFromValue(std::forward<T>(inValue));
    }

    template <typename T>
    Any::Any(std::reference_wrapper<T>& inRef)
    {
        ConstructFromRef(inRef);
    }

    template <typename T>
    Any::Any(std::reference_wrapper<T>&& inRef)
    {
        ConstructFromRef(inRef);
    }

    template <typename T>
    Any::Any(const std::reference_wrapper<T>& inRef)
    {
        ConstructFromRef(inRef);
    }

    template <typename T, size_t N>
    Any::Any(T(& inArray)[N])
    {
        ConstructFromArrayValue(inArray);
    }

    template <typename T, size_t N>
    Any::Any(T(&& inArray)[N])
    {
        ConstructFromArrayValue(std::move(inArray));
    }

    template <typename T, size_t N>
    Any::Any(std::reference_wrapper<T[N]>& inArrayRef)
    {
        ConstructFromArrayRef(inArrayRef);
    }

    template <typename T, size_t N>
    Any::Any(std::reference_wrapper<T[N]>&& inArrayRef)
    {
        ConstructFromArrayRef(inArrayRef);
    }

    template <typename T, size_t N>
    Any::Any(const std::reference_wrapper<T[N]>& inArrayRef)
    {
        ConstructFromArrayRef(inArrayRef);
    }

    template <typename T>
    Any& Any::operator=(T&& inValue)
    {
        Reset();
        ConstructFromValue(std::forward<T>(inValue));
        return *this;
    }

    template <typename T>
    Any& Any::operator=(std::reference_wrapper<T>& inRef)
    {
        Reset();
        ConstructFromRef(inRef);
        return *this;
    }

    template <typename T>
    Any& Any::operator=(std::reference_wrapper<T>&& inRef)
    {
        Reset();
        ConstructFromRef(inRef);
        return *this;
    }

    template <typename T>
    Any& Any::operator=(const std::reference_wrapper<T>& inRef)
    {
        Reset();
        ConstructFromRef(inRef);
        return *this;
    }

    template <typename T, size_t N>
    Any& Any::operator=(T(& inArray)[N])
    {
        Reset();
        ConstructFromArrayValue(inArray);
        return *this;
    }

    template <typename T, size_t N>
    Any& Any::operator=(T(&& inArray)[N])
    {
        Reset();
        ConstructFromArrayValue(std::move(inArray));
        return *this;
    }

    template <typename T, size_t N>
    Any& Any::operator=(std::reference_wrapper<T[N]>& inArrayRef)
    {
        Reset();
        ConstructFromArrayRef(inArrayRef);
        return *this;
    }

    template <typename T, size_t N>
    Any& Any::operator=(std::reference_wrapper<T[N]>&& inArrayRef)
    {
        Reset();
        ConstructFromArrayRef(inArrayRef);
        return *this;
    }

    template <typename T, size_t N>
    Any& Any::operator=(const std::reference_wrapper<T[N]>& inArrayRef)
    {
        Reset();
        ConstructFromArrayRef(inArrayRef);
        return *this;
    }

    template <typename T>
    bool Any::Convertible()
    {
        Assert(!IsArray());
        return Mirror::Convertible(
            { Type(), RemoveRefType(), RemovePointerType() },
            { GetTypeInfo<T>(), GetTypeInfo<std::remove_reference_t<T>>(), GetTypeInfo<std::remove_pointer_t<T>>() },
            rtti->getDynamicClass(Data()));
    }

    template <typename T>
    bool Any::Convertible() const
    {
        Assert(!IsArray());
        return Mirror::Convertible(
            { Type(), RemoveRefType(), RemovePointerType() },
            { GetTypeInfo<T>(), GetTypeInfo<std::remove_reference_t<T>>(), GetTypeInfo<std::remove_pointer_t<T>>() },
            rtti->getDynamicClass(Data()));
    }

    template <typename T>
    T Any::As()
    {
        Assert(Convertible<T>());
        return static_cast<T>(*static_cast<std::remove_cvref_t<T>*>(Data()));
    }

    template <typename T>
    T Any::As() const
    {
        Assert(Convertible<T>());
        return static_cast<T>(*static_cast<std::remove_cvref_t<T>*>(Data()));
    }

    template <Common::CppNotRef T>
    T* Any::TryAs()
    {
        Assert(!IsArray());
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() },
            rtti->getDynamicClass(Data()));
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <Common::CppNotRef T>
    T* Any::TryAs() const
    {
        Assert(!IsArray());
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() },
            rtti->getDynamicClass(Data()));
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <ValidTemplateView V>
    bool Any::CanAsTemplateView() const
    {
        Assert(!Empty());
        return rtti->getTemplateViewRtti().first == V::id;
    }

    template <typename T>
    void Any::ConstructFromValue(T&& inValue)
    {
        using RawType = std::remove_cvref_t<T>;

        arrayLength = 0;
        policy = AnyPolicy::memoryHolder;
        rtti = &anyRttiImpl<RawType>;
        info = HolderInfo(sizeof(RawType));
        new(Data()) RawType(std::forward<T>(inValue));
    }

    template <typename T>
    void Any::ConstructFromRef(std::reference_wrapper<T> inRef)
    {
        using RawType = std::remove_cv_t<T>;

        arrayLength = 0;
        policy = std::is_const_v<T> ? AnyPolicy::constRef : AnyPolicy::nonConstRef;
        rtti = &anyRttiImpl<RawType>;
        info = RefInfo(const_cast<RawType*>(std::addressof(inRef.get())), sizeof(RawType));
    }

    template <typename T, size_t N>
    void Any::ConstructFromArrayValue(T(& inValue)[N])
    {
        using RawType = std::remove_cv_t<T>;

        arrayLength = N;
        policy = AnyPolicy::memoryHolder;
        rtti = &anyRttiImpl<RawType>;
        info = HolderInfo(sizeof(RawType) * N);
        for (auto i = 0; i < N; i++) {
            new(Data(i)) RawType(inValue[i]);
        }
    }

    template <typename T, size_t N>
    void Any::ConstructFromArrayValue(T(&& inValue)[N])
    {
        using RawType = std::remove_cv_t<T>;

        arrayLength = N;
        policy = AnyPolicy::memoryHolder;
        rtti = &anyRttiImpl<RawType>;
        info = HolderInfo(sizeof(RawType) * N);
        for (auto i = 0; i < N; i++) {
            new(Data(i)) RawType(std::move(inValue[i]));
        }
    }

    template <typename T, size_t N>
    void Any::ConstructFromArrayRef(std::reference_wrapper<T[N]> inRef)
    {
        using RawType = std::remove_cv_t<T>;

        arrayLength = N;
        policy = std::is_const_v<T> ? AnyPolicy::constRef : AnyPolicy::nonConstRef;
        rtti = &anyRttiImpl<RawType>;
        info = RefInfo(const_cast<RawType*>(&inRef.get()[0]), sizeof(RawType) * N);
    }

    template <typename T>
    T Argument::As() const // NOLINT
    {
        return Delegate([&](auto&& value) -> decltype(auto) {
            return value.template As<T>();
        });
    }

    template <Common::CppNotRef T>
    T* Argument::TryAs() const
    {
        return Delegate([&](auto&& value) -> decltype(auto) {
            return value.template TryAs<T>();
        });
    }

    template <ValidTemplateView V>
    bool Argument::CanAsTemplateView() const
    {
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.template CanAsTemplateView<V>();
        });
    }

    template <typename F>
    decltype(auto) Argument::Delegate(F&& inFunc) const
    {
        const auto index = any.index();
        if (index == 1) {
            return inFunc(*std::get<Any*>(any));
        }
        if (index == 2) {
            return inFunc(*std::get<const Any*>(any));
        }
        if (index == 3) {
            return inFunc(const_cast<Any&>(std::get<Any>(any)));
        }
        QuickFailWithReason("Argument is empty");
        return inFunc(Any {});
    }

    template <typename T>
    Any ForwardAsAny(T&& value)
    {
        Internal::StaticCheckArgumentType<T>();

        if constexpr (std::is_lvalue_reference_v<T&&>) {
            return { std::ref(std::forward<T>(value)) };
        } else {
            return { std::forward<T>(value) };
        }
    }

    template <typename T>
    Argument ForwardAsArg(T&& value)
    {
        Internal::StaticCheckArgumentType<T>();
        return ForwardAsAny(std::forward<T>(value));
    }

    template <typename... Args>
    ArgumentList ForwardAsArgList(Args&&... args)
    {
        ArgumentList result;
        result.reserve(sizeof...(args));
        (void) std::initializer_list<int> { ([&]() -> void {
            result.emplace_back(ForwardAsArg(std::forward<Args>(args)));
        }(), 0)... };
        return result;
    }

    template <typename T>
    Any ForwardAsAnyByValue(T&& value)
    {
        Internal::StaticCheckArgumentType<T>();
        auto any = ForwardAsAny(std::forward<T>(value));
        return any.IsMemoryHolder() ? any : any.Value();
    }

    template <typename T>
    Argument ForwardAsArgByValue(T&& value)
    {
        Internal::StaticCheckArgumentType<T>();
        return { ForwardAsAnyByValue(std::forward<T>(value)) };
    }

    template <typename ... Args>
    ArgumentList ForwardAsArgListByValue(Args&&... args)
    {
        ArgumentList result;
        result.reserve(sizeof...(args));
        (void) std::initializer_list<int> { ([&]() -> void {
            result.emplace_back(ForwardAsArgByValue(std::forward<Args>(args)));
        }(), 0)... };
        return result;
    }

    template <size_t N>
    Id::Id(const char(&inName)[N])
        : hash(Common::HashUtils::StrCrc32(inName))
        , name(inName)
    {
    }

    template <typename T>
    void Variable::Set(T&& value) const
    {
        SetDyn(ForwardAsArg(std::forward<T>(value)));
    }

    template <typename... Args>
    Any Function::Invoke(Args&&... args) const
    {
        return InvokeDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename... Args>
    Any Constructor::Construct(Args&&... args) const
    {
        return ConstructDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename... Args>
    Any Constructor::New(Args&&... args) const
    {
        return NewDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename ... Args>
    Any Constructor::InplaceNew(Args&&... args) const
    {
        return InplaceNewDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename C>
    void Destructor::Destruct(C&& object) const
    {
        DestructDyn(ForwardAsArg(std::forward<C>(object)));
    }

    template <typename C>
    void Destructor::Delete(C* object) const
    {
        DeleteDyn(ForwardAsArg(object));
    }

    template <typename C, typename T>
    void MemberVariable::Set(C&& object, T&& value) const
    {
        SetDyn(ForwardAsArg(std::forward<C>(object)), ForwardAsArg(std::forward<T>(value)));
    }

    template <typename C>
    Any MemberVariable::Get(C&& object) const
    {
        return GetDyn(ForwardAsArg(std::forward<C>(object)));
    }

    template <typename C, typename... Args>
    Any MemberFunction::Invoke(C&& object, Args&&... args) const
    {
        return InvokeDyn(ForwardAsArg(std::forward<C>(object)), ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <Common::CppClass C>
    bool Class::Has()
    {
        return Has(Mirror::GetTypeInfo<C>());
    }

    template <Common::CppClass C>
    const Class* Class::Find()
    {
        return Find(Mirror::GetTypeInfo<C>());
    }

    template <Common::CppClass C>
    const Class& Class::Get()
    {
        return Get(Mirror::GetTypeInfo<C>());
    }

    template <typename ... Args>
    Any Class::Construct(Args&&... args) const
    {
        return ConstructDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename ... Args>
    Any Class::New(Args&&... args) const
    {
        return NewDyn(ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename ... Args>
    Any Class::InplaceNew(void* ptr, Args&&... args) const
    {
        return InplaceNewDyn(ptr, ForwardAsArgList(std::forward<Args>(args)...));
    }

    template <typename C>
    void Class::Destruct(C&& object) const
    {
        DestructDyn(ForwardAsArg(std::forward<C>(object)));
    }

    template <typename C>
    void Class::Delete(C* object) const
    {
        DeleteDyn(ForwardAsArg(object));
    }

    template <Common::CppEnum T>
    const Enum* Enum::Find()
    {
        auto iter = typeToIdMap.find(Mirror::GetTypeInfo<T>()->id);
        if (iter == typeToIdMap.end()) {
            return nullptr;
        }
        return Find(iter->second);
    }

    template <Common::CppEnum T>
    const Enum& Enum::Get()
    {
        auto iter = typeToIdMap.find(Mirror::GetTypeInfo<T>()->id);
        Assert(iter != typeToIdMap.end());
        return Get(iter->second);
    }

    template <Common::CppEnum E>
    void EnumValue::Set(E& value) const
    {
        SetDyn(ForwardAsArg(value));
    }

    template <Common::CppEnum E>
    bool EnumValue::Compare(const E& value) const
    {
        return Compare(ForwardAsArg(value));
    }

    template <Common::CppEnum E>
    bool Enum::HasValue(E inValue) const
    {
        return HasValue(ForwardAsArg(inValue));
    }

    template <Common::CppEnum E>
    const EnumValue* Enum::FindValue(E inValue) const
    {
        return FindValue(ForwardAsArg(inValue));
    }

    template <Common::CppEnum E>
    const EnumValue& Enum::GetValue(E inValue) const
    {
        return GetValue(ForwardAsArg(inValue));
    }

    template <typename T>
    const TypeInfo* StdOptionalViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    void StdOptionalViewRtti::Reset(const Any& inRef)
    {
        inRef.As<std::optional<T>&>().reset();
    }

    template <typename T>
    Any StdOptionalViewRtti::Emplace(const Any& inRef, const Argument& inArg)
    {
        return std::ref(inRef.As<std::optional<T>&>().emplace(std::move(inArg.As<T&>())));
    }

    template <typename T>
    Any StdOptionalViewRtti::EmplaceDefault(const Any& inRef)
    {
        return std::ref(inRef.As<std::optional<T>&>().emplace());
    }

    template <typename T>
    bool StdOptionalViewRtti::HasValue(const Any& inRef)
    {
        return inRef.As<const std::optional<T>&>().has_value();
    }

    template <typename T>
    Any StdOptionalViewRtti::GetValue(const Any& inRef)
    {
        return std::ref(inRef.As<std::optional<T>&>().value());
    }

    template <typename T>
    Any StdOptionalViewRtti::GetConstValue(const Any& inRef)
    {
        return std::ref(inRef.As<const std::optional<T>&>().value());
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::optional<T>>::Id()
    {
        return StdOptionalViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<std::optional<T>>::Get()
    {
        return &stdOptionalViewRttiImpl<T>;
    }

    template <typename K, typename V>
    const TypeInfo* StdPairViewRtti::GetKeyType()
    {
        return GetTypeInfo<K>();
    }

    template <typename K, typename V>
    const TypeInfo* StdPairViewRtti::GetValueType()
    {
        return GetTypeInfo<V>();
    }

    template <typename K, typename V>
    Any StdPairViewRtti::GetKey(const Any& inRef)
    {
        return std::ref(inRef.As<std::pair<K, V>&>().first);
    }

    template <typename K, typename V>
    Any StdPairViewRtti::GetValue(const Any& inRef)
    {
        return std::ref(inRef.As<std::pair<K, V>&>().second);
    }

    template <typename K, typename V>
    Any StdPairViewRtti::GetConstKey(const Any& inRef)
    {
        return std::ref(inRef.As<const std::pair<K, V>&>().first);
    }

    template <typename K, typename V>
    Any StdPairViewRtti::GetConstValue(const Any& inRef)
    {
        return std::ref(inRef.As<const std::pair<K, V>&>().second);
    }

    template <typename K, typename V>
    void StdPairViewRtti::Reset(const Any& inRef)
    {
        inRef.As<std::pair<K, V>&>() = {};
    }

    template <typename K, typename V>
    constexpr TemplateViewId TemplateViewRttiGetter<std::pair<K, V>>::Id()
    {
        return StdPairViewRtti::id;
    }

    template <typename K, typename V>
    const void* TemplateViewRttiGetter<std::pair<K, V>>::Get()
    {
        return &stdPairViewRttiImpl<K, V>;
    }

    template <typename T, size_t N>
    const TypeInfo* StdArrayViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T, size_t N>
    size_t StdArrayViewRtti::GetSize()
    {
        return N;
    }

    template <typename T, size_t N>
    Any StdArrayViewRtti::GetElement(const Any& inRef, size_t inIndex)
    {
        return { std::ref(inRef.As<std::array<T, N>&>()[inIndex]) };
    }

    template <typename T, size_t N>
    Any StdArrayViewRtti::GetConstElement(const Any& inRef, size_t inIndex)
    {
        return { std::ref(inRef.As<const std::array<T, N>&>()[inIndex]) };
    }

    template <typename T, size_t N>
    constexpr TemplateViewId TemplateViewRttiGetter<std::array<T, N>>::Id()
    {
        return StdArrayViewRtti::id;
    }

    template <typename T, size_t N>
    const void* TemplateViewRttiGetter<std::array<T, N>>::Get()
    {
        return &stdArrayViewRttiImpl<T, N>;
    }

    template <typename T>
    const TypeInfo* StdVectorViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    size_t StdVectorViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::vector<T>&>().size();
    }

    template <typename T>
    void StdVectorViewRtti::Reserve(const Any& inRef, size_t inSize)
    {
        inRef.As<std::vector<T>&>().reserve(inSize);
    }

    template <typename T>
    void StdVectorViewRtti::Resize(const Any& inRef, size_t inSize)
    {
        inRef.As<std::vector<T>&>().resize(inSize);
    }

    template <typename T>
    void StdVectorViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::vector<T>&>().clear();
    }

    template <typename T>
    Any StdVectorViewRtti::GetElement(const Any& inRef, size_t inIndex)
    {
        if constexpr (std::is_same_v<T, bool>) {
            return inRef.As<std::vector<T>&>()[inIndex];
        } else {
            return std::ref(inRef.As<std::vector<T>&>()[inIndex]);
        }
    }

    template <typename T>
    Any StdVectorViewRtti::GetConstElement(const Any& inRef, size_t inIndex)
    {
        if constexpr (std::is_same_v<T, bool>) {
            return inRef.As<const std::vector<T>&>()[inIndex];
        } else {
            return std::ref(inRef.As<const std::vector<T>&>()[inIndex]);
        }
    }

    template <typename T>
    Any StdVectorViewRtti::EmplaceBack(const Any& inRef, const Argument& inTempObj)
    {
        if constexpr (std::is_same_v<T, bool>) {
            return inRef.As<std::vector<T>&>().emplace_back(std::move(inTempObj.As<T&>()));
        } else {
            return std::ref(inRef.As<std::vector<T>&>().emplace_back(std::move(inTempObj.As<T&>())));
        }
    }

    template <typename T>
    Any StdVectorViewRtti::EmplaceDefaultBack(const Any& inRef)
    {
        if constexpr (std::is_same_v<T, bool>) {
            return inRef.As<std::vector<T>&>().emplace_back();
        } else {
            return std::ref(inRef.As<std::vector<T>&>().emplace_back());
        }
    }

    template <typename T>
    void StdVectorViewRtti::Erase(const Any& inRef, size_t inIndex)
    {
        auto& vec = inRef.As<std::vector<T>&>();
        vec.erase(vec.begin() + inIndex);
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::vector<T>>::Id()
    {
        return StdVectorViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<std::vector<T>>::Get()
    {
        return &stdVectorViewRttiImpl<T>;
    }

    template <typename T>
    const TypeInfo* StdListViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    size_t StdListViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::list<T>&>().size();
    }

    template <typename T>
    void StdListViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::list<T>&>().clear();
    }

    template <typename T>
    void StdListViewRtti::Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        auto& list = inRef.As<std::list<T>&>(); // NOLINT
        for (auto& element : list) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    void StdListViewRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        const auto& list = inRef.As<const std::list<T>&>(); // NOLINT
        for (const auto& element : list) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    Any StdListViewRtti::EmplaceFront(const Any& inRef, const Argument& inTempObj)
    {
        T& elemRef = inRef.As<std::list<T>&>().emplace_front(std::move(inTempObj.As<T&>()));
        return { std::ref(elemRef) };
    }

    template <typename T>
    Any StdListViewRtti::EmplaceBack(const Any& inRef, const Argument& inTempObj)
    {
        T& elemRef = inRef.As<std::list<T>&>().emplace_back(std::move(inTempObj.As<T&>()));
        return { std::ref(elemRef) };
    }

    template <typename T>
    Any StdListViewRtti::EmplaceDefaultFront(const Any& inRef)
    {
        T& elemRef = inRef.As<std::list<T>&>().emplace_front();
        return { std::ref(elemRef) };
    }

    template <typename T>
    Any StdListViewRtti::EmplaceDefaultBack(const Any& inRef)
    {
        T& elemRef = inRef.As<std::list<T>&>().emplace_back();
        return { std::ref(elemRef) };
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::list<T>>::Id()
    {
        return StdListViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<std::list<T>>::Get()
    {
        return &stdListViewRttiImpl<T>;
    }

    template <typename T>
    const TypeInfo* StdUnorderedSetViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    Any StdUnorderedSetViewRtti::CreateElement()
    {
        return { T() };
    }

    template <typename T>
    size_t StdUnorderedSetViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::unordered_set<T>&>().size();
    }

    template <typename T>
    void StdUnorderedSetViewRtti::Reserve(const Any& inRef, size_t inSize)
    {
        inRef.As<std::unordered_set<T>&>().reserve(inSize);
    }

    template <typename T>
    void StdUnorderedSetViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::unordered_set<T>&>().clear();
    }

    template <typename T>
    void StdUnorderedSetViewRtti::Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        auto& set = inRef.As<std::unordered_set<T>&>(); // NOLINT
        for (auto& element : set) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    void StdUnorderedSetViewRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        const auto& set = inRef.As<const std::unordered_set<T>&>(); // NOLINT
        for (const auto& element : set) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    bool StdUnorderedSetViewRtti::Contains(const Any& inRef, const Argument& inElement)
    {
        return inRef.As<const std::unordered_set<T>&>().contains(inElement.As<const T&>());
    }

    template <typename T>
    void StdUnorderedSetViewRtti::Emplace(const Any& inRef, const Argument& inTempObj)
    {
        inRef.As<std::unordered_set<T>&>().emplace(std::move(inTempObj.As<T&>()));
    }

    template <typename T>
    void StdUnorderedSetViewRtti::Erase(const Any& inRef, const Argument& inElement)
    {
        inRef.As<std::unordered_set<T>&>().erase(inElement.As<const T&>());
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::unordered_set<T>>::Id()
    {
        return StdUnorderedSetViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<std::unordered_set<T>>::Get()
    {
        return &stdUnorderedSetViewRttiImpl<T>;
    }

    template <typename T>
    const TypeInfo* StdSetViewRtti::GetElementType()
    {
        return GetTypeInfo<T>();
    }

    template <typename T>
    Any StdSetViewRtti::CreateElement()
    {
        return { T() };
    }

    template <typename T>
    size_t StdSetViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::set<T>&>().size();
    }

    template <typename T>
    void StdSetViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::set<T>&>().clear();
    }

    template <typename T>
    void StdSetViewRtti::Traverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        auto& set = inRef.As<std::set<T>&>();
        for (auto& element : set) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    void StdSetViewRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&)>& inTraverser)
    {
        const auto& set = inRef.As<const std::set<T>&>();
        for (const auto& element : set) {
            inTraverser(std::ref(element));
        }
    }

    template <typename T>
    bool StdSetViewRtti::Contains(const Any& inRef, const Argument& inElement)
    {
        return inRef.As<const std::set<T>&>().contains(inElement.As<const T&>());
    }

    template <typename T>
    void StdSetViewRtti::Emplace(const Any& inRef, const Argument& inTempObj)
    {
        inRef.As<std::set<T>&>().emplace(inTempObj.As<const T&>());
    }

    template <typename T>
    void StdSetViewRtti::Erase(const Any& inRef, const Argument& inElement)
    {
        inRef.As<std::set<T>&>().erase(inElement.As<const T&>());
    }

    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::set<T>>::Id()
    {
        return StdSetViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<std::set<T>>::Get()
    {
        return &stdSetViewRttiImpl<T>;
    }

    template <typename K, typename V>
    const TypeInfo* StdUnorderedMapViewRtti::GetKeyType()
    {
        return GetTypeInfo<K>();
    }

    template <typename K, typename V>
    const TypeInfo* StdUnorderedMapViewRtti::GetValueType()
    {
        return GetTypeInfo<V>();
    }

    template <typename K, typename V>
    Any StdUnorderedMapViewRtti::CreateKey()
    {
        return { K() };
    }

    template <typename K, typename V>
    Any StdUnorderedMapViewRtti::CreateValue()
    {
        return { V() };
    }

    template <typename K, typename V>
    size_t StdUnorderedMapViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::unordered_map<K, V>&>().size();
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::Reserve(const Any& inRef, size_t inSize)
    {
        inRef.As<std::unordered_map<K, V>&>().reserve(inSize);
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::unordered_map<K, V>&>().clear();
    }

    template <typename K, typename V>
    Any StdUnorderedMapViewRtti::At(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<std::unordered_map<K, V>&>().at(inKey.As<const K&>())) };
    }

    template <typename K, typename V>
    Any StdUnorderedMapViewRtti::ConstAt(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<const std::unordered_map<K, V>&>().at(inKey.As<const K&>())) };
    }

    template <typename K, typename V>
    Any StdUnorderedMapViewRtti::GetOrAdd(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<std::unordered_map<K, V>&>()[inKey.As<const K&>()]) };
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::Traverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser)
    {
        auto& map = inRef.As<std::unordered_map<K, V>&>(); // NOLINT
        for (auto& pair : map) {
            inTraverser(std::ref(pair.first), std::ref(pair.second));
        }
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser)
    {
        const auto& map = inRef.As<const std::unordered_map<K, V>&>(); // NOLINT
        for (const auto& pair : map) {
            inTraverser(std::ref(pair.first), std::ref(pair.second));
        }
    }

    template <typename K, typename V>
    bool StdUnorderedMapViewRtti::Contains(const Any& inRef, const Argument& inKey)
    {
        return inRef.As<const std::unordered_map<K, V>&>().contains(inKey.As<const K&>());
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::Emplace(const Any& inRef, const Argument& inTempKey, const Argument& inTempValue)
    {
        inRef.As<std::unordered_map<K, V>&>().emplace(std::move(inTempKey.As<K&>()), std::move(inTempValue.As<V&>()));
    }

    template <typename K, typename V>
    void StdUnorderedMapViewRtti::Erase(const Any& inRef, const Argument& inKey)
    {
        inRef.As<std::unordered_map<K, V>&>().erase(inKey.As<const K&>());
    }

    template <typename K, typename V>
    constexpr TemplateViewId TemplateViewRttiGetter<std::unordered_map<K, V>>::Id()
    {
        return StdUnorderedMapViewRtti::id;
    }

    template <typename K, typename V>
    const void* TemplateViewRttiGetter<std::unordered_map<K, V>>::Get()
    {
        return &stdUnorderedMapViewRttiImpl<K, V>;
    }

    template <typename K, typename V>
    const TypeInfo* StdMapViewRtti::GetKeyType()
    {
        return GetTypeInfo<K>();
    }

    template <typename K, typename V>
    const TypeInfo* StdMapViewRtti::GetValueType()
    {
        return GetTypeInfo<V>();
    }

    template <typename K, typename V>
    Any StdMapViewRtti::CreateKey()
    {
        return { K() };
    }

    template <typename K, typename V>
    Any StdMapViewRtti::CreateValue()
    {
        return { V() };
    }

    template <typename K, typename V>
    size_t StdMapViewRtti::GetSize(const Any& inRef)
    {
        return inRef.As<const std::map<K, V>&>().size();
    }

    template <typename K, typename V>
    void StdMapViewRtti::Clear(const Any& inRef)
    {
        inRef.As<std::map<K, V>&>().clear();
    }

    template <typename K, typename V>
    Any StdMapViewRtti::At(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<std::map<K, V>&>().at(inKey.As<const K&>())) };
    }

    template <typename K, typename V>
    Any StdMapViewRtti::ConstAt(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<const std::map<K, V>&>().at(inKey.As<const K&>())) };
    }

    template <typename K, typename V>
    Any StdMapViewRtti::GetOrAdd(const Any& inRef, const Argument& inKey)
    {
        return { std::ref(inRef.As<std::map<K, V>&>()[inKey.As<const K&>()]) };
    }

    template <typename K, typename V>
    void StdMapViewRtti::Traverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser)
    {
        auto& map = inRef.As<std::map<K, V>&>(); // NOLINT
        for (auto& pair : map) {
            inTraverser(std::ref(pair.first), std::ref(pair.second));
        }
    }

    template <typename K, typename V>
    void StdMapViewRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&, const Any&)>& inTraverser)
    {
        const auto& map = inRef.As<std::map<K, V>&>(); // NOLINT
        for (const auto& pair : map) {
            inTraverser(std::ref(pair.first), std::ref(pair.second));
        }
    }

    template <typename K, typename V>
    bool StdMapViewRtti::Contains(const Any& inRef, const Argument& inKey)
    {
        return inRef.As<const std::map<K, V>&>().contains(inKey.As<const K&>());
    }

    template <typename K, typename V>
    void StdMapViewRtti::Emplace(const Any& inRef, const Argument& inTempKey, const Argument& inTempValue)
    {
        inRef.As<std::map<K, V>&>().emplace(std::move(inTempKey.As<K&>()), std::move(inTempValue.As<V&>()));
    }

    template <typename K, typename V>
    void StdMapViewRtti::Erase(const Any& inRef, const Argument& inKey)
    {
        inRef.As<std::map<K, V>&>().erase(inKey.As<const K&>());
    }

    template <typename K, typename V>
    constexpr TemplateViewId TemplateViewRttiGetter<std::map<K, V>>::Id()
    {
        return StdMapViewRtti::id;
    }

    template <typename K, typename V>
    const void* TemplateViewRttiGetter<std::map<K, V>>::Get()
    {
        return &stdMapViewRttiImpl<K, V>;
    }

    template <typename... T>
    size_t StdTupleRtti::GetSize()
    {
        return std::tuple_size_v<std::tuple<T...>>;
    }

    template <typename... T>
    const TypeInfo* StdTupleRtti::GetElementType(size_t inIndex)
    {
        static std::array<const TypeInfo*, sizeof...(T)> types = { GetTypeInfo<T>()... };
        Assert(inIndex < types.size());
        return types[inIndex];
    }

    template <typename... T>
    Any StdTupleRtti::CreateElement(size_t inIndex)
    {
        static std::array<std::function<Any()>, sizeof...(T)> creators = Internal::BuildTupleDynElementCreatorArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        Assert(inIndex < creators.size());
        return creators[inIndex]();
    }

    template <typename... T>
    Any StdTupleRtti::GetElement(const Any& inRef, size_t inIndex)
    {
        static std::array<std::function<Any(const Argument&)>, sizeof...(T)> getters = Internal::BuildTupleDynGetterArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        Assert(inIndex < getters.size());
        return getters[inIndex](inRef);
    }

    template <typename... T>
    Any StdTupleRtti::GetConstElement(const Any& inRef, size_t inIndex)
    {
        static std::array<std::function<Any(const Argument&)>, sizeof...(T)> getters = Internal::BuildTupleDynConstGetterArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        Assert(inIndex < getters.size());
        return getters[inIndex](inRef);
    }

    template <typename... T>
    void StdTupleRtti::Traverse(const Any& inRef, const std::function<void(const Any&, size_t)>& inVisitor)
    {
        Internal::TraverseTupleDyn<T...>(inRef, inVisitor, std::make_index_sequence<sizeof...(T)> {});
    }

    template <typename... T>
    void StdTupleRtti::ConstTraverse(const Any& inRef, const std::function<void(const Any&, size_t)>& inVisitor)
    {
        Internal::ConstTraverseTupleDyn<T...>(inRef, inVisitor, std::make_index_sequence<sizeof...(T)> {});
    }

    template <typename... T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::tuple<T...>>::Id()
    {
        return StdTupleRtti::id;
    }

    template <typename... T>
    const void* TemplateViewRttiGetter<std::tuple<T...>>::Get()
    {
        return &stdTupleRttiImpl<T...>;
    }

    template <typename... T>
    size_t StdVariantRtti::TypeNum()
    {
        return sizeof...(T);
    }

    template <typename... T>
    const TypeInfo* StdVariantRtti::TypeByIndex(size_t inIndex)
    {
        static std::array<const TypeInfo*, sizeof...(T)> types = { GetTypeInfo<T>()... };
        return types[inIndex];
    }

    template <typename... T>
    Any StdVariantRtti::CreateElement(size_t inIndex)
    {
        static std::array<std::function<Any()>, sizeof...(T)> creators = { []() -> Any { return T(); }... };
        return creators[inIndex]();
    }

    template <typename... T>
    size_t StdVariantRtti::Index(const Any& inRef)
    {
        return inRef.As<const std::variant<T...>&>().index();
    }

    template <typename... T>
    Any StdVariantRtti::GetElement(const Any& inRef, size_t inIndex)
    {
        static auto getters = Internal::BuildVariantDynGetterArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        return getters[inIndex](inRef);
    }

    template <typename... T>
    Any StdVariantRtti::GetConstElement(const Any& inRef, size_t inIndex)
    {
        static auto constGetters = Internal::BuildVariantDynConstGetterArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        return constGetters[inIndex](inRef);
    }

    template <typename... T>
    void StdVariantRtti::Visit(const Any& inRef, const std::function<void(const Any&)>& inVisitor)
    {
        std::visit([&]<typename T0>(T0&& inValue) -> void {
            inVisitor(std::ref(inValue));
        }, inRef.As<std::variant<T...>&>());
    }

    template <typename... T>
    void StdVariantRtti::ConstVisit(const Any& inRef, const std::function<void(const Any&)>& inVisitor)
    {
        std::visit([&]<typename T0>(T0&& inValue) -> void {
            inVisitor(std::ref(inValue));
        }, inRef.As<const std::variant<T...>&>());
    }

    template <typename... T>
    Any StdVariantRtti::Emplace(const Any& inRef, size_t inIndex, const Argument& inTempObj)
    {
        static auto emplaceFuncs = Internal::BuildVariantDynEmplaceFuncArray<T...>(std::make_index_sequence<sizeof...(T)> {});
        return emplaceFuncs[inIndex](inRef, inTempObj);
    }

    template <typename... T>
    constexpr TemplateViewId TemplateViewRttiGetter<std::variant<T...>>::Id()
    {
        return StdVariantRtti::id;
    }

    template <typename... T>
    const void* TemplateViewRttiGetter<std::variant<T...>>::Get()
    {
        return &stdVariantRttiImpl<T...>;
    }
} // namespace Mirror
