//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <vector>
#include <array>
#include <unordered_map>
#include <optional>

#include <Mirror/Any.h>
#include <Mirror/Api.h>
#include <Mirror/TypeInfo.h>

namespace Mirror {
    class MIRROR_API Type {
    public:
        virtual ~Type();

        const std::string& GetMeta(const std::string& key) const;
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
            Any ref = Any::From(std::forward<std::remove_reference_t<T>>(value));
            Set(&ref);
        }

        void Set(Any* value) const;
        Any Get() const;

    private:
        friend class GlobalRegistry;
        template <typename C> friend class ClassRegistry;

        using Setter = std::function<void(Any*)>;
        using Getter = std::function<Any()>;

        Variable(std::string inName, Setter inSetter, Getter inGetter);

        Setter setter;
        Getter getter;
    };

    class MIRROR_API Function : public Type {
    public:
        ~Function() override;

        template <typename... Args>
        Any Invoke(Args... args) const
        {
            std::array<Any, sizeof...(args)> refs = { Any::From(std::forward<std::remove_reference_t<Args>>(args))... };
            return InvokeWith(refs.data(), refs.size());
        }

        Any InvokeWith(Any* arguments, size_t argumentsSize) const;

    private:
        friend class GlobalRegistry;
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(Any*, size_t)>;

        Function(std::string inName, Invoker inInvoker);

        Invoker invoker;
    };

    class MIRROR_API Constructor : public Type {
    public:
        ~Constructor() override;

        template <typename... Args>
        Any ConstructOnStack(Args... args)
        {
            std::array<Any, sizeof...(args)> refs = { Any::From(std::forward<std::remove_reference_t<Args>>(args))... };
            return ConstructOnStackWith(refs.data(), refs.size());
        }

        template <typename... Args>
        Any NewObject(Args... args)
        {
            std::array<Any, sizeof...(args)> refs = { Any::From(std::forward<std::remove_reference_t<Args>>(args))... };
            return NewObjectWith(refs.data(), refs.size());
        }

        Any ConstructOnStackWith(Any* arguments, size_t argumentsSize);
        Any NewObjectWith(Any* arguments, size_t argumentsSize);

    private:
        template <typename C> friend class ClassRegistry;

        using Invoker = std::function<Any(Any*, size_t)>;

        Constructor(std::string inName, Invoker inStackConstructor, Invoker inHeapConstructor);

        Invoker stackConstructor;
        Invoker heapConstructor;
    };

    class MIRROR_API MemberVariable : public Type {
    public:
        ~MemberVariable() override;

        template <typename C, typename T>
        void Set(C&& clazz, T value)
        {
            Any classRef = Any(std::ref(std::forward<C>(clazz)));
            Any valueRef = Any::From(std::forward<std::remove_reference_t<T>>(value));
            Set(&classRef, &valueRef);
        }

        void Set(Any* clazz, Any* value);
        Any Get(Any* clazz);

    private:
        using Setter = std::function<void(Any*, Any*)>;
        using Getter = std::function<Any(Any*)>;

        MemberVariable(std::string inName, Setter inSetter, Getter inGetter);

        Setter setter;
        Getter getter;
    };

    class MIRROR_API MemberFunction : public Type {
    public:
        ~MemberFunction() override;

        template <typename C, typename... Args>
        Any Invoke(C&& clazz, Args&&... args) const
        {
            Any classRef = Any(std::ref(std::forward<C>(clazz)));
            std::array<Any, sizeof...(Args)> argRefs = { Any::From(std::forward<std::remove_reference_t<Args>>(args))... };
            return InvokeWith(&classRef, argRefs.data(), argRefs.size());
        }

        Any InvokeWith(Any* clazz, Any* args, size_t argsSize) const;

    private:
        using Invoker = std::function<Any(Any*, Any*, size_t)>;

        MemberFunction(std::string inName, Invoker inInvoker);

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

        [[nodiscard]] const Variable* FindVariable(const std::string& name) const;
        [[nodiscard]] const Variable& GetVariable(const std::string& name) const;
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

        [[nodiscard]] static const Class* Find(const std::string& name);
        [[nodiscard]] static const Class& Get(const std::string& name);

        [[nodiscard]] const Constructor* FindConstructor(const std::string& name) const;
        [[nodiscard]] const Constructor& GetConstructor(const std::string& name) const;
        [[nodiscard]] const Variable* FindStaticVariable(const std::string& name) const;
        [[nodiscard]] const Variable& GetStaticVariable(const std::string& name) const;
        [[nodiscard]] const Function* FindStaticFunction(const std::string& name) const;
        [[nodiscard]] const Function& GetStaticFunction(const std::string& name) const;
        [[nodiscard]] const MemberVariable* FindMemberVariable(const std::string& name) const;
        [[nodiscard]] const MemberVariable& GetMemberVariable(const std::string& name) const;
        [[nodiscard]] const MemberFunction* FindMemberFunction(const std::string& name) const;
        [[nodiscard]] const MemberFunction& GetMemberFunction(const std::string& name) const;

    private:
        friend class Registry;
        template <typename T> friend class ClassRegistry;

        explicit Class(std::string name);

        std::optional<Function> destructor;
        std::unordered_map<std::string, Constructor> constructors;
        std::unordered_map<std::string, Variable> staticVariables;
        std::unordered_map<std::string, Function> staticFunctions;
        std::unordered_map<std::string, MemberVariable> memberVariables;
        std::unordered_map<std::string, MemberFunction> memberFunctions;
    };
}
