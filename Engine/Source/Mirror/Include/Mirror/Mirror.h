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
    class Variable;
    class MemberVariable;

    struct TypeInfo {
#if BUILD_CONFIG_DEBUG
        // NOTICE: this name is platform relative, so do not use it unless for debug
        std::string debugName;
#endif
        std::string name;
        TypeId id;
        TypeId removePointerType;
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

    template <typename T>
    TypeInfo* GetTypeInfo();

    struct NamePresets {
        static constexpr auto globalScope = "_globalScope";
        static constexpr auto destructor = "_destructor";
        static constexpr auto defaultConstructor = "_defaultConstructor";
    };

    enum class AnyPolicy : uint8_t {
        memoryHolder,
        ref,
        constRef,
        max
    };

    struct AnyRtti {
        using DetorFunc = void(void*) noexcept;
        using CopyConstructFunc = void(void*, const void*);
        using MoveConstructFunc = void(void*, void*) noexcept;
        using EqualFunc = bool(const void*, const void*);

        template <typename T> static void Detor(void* inThis) noexcept;
        template <typename T> static void CopyConstruct(void* inThis, const void* inOther);
        template <typename T> static void MoveConstruct(void* inThis, void* inOther) noexcept;
        template <typename T> static bool Equal(const void* inThis, const void* inOther) noexcept;

        DetorFunc* detor;
        CopyConstructFunc* copyConstruct;
        MoveConstructFunc* moveConstruct;
        EqualFunc* equal;
    };

    template <typename T>
    static constexpr AnyRtti anyRttiImpl = {
        &AnyRtti::Detor<T>,
        &AnyRtti::CopyConstruct<T>,
        &AnyRtti::MoveConstruct<T>,
        &AnyRtti::Equal<T>
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
        template <typename T> Any& operator=(const std::reference_wrapper<T>& inRef);

        template <typename T> bool Convertible();
        template <typename T> bool Convertible() const;
        bool Convertible(const TypeInfo* inType);
        bool Convertible(const TypeInfo* inType) const;

        template <typename T> T As();
        template <typename T> T As() const;
        template <typename T> T* TryAs();
        template <typename T> T* TryAs() const;

        Any GetRef();
        Any GetRef() const;
        Any GetConstRef() const;
        Any AsValue() const;

        AnyPolicy Policy() const;
        bool IsMemoryHolder() const;
        bool IsRef() const;
        bool IsConstRef() const;
        void Reset();
        bool Empty() const;

        // always return original ptr and size, even policy is ref
        void* Ptr() const;
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
            static constexpr size_t MaxStackMemorySize = sizeof(std::vector<uint8_t>);
            using StackMemory = std::array<uint8_t, MaxStackMemorySize>;
            using HeapMemory = std::vector<uint8_t>;

            std::variant<StackMemory, HeapMemory> memory;
            size_t memorySize;
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
        TypeId typeId;
        std::variant<RefInfo, HolderInfo> info;
    };

    // TODO find by compile time id
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
        std::unordered_map<std::string, std::string> metas;
    };

    class MIRROR_API Variable final : public Type {
    public:
        ~Variable() override;

        template <typename T>
        void Set(T value) const;

        const TypeInfo* GetTypeInfo() const;
        void Set(Any* value) const;
        Any Get() const;
        void Serialize(Common::SerializeStream& stream) const;
        void Deserialize(Common::DeserializeStream& stream) const;

    private:
        friend class GlobalRegistry;
        friend class GlobalScope;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(Any*)>;
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

        // args must passed by std::ref()
        template <typename... Args>
        Any Invoke(Args&&... args) const;

        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        // TODO replace with Arguments
        Any InvokeWith(Any* arguments, uint8_t argumentsSize) const;

    private:
        friend class GlobalRegistry;
        friend class GlobalScope;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(Any*, uint8_t)>;

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

        // args must passed by std::ref()
        template <typename... Args>
        Any ConstructOnStack(Args&&... args) const;

        // args must passed by std::ref()
        template <typename... Args>
        Any NewObject(Args&&... args) const;

        uint8_t GetArgsNum() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;

        // TODO replace with arguments
        Any ConstructOnStackWith(Any* arguments, uint8_t argumentsSize) const;
        Any NewObjectWith(Any* arguments, uint8_t argumentsSize) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(Any*, uint8_t)>;

        struct ConstructParams {
            std::string name;
            uint8_t argsNum;
            std::vector<const TypeInfo*> argTypeInfos;
            Invoker stackConstructor;
            Invoker heapConstructor;
        };

        explicit Constructor(ConstructParams&& params);

        uint8_t argsNum;
        std::vector<const TypeInfo*> argTypeInfos;
        Invoker stackConstructor;
        Invoker heapConstructor;
    };

    class MIRROR_API Destructor final : public Type {
    public:
        ~Destructor() override;

        template <typename C>
        void Invoke(C&& object) const;

        void InvokeWith(Any* object) const;

    private:
        friend class Registry;
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<void(Any*)>;

        struct ConstructParams {
            Invoker destructor;
        };

        explicit Destructor(ConstructParams&& params);

        Invoker destructor;
    };

    class MIRROR_API MemberVariable final : public Type {
    public:
        ~MemberVariable() override;

        template <typename C, typename T>
        void Set(C&& object, T value) const;

        uint32_t SizeOf() const;
        const TypeInfo* GetTypeInfo() const;
        void Set(Any* object, Any* value) const;
        Any Get(Any* object) const;
        // TODO const getter
        void Serialize(Common::SerializeStream& stream, Any* object) const;
        void Deserialize(Common::DeserializeStream& stream, Any* object) const;
        bool IsTransient() const;

    private:
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(Any*, Any*)>;
        // TODO const getter
        using Getter = std::function<Any(Any*)>;
        using MemberVariableSerializer = std::function<void(Common::SerializeStream&, const MemberVariable&, Any*)>;
        using MemberVariableDeserializer = std::function<void(Common::DeserializeStream&, const MemberVariable&, Any*)>;

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

        // args must passed by std::ref()
        template <typename C, typename... Args>
        Any Invoke(C&& object, Args&&... args) const;

        // TODO invoke const

        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        // TODO replace with arguments
        Any InvokeWith(Any* object, Any* args, size_t argsSize) const;

    private:
        friend class Class;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(Any*, Any*, size_t)>;

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

    class MIRROR_API GlobalScope final : public Type {
    public:
        ~GlobalScope() override;

        static const GlobalScope& Get();

        template <typename F>
        void ForEachVariable(F&& func) const;

        template <typename F>
        void ForEachFunction(F&& func) const;

        bool HasVariable(const std::string& name) const;
        const Variable* FindVariable(const std::string& name) const;
        const Variable& GetVariable(const std::string& name) const;
        bool HasFunction(const std::string& name) const;
        const Function* FindFunction(const std::string& name) const;
        const Function& GetFunction(const std::string& name) const;

    private:
        friend class Registry;
        friend class GlobalRegistry;

        Variable& EmplaceVariable(const std::string& inName, Variable::ConstructParams&& inParams);
        Function& EmplaceFunction(const std::string& inName, Function::ConstructParams&& inParams);

        GlobalScope();

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Function> functions;
    };

    class MIRROR_API Class final : public Type {
    public:
        ~Class() override;

        template <typename C>
        requires std::is_class_v<C>
        static bool Has();

        template <typename C>
        requires std::is_class_v<C>
        static const Class* Find();

        template <typename C>
        requires std::is_class_v<C>
        static const Class& Get();

        template <typename F>
        void ForEachStaticVariable(F&& func) const;

        template <typename F>
        void ForEachStaticFunction(F&& func) const;

        template <typename F>
        void ForEachMemberVariable(F&& func) const;

        template <typename F>
        void ForEachMemberFunction(F&& func) const;

        template <typename... Args>
        Any ConstructOnStack(Args&&... args)
        {
            std::array<Any, sizeof...(args)> refs = { Any(std::forward<Args>(args))... };
            const auto* constructor = FindSuitableConstructor(refs);
            Assert(constructor != nullptr);
            return constructor->ConstructOnStack(refs);
        }

        template <typename... Args>
        Any NewObject(Args&&... args)
        {
            std::array<Any, sizeof...(args)> refs = { Any(std::forward<Args>(args))... };
            const auto* constructor = FindSuitableConstructor(refs);
            Assert(constructor != nullptr);
            return constructor->NewObject(refs);
        }

        template <typename F>
        static void ForEachClass(F&& func);

        static bool Has(const std::string& name);
        static const Class* Find(const std::string& name);
        static const Class& Get(const std::string& name);
        static bool Has(const TypeInfo* typeInfo);
        static const Class* Find(const TypeInfo* typeInfo);
        static const Class& Get(const TypeInfo* typeInfo);
        static const Class* Find(TypeId typeId);
        static const Class& Get(TypeId typeId);
        static std::vector<const Class*> GetAll();
        static std::vector<const Class*> FindWithCategory(const std::string& category);

        const TypeInfo* GetTypeInfo() const;
        // TODO GetPointerTypeInfo() / GetConstPointerTypeInfo()
        // TODO DynamicCast(Any/const Any)
        bool HasDefaultConstructor() const;
        const Class* GetBaseClass() const;
        bool IsBaseOf(const Class* derivedClass) const;
        bool IsDerivedFrom(const Class* baseClass) const;
        const Constructor* FindDefaultConstructor() const;
        const Constructor& GetDefaultConstructor() const;
        bool HasDestructor() const;
        const Destructor* FindDestructor() const;
        const Destructor& GetDestructor() const;
        bool HasConstructor(const std::string& name) const;
        const Constructor* FindSuitableConstructor(const Any* args, uint8_t argNum) const;
        Any ConstructOnStackSuitable(Any* args, uint8_t argNum) const;
        Any NewObjectSuitable(Any* args, uint8_t argNum) const;
        const Constructor* FindConstructor(const std::string& name) const;
        const Constructor& GetConstructor(const std::string& name) const;
        bool HasStaticVariable(const std::string& name) const;
        const Variable* FindStaticVariable(const std::string& name) const;
        const Variable& GetStaticVariable(const std::string& name) const;
        bool HasStaticFunction(const std::string& name) const;
        const Function* FindStaticFunction(const std::string& name) const;
        const Function& GetStaticFunction(const std::string& name) const;
        bool HasMemberVariable(const std::string& name) const;
        const MemberVariable* FindMemberVariable(const std::string& name) const;
        const MemberVariable& GetMemberVariable(const std::string& name) const;
        bool HasMemberFunction(const std::string& name) const;
        const MemberFunction* FindMemberFunction(const std::string& name) const;
        const MemberFunction& GetMemberFunction(const std::string& name) const;
        void Serialize(Common::SerializeStream& stream, Any* obj) const;
        void Deserailize(Common::DeserializeStream& stream, Any* obj) const;

    private:
        static std::unordered_map<TypeId, std::string> typeToNameMap;

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
        Constructor& EmplaceConstructor(const std::string& inName, Constructor::ConstructParams&& inParams);
        Variable& EmplaceStaticVariable(const std::string& inName, Variable::ConstructParams&& inParams);
        Function& EmplaceStaticFunction(const std::string& inName, Function::ConstructParams&& inParams);
        MemberVariable& EmplaceMemberVariable(const std::string& inName, MemberVariable::ConstructParams&& inParams);
        MemberFunction& EmplaceMemberFunction(const std::string& inName, MemberFunction::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        BaseClassGetter baseClassGetter;
        std::optional<Any> defaultObject;
        std::optional<Destructor> destructor;
        std::unordered_map<std::string, Constructor> constructors;
        std::unordered_map<std::string, Variable> staticVariables;
        std::unordered_map<std::string, Function> staticFunctions;
        std::unordered_map<std::string, MemberVariable> memberVariables;
        std::unordered_map<std::string, MemberFunction> memberFunctions;
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
        bool Compare(Any* value) const;

        using Getter = std::function<Any()>;
        using Comparer = std::function<bool(Any*)>;

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
        template <typename T>
        requires std::is_enum_v<T>
        static const Enum* Find();

        template <typename T>
        requires std::is_enum_v<T>
        static const Enum& Get();

        static const Enum* Find(const std::string& name);
        static const Enum& Get(const std::string& name);

        ~Enum() override;

        const TypeInfo* GetTypeInfo() const;
        Any GetElement(const std::string& name) const;
        std::string GetElementName(Any* value) const;

    private:
        static std::unordered_map<TypeId, std::string> typeToNameMap;

        friend class Registry;
        template <typename T> friend class EnumRegistry;

        struct ConstructParams {
            std::string name;
            const TypeInfo* typeInfo;
        };

        explicit Enum(ConstructParams&& params);

        EnumElement& EmplaceElement(const std::string& inName, EnumElement::ConstructParams&& inParams);

        const TypeInfo* typeInfo;
        std::unordered_map<std::string, EnumElement> elements;
    };
}

namespace Mirror {
    template <typename T>
    TypeInfo* GetTypeInfo()
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
        ConstructFromValue(inValue);
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
        return Convertible(GetTypeInfo<T>());
    }

    template <typename T>
    bool Any::Convertible() const
    {
        return Convertible(GetTypeInfo<T>());
    }

    template <typename T>
    T Any::As()
    {
        Assert(Convertible<T>());
        return *static_cast<std::remove_cvref_t<T>*>(Ptr());
    }

    template <typename T>
    T Any::As() const
    {
        Assert(Convertible<T>());
        return *static_cast<std::remove_cvref_t<T>*>(Ptr());
    }

    template <typename T>
    T* Any::TryAs()
    {
        return Convertible<T>() ? static_cast<std::remove_cvref_t<T>*>(Ptr()) : nullptr;
    }

    template <typename T>
    T* Any::TryAs() const
    {
        return Convertible<T>() ? static_cast<std::remove_cvref_t<T>*>(Ptr()) : nullptr;
    }

    template <typename T>
    void Any::ConstructFromValue(T&& inValue)
    {
        using RawType = std::remove_cvref_t<T>;

        policy = AnyPolicy::memoryHolder;
        rtti = &anyRttiImpl<RawType>;
        typeId = GetTypeInfo<T>()->id;
        info = HolderInfo(sizeof(RawType));
        new(Ptr()) RawType(std::forward<T>(inValue));
    }

    template <typename T>
    void Any::ConstructFromRef(std::reference_wrapper<T> inRef)
    {
        using RawType = std::remove_const_t<T>;

        policy = std::is_const_v<T> ? AnyPolicy::constRef : AnyPolicy::ref;
        rtti = &anyRttiImpl<RawType>;
        typeId = GetTypeInfo<T>()->id;
        info = RefInfo(const_cast<RawType*>(&inRef.get()), sizeof(RawType));
    }

    template <typename T>
    void Variable::Set(T value) const
    {
        auto ref = Any(std::forward<std::remove_reference_t<T>>(value));
        Set(&ref);
    }

    template <typename... Args>
    Any Function::Invoke(Args&&... args) const
    {
        // TODO check const any and non const any
        std::array<Any, sizeof...(args)> refs = { Any(std::forward<Args>(args))... };
        return InvokeWith(refs.data(), refs.size());
    }

    template <typename... Args>
    Any Constructor::ConstructOnStack(Args&&... args) const
    {
        // TODO check const any and non const any
        std::array<Any, sizeof...(args)> refs = { Any(std::forward<Args>(args))... };
        return ConstructOnStackWith(refs.data(), refs.size());
    }

    template <typename... Args>
    Any Constructor::NewObject(Args&&... args) const
    {
        // TODO check const any and non const any
        std::array<Any, sizeof...(args)> refs = { Any(std::forward<Args>(args))... };
        return NewObjectWith(refs.data(), refs.size());
    }

    template <typename C>
    void Destructor::Invoke(C&& object) const
    {
        auto classRef = Any(std::ref(std::forward<C>(object)));
        InvokeWith(&classRef);
    }

    template <typename C, typename T>
    void MemberVariable::Set(C&& object, T value) const
    {
        auto classRef = Any(std::ref(std::forward<C>(object)));
        auto valueRef = Any(std::forward<std::remove_reference_t<T>>(value));
        Set(&classRef, &valueRef);
    }

    template <typename C, typename... Args>
    Any MemberFunction::Invoke(C&& object, Args&&... args) const
    {
        // TODO support const any and non-const any
        auto classRef = Any(std::ref(std::forward<C>(object)));
        std::array<Any, sizeof...(args)> argRefs = { Any(std::forward<Args>(args))... };
        return InvokeWith(&classRef, argRefs.data(), argRefs.size());
    }

    template <typename F>
    void GlobalScope::ForEachVariable(F&& func) const
    {
        for (const auto& [name, variable] : variables) {
            func(variable);
        }
    }

    template <typename F>
    void GlobalScope::ForEachFunction(F&& func) const
    {
        for (const auto& [name, function] : functions) {
            func(function);
        }
    }

    template <typename C>
    requires std::is_class_v<C>
    bool Class::Has()
    {
        return Has(Mirror::GetTypeInfo<C>());
    }

    template <typename C>
    requires std::is_class_v<C>
    const Class* Class::Find()
    {
        return Find(Mirror::GetTypeInfo<C>());
    }

    template <typename C>
    requires std::is_class_v<C>
    const Class& Class::Get()
    {
        return Get(Mirror::GetTypeInfo<C>());
    }

    template <typename F>
    void Class::ForEachStaticVariable(F&& func) const
    {
        for (const auto& [name, staticVariable] : staticVariables) {
            func(staticVariable);
        }
    }

    template <typename F>
    void Class::ForEachStaticFunction(F&& func) const
    {
        for (const auto& [name, staticFunction] : staticFunctions) {
            func(staticFunction);
        }
    }

    template <typename F>
    void Class::ForEachMemberVariable(F&& func) const
    {
        for (const auto& [name, variable] : memberVariables) {
            func(variable);
        }
    }

    template <typename F>
    void Class::ForEachMemberFunction(F&& func) const
    {
        for (const auto& [name, function] : memberFunctions) {
            func(function);
        }
    }

    template <typename T>
    requires std::is_enum_v<T>
    const Enum* Enum::Find()
    {
        auto iter = typeToNameMap.find(Mirror::GetTypeInfo<T>()->id);
        Assert(iter != typeToNameMap.end());
        return Find(iter->second);
    }

    template <typename T>
    requires std::is_enum_v<T>
    const Enum& Enum::Get()
    {
        auto iter = typeToNameMap.find(Mirror::GetTypeInfo<T>()->id);
        Assert(iter != typeToNameMap.end());
        return Get(iter->second);
    }
}
