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

    struct TypeInfoCompact {
        const TypeInfo* raw;
        const TypeInfo* removeRef;
        const TypeInfo* removePointer;
    };

    MIRROR_API bool PointerConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType);
    MIRROR_API bool PolymorphismConvertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType);
    MIRROR_API bool Convertible(const TypeInfoCompact& inSrcType, const TypeInfoCompact& inDstType);

    enum class AnyPolicy : uint8_t {
        memoryHolder,
        nonConstRef,
        constRef,
        max
    };

    class Any;

    struct AnyRtti {
        using DetorFunc = void(void*) noexcept;
        using CopyConstructFunc = void(void*, const void*);
        using MoveConstructFunc = void(void*, void*) noexcept;
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

        template <typename T> static void Detor(void* inThis) noexcept;
        template <typename T> static void CopyConstruct(void* inThis, const void* inOther);
        template <typename T> static void MoveConstruct(void* inThis, void* inOther) noexcept;
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

        DetorFunc* detor;
        CopyConstructFunc* copyConstruct;
        MoveConstructFunc* moveConstruct;
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
    };

    template <typename T, size_t N = 1>
    static constexpr AnyRtti anyRttiImpl = {
        &AnyRtti::Detor<T>,
        &AnyRtti::CopyConstruct<T>,
        &AnyRtti::MoveConstruct<T>,
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
        &AnyRtti::ToString<T>
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

        template <typename T> bool Convertible();
        template <typename T> bool Convertible() const;

        template <typename T> T As();
        template <typename T> T As() const;
        template <Common::CppNotRef T> T* TryAs();
        template <Common::CppNotRef T> T* TryAs() const;
        template <typename B, typename T> T PolyAs();
        template <typename B, typename T> T PolyAs() const;

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
        void PerformMoveConstruct(Any&& inOther);
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

        bool IsMemoryHolder() const;
        bool IsRef() const;
        bool IsNonConstRef() const;
        bool IsConstRef() const;
        const TypeInfo* Type() const;
        const TypeInfo* RemoveRefType() const;
        const TypeInfo* AddPointerType() const;
        const TypeInfo* RemovePointerType() const;

    private:
        template <typename F> decltype(auto) Delegate(F&& inFunc) const;

        std::variant<std::monostate, Any*, const Any*, Any> any;
    };

    using ArgumentList = std::vector<Argument>;

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
    };

    class MIRROR_API ReflNode {
    public:
        virtual ~ReflNode();

        const Id& GetId() const;
        const std::string& GetName() const;
        const std::string& GetMeta(const std::string& key) const;
        std::string GetAllMeta() const;
        bool HasMeta(const std::string& key) const;
        bool GetMetaBool(const std::string& key) const;
        int32_t GetMetaInt32(const std::string& key) const;
        int64_t GetMetaInt64(const std::string& key) const;
        float GetMetaFloat(const std::string& key) const;

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

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(const ArgumentList&)>;

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
    };

    class MIRROR_API Destructor final : public ReflNode {
    public:
        ~Destructor() override;

        template <typename C> void Invoke(C&& object) const;

        const std::string& GetOwnerName() const;
        const Id& GetOwnerId() const;
        const Class& GetOwner() const;
        FieldAccess GetAccess() const;
        void InvokeDyn(const Argument& argument) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<void(const Argument&)>;

        struct ConstructParams {
            Id owner;
            FieldAccess access;
            Invoker destructor;
        };

        explicit Destructor(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        Invoker destructor;
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
        uint32_t SizeOf() const;
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
            uint32_t memorySize;
            const TypeInfo* typeInfo;
            Setter setter;
            Getter getter;
        };

        explicit MemberVariable(ConstructParams&& params);

        Id owner;
        FieldAccess access;
        uint32_t memorySize;
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

        std::unordered_map<Id, Variable, IdHashProvider> variables;
        std::unordered_map<Id, Function, IdHashProvider> functions;
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

        template <typename... Args> Any Construct(Args&&... args);
        template <typename... Args> Any New(Args&&... args);

        void ForEachStaticVariable(const VariableTraverser& func) const;
        void ForEachStaticFunction(const FunctionTraverser& func) const;
        void ForEachMemberVariable(const MemberVariableTraverser& func) const;
        void ForEachMemberFunction(const MemberFunctionTraverser& func) const;
        const TypeInfo* GetTypeInfo() const;
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

        Any ConstructDyn(const ArgumentList& arguments) const;
        Any NewDyn(const ArgumentList& arguments) const;

    private:
        static std::unordered_map<TypeId, Id> typeToIdMap;

        friend class Registry;
        template <typename T> friend class ClassRegistry;

        using BaseClassGetter = std::function<const Class*()>;

        struct ConstructParams {
            Id id;
            const TypeInfo* typeInfo;
            BaseClassGetter baseClassGetter;
            std::function<Any()> defaultObjectCreator;
            std::optional<Destructor::ConstructParams> destructorParams;
            std::optional<Constructor::ConstructParams> defaultConstructorParams;
        };

        explicit Class(ConstructParams&& params);

        void CreateDefaultObject(const std::function<Any()>& inCreator);
        Destructor& EmplaceDestructor(Destructor::ConstructParams&& inParams);
        Constructor& EmplaceConstructor(const Id& inId, Constructor::ConstructParams&& inParams);
        Variable& EmplaceStaticVariable(const Id& inId, Variable::ConstructParams&& inParams);
        Function& EmplaceStaticFunction(const Id& inId, Function::ConstructParams&& inParams);
        MemberVariable& EmplaceMemberVariable(const Id& inId, MemberVariable::ConstructParams&& inParams);
        MemberFunction& EmplaceMemberFunction(const Id& inId, MemberFunction::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        BaseClassGetter baseClassGetter;
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

namespace Mirror::Internal {
    template <typename T>
    void StaticCheckArgumentType()
    {
        using RawType = std::remove_cvref_t<T>;
        static_assert(
            !std::is_same_v<RawType, Any> && !std::is_same_v<RawType, Any*> && !std::is_same_v<RawType, const Any*> && !std::is_same_v<RawType, Argument>,
            "static version reflection method do no support use Any/Any*/const Any*/Argument as argument, please check you arguments");
    }

    template <typename T>
    Any ForwardAsAny(T&& value)
    {
        StaticCheckArgumentType<T>();

        if constexpr (std::is_lvalue_reference_v<T&&>) {
            return { std::ref(std::forward<T>(value)) };
        } else {
            return { std::forward<T>(value) };
        }
    }

    template <typename T>
    Argument ForwardAsArgument(T&& value)
    {
        StaticCheckArgumentType<T>();
        return ForwardAsAny(std::forward<T>(value));
    }

    template <typename... Args>
    ArgumentList ForwardAsArgumentList(Args&&... args)
    {
        ArgumentList result;
        result.reserve(sizeof...(args));
        (void) std::initializer_list<int> { ([&]() -> void {
            result.emplace_back(ForwardAsArgument(std::forward<Args>(args)));
        }(), 0)... };
        return result;
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
            return SerializeDyn(stream, Mirror::Class::Get<T>(), Mirror::Internal::ForwardAsArgument(value));
        }

        static size_t Deserialize(BinaryDeserializeStream& stream, T& value)
        {
            return DeserializeDyn(stream, Mirror::Class::Get<T>(), Mirror::Internal::ForwardAsArgument(value));
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
                std::underlying_type_t<E> unlderlyingValue;
                deserialized += Serializer<std::underlying_type_t<E>>::Deserialize(stream, unlderlyingValue);
                value = static_cast<E>(unlderlyingValue);
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
                JsonSerializeDyn(outJsonValue, inAllocator, *baseClass, inObj);
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

            if (baseClass != nullptr) {
                JsonDeserializeDyn(inJsonValue, *baseClass, outObj);
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
            JsonSerializeDyn(outValue, inAllocator, Mirror::Class::Get<T>(), Mirror::Internal::ForwardAsArgument(inValue));
        }

        static void JsonDeserialize(const rapidjson::Value& inValue, T& outValue)
        {
            JsonDeserializeDyn(inValue, Mirror::Class::Get<T>(), Mirror::Internal::ForwardAsArgument(outValue));
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

            std::stringstream stream;
            stream << "{ ";
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
            return ToStringDyn(Mirror::Class::Get<T>(), Mirror::Internal::ForwardAsArgument(inValue));
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
            { GetTypeInfo<T>(), GetTypeInfo<std::remove_reference_t<T>>(), GetTypeInfo<std::remove_pointer_t<T>>() });
    }

    template <typename T>
    bool Any::Convertible() const
    {
        Assert(!IsArray());
        return Mirror::Convertible(
            { Type(), RemoveRefType(), RemovePointerType() },
            { GetTypeInfo<T>(), GetTypeInfo<std::remove_reference_t<T>>(), GetTypeInfo<std::remove_pointer_t<T>>() });
    }

    template <typename T>
    T Any::As()
    {
        Assert(Convertible<T>());
        return *static_cast<std::remove_cvref_t<T>*>(Data());
    }

    template <typename T>
    T Any::As() const
    {
        Assert(Convertible<T>());
        return *static_cast<std::remove_cvref_t<T>*>(Data());
    }

    template <Common::CppNotRef T>
    T* Any::TryAs()
    {
        Assert(!IsArray());
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() });
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <Common::CppNotRef T>
    T* Any::TryAs() const
    {
        Assert(!IsArray());
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() });
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <typename B, typename T>
    T Any::PolyAs()
    {
        Assert(!IsArray());
        return dynamic_cast<T>(As<B>());
    }

    template <typename B, typename T>
    T Any::PolyAs() const
    {
        Assert(!IsArray());
        return dynamic_cast<T>(As<B>());
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
        info = RefInfo(const_cast<RawType*>(&inRef.get()), sizeof(RawType));
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
        return Delegate([](auto&& value) -> decltype(auto) {
            return value.template As<T>();
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

    template <size_t N>
    Id::Id(const char(&inName)[N])
        : hash(Common::HashUtils::StrCrc32(inName))
        , name(inName)
    {
    }

    template <typename T>
    void Variable::Set(T&& value) const
    {
        SetDyn(Internal::ForwardAsArgument(std::forward<T>(value)));
    }

    template <typename... Args>
    Any Function::Invoke(Args&&... args) const
    {
        return InvokeDyn(Internal::ForwardAsArgumentList(std::forward<Args>(args)...));
    }

    template <typename... Args>
    Any Constructor::Construct(Args&&... args) const
    {
        return ConstructDyn(Internal::ForwardAsArgumentList(std::forward<Args>(args)...));
    }

    template <typename... Args>
    Any Constructor::New(Args&&... args) const
    {
        return NewDyn(Internal::ForwardAsArgumentList(std::forward<Args>(args)...));
    }

    template <typename C>
    void Destructor::Invoke(C&& object) const
    {
        InvokeDyn(Internal::ForwardAsArgument(std::forward<C>(object)));
    }

    template <typename C, typename T>
    void MemberVariable::Set(C&& object, T&& value) const
    {
        SetDyn(Internal::ForwardAsArgument(std::forward<C>(object)), Internal::ForwardAsArgument(std::forward<T>(value)));
    }

    template <typename C>
    Any MemberVariable::Get(C&& object) const
    {
        return GetDyn(Internal::ForwardAsArgument(std::forward<C>(object)));
    }

    template <typename C, typename... Args>
    Any MemberFunction::Invoke(C&& object, Args&&... args) const
    {
        return InvokeDyn(Internal::ForwardAsArgument(std::forward<C>(object)), Internal::ForwardAsArgumentList(std::forward<Args>(args)...));
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
    Any Class::Construct(Args&&... args)
    {
        auto arguments = Internal::ForwardAsArgumentList(std::forward<Args>(args)...);
        const auto* constructor = FindSuitableConstructor(arguments);
        Assert(constructor != nullptr);
        return constructor->Construct(arguments);
    }

    template <typename ... Args>
    Any Class::New(Args&&... args)
    {
        auto arguments = Internal::ForwardAsArgumentList(std::forward<Args>(args)...);
        const auto* constructor = FindSuitableConstructor(arguments);
        Assert(constructor != nullptr);
        return constructor->New(arguments);
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
        SetDyn(Internal::ForwardAsArgument(value));
    }

    template <Common::CppEnum E>
    bool EnumValue::Compare(const E& value) const
    {
        return Compare(Internal::ForwardAsArgument(value));
    }

    template <Common::CppEnum E>
    bool Enum::HasValue(E inValue) const
    {
        return HasValue(Internal::ForwardAsArgument(inValue));
    }

    template <Common::CppEnum E>
    const EnumValue* Enum::FindValue(E inValue) const
    {
        return FindValue(Internal::ForwardAsArgument(inValue));
    }

    template <Common::CppEnum E>
    const EnumValue& Enum::GetValue(E inValue) const
    {
        return GetValue(Internal::ForwardAsArgument(inValue));
    }
}
