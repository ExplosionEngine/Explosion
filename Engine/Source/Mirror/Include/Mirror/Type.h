//
// Created by johnk on 2022/9/21.
//

#pragma once

#include <array>
#include <unordered_map>

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
        template <typename E>
        friend class MetaDataRegistry;

        std::string name;
        std::unordered_map<std::string, std::string> metas;
    };

    class MIRROR_API Variable : public Type {
    public:
        ~Variable() override;

        template <typename T>
        void Set(T&& value)
        {
            Any ref = Any(std::ref(std::forward<T>(value)));
            Set(&ref);
        }

        void Set(Any* value);
        Any Get();

    private:
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
        Any Invoke(Args&&... args)
        {
            std::array<Any*, sizeof...(Args)> refs = { std::ref(std::forward<Args>(args))... };
            return Invoke(refs.data(), refs.size());
        }

        Any Invoke(Any* arguments, size_t argumentsSize);

    private:
        using Invoker = std::function<Any(Any*, size_t)>;

        Function(std::string inName, Invoker inInvoker);

        Invoker invoker;
    };

    class MIRROR_API MemberVariable : public Type {
    public:
        ~MemberVariable() override;

        template <typename C, typename T>
        void Set(C&& clazz, T&& value)
        {
            Any classRef = Any(std::ref(std::forward<C>(clazz)));
            Any valueRef = Any(std::ref(std::forward<T>(value)));
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
        Any Invoke(C&& clazz, Args&&... args)
        {
            Any classRef = Any(std::ref(std::forward<C>(clazz)));
            std::array<Any, sizeof...(Args)> argRefs = { std::ref(std::forward<Args>(args))... };
            return Invoke(&classRef, argRefs.data(), argRefs.size());
        }

        Any Invoke(Any* clazz, Any* args, size_t argsSize);

    private:
        using Invoker = std::function<Any(Any*, Any*, size_t)>;

        MemberFunction(std::string inName, Invoker inInvoker);

        Invoker invoker;
    };

    class MIRROR_API GlobalScope : public Type {
    public:
        ~GlobalScope() override = default;

        // TODO

    private:
        friend class Registry;
        friend class GlobalRegistry;

        GlobalScope() : Type("_GlobalScope") {}

        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Function> functions;
    };

    class MIRROR_API Class : public Type {
    public:
        ~Class() override = default;

        // TODO

    private:
        friend class Registry;

        explicit Class(std::string name) : Type(std::move(name)) {}

        std::unordered_map<std::string, Function> constructors;
        std::unordered_map<std::string, Variable> staticVariables;
        std::unordered_map<std::string, Function> staticFunctions;
        std::unordered_map<std::string, MemberVariable> memberVariables;
        std::unordered_map<std::string, MemberFunction> memberFunctions;
    };
}
