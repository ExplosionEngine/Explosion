//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <optional>

#include <Common/Serialization.h>
#include <Mirror/Any.h>
#include <Mirror/Api.h>
#include <Mirror/TypeInfo.h>

namespace Mirror {
    struct NamePresets {
        static constexpr const char* globalScope = "_globalScope";
        static constexpr const char* destructor = "_destructor";
        static constexpr const char* defaultConstructor = "_defaultConstructor";
    };

    class Variable;
    class MemberVariable;

    using VariableSerializer = std::function<void(Common::SerializeStream&, const Variable&)>;
    using MemberVariableSerializer = std::function<void(Common::SerializeStream&, const MemberVariable&, Any*)>;
    using VariableDeserializer = std::function<void(Common::DeserializeStream&, const Variable&)>;
    using MemberVariableDeserializer = std::function<void(Common::DeserializeStream&, const MemberVariable&, Any*)>;
}

namespace Mirror {
    class MIRROR_API Type {
    public:
        virtual ~Type();

        [[nodiscard]] const std::string& GetName() const;
        [[nodiscard]] const std::string& GetMeta(const std::string& key) const;
        [[nodiscard]] std::string GetAllMeta() const;
        bool HasMeta(const std::string& key) const;

    protected:
        explicit Type(std::string inName);

    private:
        template <typename Derived> friend class MetaDataRegistry;

        std::string name;
        std::unordered_map<std::string, std::string> metas;
    };

    class MIRROR_API Variable : public Type {
    public:
        ~Variable() override;

        template <typename T>
        void Set(T value) const
        {
            Any ref = Any(std::forward<std::remove_reference_t<T>>(value));
            Set(&ref);
        }

        const TypeInfo* GetTypeInfo() const;
        void Set(Any* value) const;
        Any Get() const;
        void Serialize(Common::SerializeStream& stream) const;
        void Deserialize(Common::DeserializeStream& stream) const;

    private:
        friend class GlobalRegistry;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(Any*)>;
        using Getter = std::function<Any()>;

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

    class MIRROR_API Function : public Type {
    public:
        ~Function() override;

        template <typename... Args>
        Any Invoke(Args... args) const
        {
            std::array<Any, sizeof...(args)> refs = { Any(std::forward<std::remove_reference_t<Args>>(args))... };
            return InvokeWith(refs.data(), refs.size());
        }

        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        Any InvokeWith(Any* arguments, uint8_t argumentsSize) const;

    private:
        friend class GlobalRegistry;
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

    class MIRROR_API Constructor : public Type {
    public:
        ~Constructor() override;

        template <typename... Args>
        Any ConstructOnStack(Args... args) const
        {
            std::array<Any, sizeof...(args)> refs = { Any(std::forward<std::remove_reference_t<Args>>(args))... };
            return ConstructOnStackWith(refs.data(), refs.size());
        }

        template <typename... Args>
        Any NewObject(Args... args) const
        {
            std::array<Any, sizeof...(args)> refs = { Any(std::forward<std::remove_reference_t<Args>>(args))... };
            return NewObjectWith(refs.data(), refs.size());
        }

        uint8_t GetArgsNum() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        Any ConstructOnStackWith(Any* arguments, uint8_t argumentsSize) const;
        Any NewObjectWith(Any* arguments, uint8_t argumentsSize) const;

    private:
        friend class Registry;
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

    class MIRROR_API Destructor : public Type {
    public:
        ~Destructor() override;

        template <typename C>
        void Invoke(C&& object) const
        {
            Any classRef = Any(std::ref(std::forward<C>(object)));
            InvokeWith(&classRef);
        }

        void InvokeWith(Any* object) const;

    private:
        friend class Registry;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<void(Any*)>;

        struct ConstructParams {
            Invoker destructor;
        };

        explicit Destructor(ConstructParams&& params);

        Invoker destructor;
    };

    class MIRROR_API MemberVariable : public Type {
    public:
        ~MemberVariable() override;

        template <typename C, typename T>
        void Set(C&& object, T value) const
        {
            Any classRef = Any(std::ref(std::forward<C>(object)));
            Any valueRef = Any(std::forward<std::remove_reference_t<T>>(value));
            Set(&classRef, &valueRef);
        }

        uint32_t SizeOf() const;
        const TypeInfo* GetTypeInfo() const;
        void Set(Any* object, Any* value) const;
        Any Get(Any* object) const;
        void Serialize(Common::SerializeStream& stream, Any* object) const;
        void Deserialize(Common::DeserializeStream& stream, Any* object) const;

    private:
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(Any*, Any*)>;
        using Getter = std::function<Any(Any*)>;

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

    class MIRROR_API MemberFunction : public Type {
    public:
        ~MemberFunction() override;

        template <typename C, typename... Args>
        Any Invoke(C&& object, Args&&... args) const
        {
            Any classRef = Any(std::ref(std::forward<C>(object)));
            std::array<Any, sizeof...(Args)> argRefs = { Any(std::forward<std::remove_reference_t<Args>>(args))... };
            return InvokeWith(&classRef, argRefs.data(), argRefs.size());
        }

        uint8_t GetArgsNum() const;
        const TypeInfo* GetRetTypeInfo() const;
        const TypeInfo* GetArgTypeInfo(uint8_t argIndex) const;
        const std::vector<const TypeInfo*>& GetArgTypeInfos() const;
        Any InvokeWith(Any* object, Any* args, size_t argsSize) const;

    private:
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

    class MIRROR_API GlobalScope : public Type {
    public:
        ~GlobalScope() override;

        static const GlobalScope& Get();

        template <typename F>
        void ForEachVariable(F&& func) const
        {
            for (const auto& iter : variables) {
                func(iter.second);
            }
        }

        template <typename F>
        void ForEachFunction(F&& func) const
        {
            for (const auto& iter : functions) {
                func(iter.second);
            }
        }

        [[nodiscard]] bool HasVariable(const std::string& name) const;
        [[nodiscard]] const Variable* FindVariable(const std::string& name) const;
        [[nodiscard]] const Variable& GetVariable(const std::string& name) const;
        [[nodiscard]] bool HasFunction(const std::string& name) const;
        [[nodiscard]] const Function* FindFunction(const std::string& name) const;
        [[nodiscard]] const Function& GetFunction(const std::string& name) const;

    private:
        friend class Registry;
        friend class GlobalRegistry;

        GlobalScope();

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Function> functions;
    };

    class MIRROR_API Class : public Type {
    public:
        ~Class() override;

        template <typename C>
        requires std::is_class_v<C>
        [[nodiscard]] static bool Has()
        {
            return typeToNameMap.contains(Mirror::GetTypeInfo<C>()->id);
        }

        template <typename C>
        requires std::is_class_v<C>
        [[nodiscard]] static const Class* Find()
        {
            auto iter = typeToNameMap.find(Mirror::GetTypeInfo<C>()->id);
            if (iter == typeToNameMap.end()) {
                return nullptr;
            }
            return Find(iter->second);
        }

        template <typename C>
        requires std::is_class_v<C>
        [[nodiscard]] static const Class& Get()
        {
            auto iter = typeToNameMap.find(Mirror::GetTypeInfo<C>()->id);
            AssertWithReason(iter != typeToNameMap.end(), "did you forget add EClass() annotation to class ?");
            return Get(iter->second);
        }

        [[nodiscard]] static bool Has(const std::string& name);
        [[nodiscard]] static const Class* Find(const std::string& name);
        [[nodiscard]] static const Class& Get(const std::string& name);

        template <typename F>
        void ForEachStaticVariable(F&& func) const
        {
            for (const auto& iter : staticVariables) {
                func(iter.second);
            }
        }

        template <typename F>
        void ForEachStaticFunction(F&& func) const
        {
            for (const auto& iter : staticFunctions) {
                func(iter.second);
            }
        }

        template <typename F>
        void ForEachMemberVariable(F&& func) const
        {
            for (const auto& iter : memberVariables) {
                func(iter.second);
            }
        }

        template <typename F>
        void ForEachMemberFunction(F&& func) const
        {
            for (const auto& iter : memberFunctions) {
                func(iter.second);
            }
        }

        template <typename T>
        T* DynamicCast(Any* target) const
        {
            bool isClassPointer = classPointerChecker(target);
            if (isClassPointer) {
                return target->ForceAs<T*>();
            }
            const Class* baseClass = GetBaseClass();
            if (baseClass == nullptr) {
                return nullptr;
            }
            return baseClass->DynamicCast<T>(target);
        }

        [[nodiscard]] const TypeInfo* GetTypeInfo() const;
        [[nodiscard]] bool HasDefaultConstructor() const;
        [[nodiscard]] const Mirror::Class* GetBaseClass() const;
        [[nodiscard]] const Constructor* FindDefaultConstructor() const;
        [[nodiscard]] const Constructor& GetDefaultConstructor() const;
        [[nodiscard]] bool HasDestructor() const;
        [[nodiscard]] const Destructor* FindDestructor() const;
        [[nodiscard]] const Destructor& GetDestructor() const;
        [[nodiscard]] bool HasConstructor(const std::string& name) const;
        [[nodiscard]] const Constructor* FindConstructor(const std::string& name) const;
        [[nodiscard]] const Constructor& GetConstructor(const std::string& name) const;
        [[nodiscard]] bool HasStaticVariable(const std::string& name) const;
        [[nodiscard]] const Variable* FindStaticVariable(const std::string& name) const;
        [[nodiscard]] const Variable& GetStaticVariable(const std::string& name) const;
        [[nodiscard]] bool HasStaticFunction(const std::string& name) const;
        [[nodiscard]] const Function* FindStaticFunction(const std::string& name) const;
        [[nodiscard]] const Function& GetStaticFunction(const std::string& name) const;
        [[nodiscard]] bool HasMemberVariable(const std::string& name) const;
        [[nodiscard]] const MemberVariable* FindMemberVariable(const std::string& name) const;
        [[nodiscard]] const MemberVariable& GetMemberVariable(const std::string& name) const;
        [[nodiscard]] bool HasMemberFunction(const std::string& name) const;
        [[nodiscard]] const MemberFunction* FindMemberFunction(const std::string& name) const;
        [[nodiscard]] const MemberFunction& GetMemberFunction(const std::string& name) const;
        void Serialize(Common::SerializeStream& stream, Mirror::Any* obj) const;
        void Deserailize(Common::DeserializeStream& stream, Mirror::Any* obj) const;

    private:
        static std::unordered_map<TypeId, std::string> typeToNameMap;

        friend class Registry;
        template <typename T> friend class ClassRegistry;

        using BaseClassGetter = std::function<const Mirror::Class*()>;
        using ClassPointerChecker = std::function<bool(Mirror::Any*)>;

        struct ConstructParams {
            std::string name;
            const TypeInfo* typeInfo;
            BaseClassGetter baseClassGetter;
            ClassPointerChecker classPointerChecker;
            std::optional<Mirror::Any> defaultObject;
            std::optional<Destructor> destructor;
            std::optional<Constructor> defaultConstructor;
        };

        explicit Class(ConstructParams&& params);

        const TypeInfo* typeInfo;
        BaseClassGetter baseClassGetter;
        ClassPointerChecker classPointerChecker;
        std::optional<Mirror::Any> defaultObject;
        std::optional<Destructor> destructor;
        std::unordered_map<std::string, Constructor> constructors;
        std::unordered_map<std::string, Variable> staticVariables;
        std::unordered_map<std::string, Function> staticFunctions;
        std::unordered_map<std::string, MemberVariable> memberVariables;
        std::unordered_map<std::string, MemberFunction> memberFunctions;
    };

    class MIRROR_API EnumElement : public Type {
    public:
        ~EnumElement() override;

    private:
        friend class Registry;
        template <typename T> friend class EnumRegistry;

        friend class Enum;

        [[nodiscard]] Any Get() const;
        [[nodiscard]] bool Compare(Any* value) const;

        using Getter = std::function<Any()>;
        using Comparer = std::function<bool(Any*)>;

        EnumElement(std::string inName, Getter inGetter, Comparer inComparer);

        Getter getter;
        Comparer comparer;
    };

    class MIRROR_API Enum : public Type {
    public:
        template <typename T>
        requires std::is_enum_v<T>
        [[nodiscard]] static const Enum* Find()
        {
            auto iter = typeToNameMap.find(Mirror::GetTypeInfo<T>()->id);
            Assert(iter != typeToNameMap.end());
            return Find(iter->second);
        }

        template <typename T>
        requires std::is_enum_v<T>
        [[nodiscard]] static const Enum& Get()
        {
            auto iter = typeToNameMap.find(Mirror::GetTypeInfo<T>()->id);
            Assert(iter != typeToNameMap.end());
            return Get(iter->second);
        }

        [[nodiscard]] static const Enum* Find(const std::string& name);
        [[nodiscard]] static const Enum& Get(const std::string& name);

        ~Enum() override;

        [[nodiscard]] const TypeInfo* GetTypeInfo() const;
        [[nodiscard]] Any GetElement(const std::string& name) const;
        [[nodiscard]] std::string GetElementName(Any* value) const;

    private:
        static std::unordered_map<TypeId, std::string> typeToNameMap;

        friend class Registry;
        template <typename T> friend class EnumRegistry;

        struct ConstructParams {
            std::string name;
            const TypeInfo* typeInfo;
        };

        explicit Enum(ConstructParams&& params);

        const TypeInfo* typeInfo;
        std::unordered_map<std::string, EnumElement> elements;
    };
}
