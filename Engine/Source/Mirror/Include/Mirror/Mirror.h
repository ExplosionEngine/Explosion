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
        const uint32_t isArray : 1;
        const uint32_t isArithmetic : 1;
        const uint32_t isIntegral : 1;
        const uint32_t isFloatingPoint : 1;
        const uint32_t copyConstructible : 1;
        const uint32_t copyAssignable : 1;
        const uint32_t moveConstructible : 1;
        const uint32_t moveAssignable : 1;
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
        ref,
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
    };

    template <typename T>
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
        &AnyRtti::Deref<T>
    };

    class MIRROR_API Any {
    public:
        Any();
        ~Any();
        Any(Any& inOther);
        Any(const Any& inOther);
        Any(const Any& inOther, AnyPolicy inPolicy);
        Any(Any&& inOther) noexcept;

        template <typename T> Any(T&& inValue); // NOLINT
        template <typename T> Any(std::reference_wrapper<T>& inRef); // NOLINT
        template <typename T> Any(std::reference_wrapper<T>&& inRef); // NOLINT
        template <typename T> Any(const std::reference_wrapper<T>& inRef); // NOLINT

        Any& operator=(const Any& inOther);
        Any& operator=(Any&& inOther) noexcept;

        template <typename T> Any& operator=(T&& inValue);
        template <typename T> Any& operator=(std::reference_wrapper<T>& inRef);
        template <typename T> Any& operator=(std::reference_wrapper<T>&& inRef);
        template <typename T> Any& operator=(const std::reference_wrapper<T>& inRef);

        template <typename T> bool Convertible();
        template <typename T> bool Convertible() const;

        template <typename T> T As();
        template <typename T> T As() const;
        template <typename T> T* TryAs();
        template <typename T> T* TryAs() const;

        // TODO array support

        Any Ref();
        Any Ref() const;
        Any ConstRef() const;
        Any AsValue() const;
        Any Ptr();
        Any Ptr() const;
        Any ConstPtr() const;
        Any Deref() const;

        AnyPolicy Policy() const;
        bool IsMemoryHolder() const;
        bool IsRef() const;
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

        // always return original ptr and size, even policy is ref
        void* Data() const;
        size_t Size() const;

        explicit operator bool() const;
        bool operator==(const Any& inAny) const;

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
            using StackMemory = Common::InplaceVector<uint8_t, MaxStackMemorySize>;
            using HeapMemory = std::vector<uint8_t>;
            static_assert(sizeof(StackMemory) == sizeof(HeapMemory));

            std::variant<StackMemory, HeapMemory> memory;
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

        void PerformCopyConstruct(const Any& inOther);
        void PerformCopyConstructWithPolicy(const Any& inOther, AnyPolicy inPolicy);
        void PerformMoveConstruct(Any&& inOther);

        AnyPolicy policy;
        const AnyRtti* rtti;
        std::variant<RefInfo, HolderInfo> info;
    };

    class Variable;
    class MemberVariable;

    class MIRROR_API Argument {
    public:
        Argument();
        Argument(Any& inAny); // NOLINT
        Argument(const Any& inAny); // NOLINT
        Argument(Any&& inAny); // NOLINT
        Argument& operator=(Any& inAny);
        Argument& operator=(const Any& inAny);
        Argument& operator=(Any&& inAny);

        template <typename T>
        T As() const;

        const TypeInfo* Type() const;
        const TypeInfo* RemoveRefType() const;
        const TypeInfo* AddPointerType() const;
        const TypeInfo* RemovePointerType() const;

    private:
        std::variant<std::monostate, Any*, const Any*, Any> any;
    };

    using ArgumentList = std::vector<Argument>;

    struct MIRROR_API Id {
        Id();
        template <size_t N> Id(const char (&inName)[N]); // NOLINT
        Id(std::string inName); // NOLINT

        bool operator==(const Id& inRhs) const;

        size_t hash;
        std::string name;
    };

    struct MIRROR_API IdHashProvider {
        size_t operator()(const Id& inId) const noexcept;
    };

    struct MIRROR_API NamePresets {
        static const Id globalScope;
        static const Id detor;
        static const Id defaultCtor;
    };

    class MIRROR_API Type {
    public:
        virtual ~Type();

        const std::string& GetName() const;
        const std::string& GetMeta(const std::string& key) const;
        std::string GetAllMeta() const;
        bool HasMeta(const std::string& key) const;
        bool GetMetaBool(const std::string& key) const;
        int32_t GetMetaInt32(const std::string& key) const;
        int64_t GetMetaInt64(const std::string& key) const;
        float GetMetaFloat(const std::string& key) const;

    protected:
        explicit Type(std::string inName);

    private:
        template <typename Derived> friend class MetaDataRegistry;

        std::string name;
        std::unordered_map<Id, std::string, IdHashProvider> metas;
    };

    class MIRROR_API Variable final : public Type {
    public:
        ~Variable() override;

        template <typename T> void Set(T&& value) const;
        Any Get() const;
        void Serialize(Common::SerializeStream& stream) const;
        void Deserialize(Common::DeserializeStream& stream) const;

        const TypeInfo* GetTypeInfo() const;
        void SetDyn(const Argument& inArgument) const;
        Any GetDyn() const;
        void SerializeDyn(Common::SerializeStream& stream) const;
        void DeserializeDyn(Common::DeserializeStream& stream) const;

    private:
        friend class GlobalRegistry;
        friend class GlobalScope;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(const Argument&)>;
        using Getter = std::function<Any()>;
        using VariableSerializer = std::function<void(Common::SerializeStream&, const Variable&)>;
        using VariableDeserializer = std::function<void(Common::DeserializeStream&, const Variable&)>;

        struct ConstructParams {
            std::string name;
            size_t memorySize;
            const TypeInfo* typeInfo;
            Setter setter;
            Getter getter;
            VariableSerializer serializer;
            VariableDeserializer deserializer;
        };

        explicit Variable(ConstructParams&& params);

        size_t memorySize;
        const TypeInfo* typeInfo;
        Setter setter;
        Getter getter;
        VariableSerializer serializer;
        VariableDeserializer deserializer;
    };

    class MIRROR_API Function final : public Type {
    public:
        ~Function() override;

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
            std::string name;
            uint8_t argsNum;
            const TypeInfo* retTypeInfo;
            std::vector<const TypeInfo*> argTypeInfos;
            Invoker invoker;
        };

        explicit Function(ConstructParams&& params);

        uint8_t argsNum;
        const TypeInfo* retTypeInfo;
        std::vector<const TypeInfo*> argTypeInfos;
        Invoker invoker;
    };

    class MIRROR_API Constructor final : public Type {
    public:
        ~Constructor() override;

        template <typename... Args> Any Construct(Args&&... args) const;
        template <typename... Args> Any New(Args&&... args) const;

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
            std::string name;
            uint8_t argsNum;
            std::vector<const TypeInfo*> argTypeInfos;
            std::vector<const TypeInfo*> argRemoveRefTypeInfos;
            std::vector<const TypeInfo*> argRemovePointerTypeInfos;
            Invoker stackConstructor;
            Invoker heapConstructor;
        };

        explicit Constructor(ConstructParams&& params);

        uint8_t argsNum;
        std::vector<const TypeInfo*> argTypeInfos;
        std::vector<const TypeInfo*> argRemoveRefTypeInfos;
        std::vector<const TypeInfo*> argRemovePointerTypeInfos;
        Invoker stackConstructor;
        Invoker heapConstructor;
    };

    class MIRROR_API Destructor final : public Type {
    public:
        ~Destructor() override;

        template <typename C> void Invoke(C&& object) const;
        void InvokeDyn(const Argument& argument) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<void(const Argument&)>;

        struct ConstructParams {
            Invoker destructor;
        };

        explicit Destructor(ConstructParams&& params);

        Invoker destructor;
    };

    class MIRROR_API MemberVariable final : public Type {
    public:
        ~MemberVariable() override;

        template <typename C, typename T> void Set(C&& object, T&& value) const;
        template <typename C> Any Get(C&& object) const;
        template <typename C> void Serialize(Common::SerializeStream& stream, C&& object) const;
        template <typename C> void Deserialize(Common::DeserializeStream& stream, C&& object) const;

        uint32_t SizeOf() const;
        const TypeInfo* GetTypeInfo() const;
        void SetDyn(const Argument& object, const Argument& value) const;
        Any GetDyn(const Argument& object) const;
        void SerializeDyn(Common::SerializeStream& stream, const Argument& object) const;
        void DeserializeDyn(Common::DeserializeStream& stream, const Argument& object) const;
        bool IsTransient() const;

    private:
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(const Argument&, const Argument&)>;
        using Getter = std::function<Any(const Argument&)>;
        using MemberVariableSerializer = std::function<void(Common::SerializeStream&, const MemberVariable&, const Argument&)>;
        using MemberVariableDeserializer = std::function<void(Common::DeserializeStream&, const MemberVariable&, const Argument&)>;

        struct ConstructParams {
            std::string name;
            uint32_t memorySize;
            const TypeInfo* typeInfo;
            Setter setter;
            Getter getter;
            MemberVariableSerializer serializer;
            MemberVariableDeserializer deserializer;
        };

        explicit MemberVariable(ConstructParams&& params);

        uint32_t memorySize;
        const TypeInfo* typeInfo;
        Setter setter;
        Getter getter;
        MemberVariableSerializer serializer;
        MemberVariableDeserializer deserializer;
    };

    class MIRROR_API MemberFunction final : public Type {
    public:
        ~MemberFunction() override;

        template <typename C, typename... Args> Any Invoke(C&& object, Args&&... args) const;

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
            std::string name;
            uint8_t argsNum;
            const TypeInfo* retTypeInfo;
            std::vector<const TypeInfo*> argTypeInfos;
            Invoker invoker;
        };

        explicit MemberFunction(ConstructParams&& params);

        uint8_t argsNum;
        const TypeInfo* retTypeInfo;
        std::vector<const TypeInfo*> argTypeInfos;
        Invoker invoker;
    };

    using VariableTraverser = std::function<void(const Variable&)>;
    using FunctionTraverser = std::function<void(const Function&)>;
    using MemberVariableTraverser = std::function<void(const MemberVariable&)>;
    using MemberFunctionTraverser = std::function<void(const MemberFunction&)>;

    class MIRROR_API GlobalScope final : public Type {
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

    class MIRROR_API Class final : public Type {
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
        template <typename T> void Serialize(Common::SerializeStream& stream, T&& obj) const;
        template <typename T> void Deserailize(Common::DeserializeStream& stream, T&& obj) const;

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
        const MemberFunction* FindMemberFunction(const Id& inId) const;
        const MemberFunction& GetMemberFunction(const Id& inId) const;

        Any ConstructDyn(const ArgumentList& arguments) const;
        Any NewDyn(const ArgumentList& arguments) const;
        void SerializeDyn(Common::SerializeStream& stream, const Argument& obj) const;
        void DeserailizeDyn(Common::DeserializeStream& stream, const Argument& obj) const;

    private:
        static std::unordered_map<TypeId, Id> typeToIdMap;

        friend class Registry;
        template <typename T> friend class ClassRegistry;

        using BaseClassGetter = std::function<const Class*()>;

        struct ConstructParams {
            std::string name;
            const TypeInfo* typeInfo;
            BaseClassGetter baseClassGetter;
            std::optional<std::function<Any()>> defaultObjectCreator;
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
        std::optional<Any> defaultObject;
        std::optional<Destructor> destructor;
        std::unordered_map<Id, Constructor, IdHashProvider> constructors;
        std::unordered_map<Id, Variable, IdHashProvider> staticVariables;
        std::unordered_map<Id, Function, IdHashProvider> staticFunctions;
        std::unordered_map<Id, MemberVariable, IdHashProvider> memberVariables;
        std::unordered_map<Id, MemberFunction, IdHashProvider> memberFunctions;
    };

    class MIRROR_API EnumElement final : public Type {
    public:
        ~EnumElement() override;

    private:
        friend class Registry;
        friend class Enum;
        template <typename T> friend class EnumRegistry;

        friend class Enum;

        Any Get() const;
        bool Compare(const Argument& argument) const;

        using Getter = std::function<Any()>;
        using Comparer = std::function<bool(const Argument&)>;

        struct ConstructParams {
            std::string name;
            Getter getter;
            Comparer comparer;
        };

        explicit EnumElement(ConstructParams&& inParams);

        Getter getter;
        Comparer comparer;
    };

    class MIRROR_API Enum final : public Type {
    public:
        template <Common::CppEnum T>
        static const Enum* Find();

        template <Common::CppEnum T>
        static const Enum& Get();

        static const Enum* Find(const Id& inId);
        static const Enum& Get(const Id& inId);

        ~Enum() override;

        const TypeInfo* GetTypeInfo() const;
        Any GetElement(const Id& inId) const;
        std::string GetElementName(const Argument& argument) const;

    private:
        static std::unordered_map<TypeId, Id> typeToIdMap;

        friend class Registry;
        template <typename T> friend class EnumRegistry;

        struct ConstructParams {
            std::string name;
            const TypeInfo* typeInfo;
        };

        explicit Enum(ConstructParams&& params);

        EnumElement& EmplaceElement(const Id& inId, EnumElement::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        std::unordered_map<Id, EnumElement, IdHashProvider> elements;
    };
}

namespace Mirror::Internal {
    template <typename T>
    void StaticCheckArgumentType()
    {
        using RawType = std::remove_cvref_t<T>;
        static_assert(
            !std::is_same_v<RawType, Any> && !std::is_same_v<RawType, Any*> && !std::is_same_v<RawType, const Any*>,
            "static version reflection method do no support use Any/Any*/const Any* as argument, please check you arguments");
    }

    template <typename T>
    Argument ForwardAsArgument(T&& value)
    {
        StaticCheckArgumentType<T>();

        if constexpr (std::is_lvalue_reference_v<T&&>) {
            return { Any(std::ref(std::forward<T>(value))) };
        } else {
            return { Any(std::forward<T>(value)) };
        }
    }

    template <typename... Args>
    ArgumentList ForwardAsArgumentList(Args&&... args)
    {
        ArgumentList result;
        result.reserve(sizeof...(args));
        (void) std::initializer_list<int> { ([&]() -> void {
            StaticCheckArgumentType<Args>();

            if constexpr (std::is_lvalue_reference_v<Args&&>) {
                result.emplace_back(Any(std::ref(std::forward<Args>(args))));
            } else {
                result.emplace_back(Any(std::forward<Args>(args)));
            }
        }(), 0)... };
        return result;
    }
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
            std::is_const_v<T>,
            std::is_lvalue_reference_v<T>,
            std::is_lvalue_reference_v<T> ? std::is_const_v<std::remove_reference_t<T>> : false,
            std::is_rvalue_reference_v<T>,
            std::is_pointer_v<T>,
            std::is_pointer_v<T> ? std::is_const_v<std::remove_pointer_t<T>> : false,
            std::is_class_v<T>,
            std::is_array_v<T>,
            std::is_arithmetic_v<T>,
            std::is_integral_v<T>,
            std::is_floating_point_v<T>,
            std::is_copy_constructible_v<T>,
            std::is_copy_assignable_v<T>,
            std::is_move_constructible_v<T>,
            std::is_move_assignable_v<T>,
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
            QuickFailWithReason(fmt::format("type {} is no support copy construct", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    void AnyRtti::MoveConstruct(void* inThis, void* inOther) noexcept
    {
        if constexpr (std::is_move_constructible_v<T>) {
            new(inThis) T(std::move(*static_cast<T*>(inOther)));
        } else {
            QuickFailWithReason(fmt::format("type {} is no support move construct", GetTypeInfo<T>()->name));
        }
    }

    template <typename T>
    bool AnyRtti::Equal(const void* inThis, const void* inOther) noexcept
    {
        if constexpr (std::equality_comparable<T>) {
            return *static_cast<const T*>(inThis) == *static_cast<const T*>(inOther);
        } else {
            QuickFailWithReason(fmt::format("type {} is no support equal compare", GetTypeInfo<T>()->name));
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

    template <typename T>
    bool Any::Convertible()
    {
        return Mirror::Convertible(
            { Type(), RemoveRefType(), RemovePointerType() },
            { GetTypeInfo<T>(), GetTypeInfo<std::remove_reference_t<T>>(), GetTypeInfo<std::remove_pointer_t<T>>() });
    }

    template <typename T>
    bool Any::Convertible() const
    {
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

    template <typename T>
    T* Any::TryAs()
    {
        Assert(!std::is_reference_v<T>);
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() });
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <typename T>
    T* Any::TryAs() const
    {
        Assert(!std::is_reference_v<T>);
        const bool convertible = Mirror::Convertible(
            { AddPointerType(), AddPointerType(), RemoveRefType() },
            { GetTypeInfo<T*>(), GetTypeInfo<T*>(), GetTypeInfo<T>() });
        return convertible ? static_cast<std::remove_cvref_t<T>*>(Data()) : nullptr;
    }

    template <typename T>
    void Any::ConstructFromValue(T&& inValue)
    {
        using RawType = std::remove_cvref_t<T>;

        policy = AnyPolicy::memoryHolder;
        rtti = &anyRttiImpl<RawType>;
        info = HolderInfo(sizeof(RawType));
        new(Data()) RawType(std::forward<T>(inValue));
    }

    template <typename T>
    void Any::ConstructFromRef(std::reference_wrapper<T> inRef)
    {
        using RawType = std::remove_const_t<T>;

        policy = std::is_const_v<T> ? AnyPolicy::constRef : AnyPolicy::ref;
        rtti = &anyRttiImpl<RawType>;
        info = RefInfo(const_cast<RawType*>(&inRef.get()), sizeof(RawType));
    }

    template <typename T>
    T Argument::As() const // NOLINT
    {
        const auto index = any.index();
        if (index == 1) {
            return std::get<Any*>(any)->As<T>();
        }
        if (index == 2) {
            return std::get<const Any*>(any)->As<T>();
        }
        if (index == 3) {
            return const_cast<Any&>(std::get<Any>(any)).As<T>();
        }
        QuickFailWithReason("Argument is empty");
        return std::get<Any*>(any)->As<T>();
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

    template <typename C>
    void MemberVariable::Serialize(Common::SerializeStream& stream, C&& object) const
    {
        SerializeDyn(stream, Internal::ForwardAsArgument(std::forward<C>(object)));
    }

    template <typename C>
    void MemberVariable::Deserialize(Common::DeserializeStream& stream, C&& object) const
    {
        DeserializeDyn(stream, Internal::ForwardAsArgument(std::forward<C>(object)));
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

    template <typename T>
    void Class::Serialize(Common::SerializeStream& stream, T&& obj) const
    {
        SerializeDyn(stream, Internal::ForwardAsArgument(std::forward<T>(obj)));
    }

    template <typename T>
    void Class::Deserailize(Common::DeserializeStream& stream, T&& obj) const
    {
        DeserailizeDyn(stream, Internal::ForwardAsArgument(std::forward<T>(obj)));
    }

    template <Common::CppEnum T>
    const Enum* Enum::Find()
    {
        auto iter = typeToIdMap.find(Mirror::GetTypeInfo<T>()->id);
        Assert(iter != typeToIdMap.end());
        return Find(iter->second);
    }

    template <Common::CppEnum T>
    const Enum& Enum::Get()
    {
        auto iter = typeToIdMap.find(Mirror::GetTypeInfo<T>()->id);
        Assert(iter != typeToIdMap.end());
        return Get(iter->second);
    }
}
