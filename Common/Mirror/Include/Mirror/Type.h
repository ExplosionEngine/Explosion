//
// Created by johnk on 2021/9/4.
//

#ifndef EXPLOSION_TYPE_H
#define EXPLOSION_TYPE_H

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

#include <Mirror/Exception.h>

namespace Explosion::Mirror {
    class Any;
    class Ref;
}

namespace Explosion::Mirror::Internal {
    struct TypeInfo {
        size_t id;
        std::string name;
    };

    struct VariableInfo {
        const TypeInfo* type;
        std::function<Any(Ref)> getter;
        std::function<void(Ref, Ref)> setter;
    };

    struct FunctionInfo {
        const TypeInfo* type;
        const TypeInfo* retType;
        std::vector<const TypeInfo*> argTypes;
        std::function<Any(Ref, std::vector<Ref>)> invoker;
    };

    template <typename T>
    const TypeInfo* FetchTypeInfo()
    {
        static TypeInfo info {
            typeid(T).hash_code(),
            typeid(T).name()
        };
        return &info;
    }
}

namespace Explosion::Mirror {
    class Type {
    public:
        explicit Type(const Internal::TypeInfo* info) : info(info) {}
        ~Type() = default;

    private:
        const Internal::TypeInfo* info;
    };

    class Definition {
    public:
        Definition() = default;
        ~Definition() = default;

        virtual Type GetType() = 0;
    };

    class Variable : public Definition {
    public:
        explicit Variable(const Internal::VariableInfo* info) : Definition(), info(info) {}
        Variable(Variable&& variable) noexcept : info(variable.info) {}
        Variable(const Variable&) = default;
        Variable& operator=(const Variable&) = default;
        ~Variable() = default;

        Type GetType() override;
        Any Get(Ref instance);
        void Set(Ref instance, Ref value);

    private:
        const Internal::VariableInfo* info;
    };

    class Function : public Definition {
    public:
        explicit Function(const Internal::FunctionInfo* info) : Definition(), info(info) {}
        Function(Function&& function) noexcept : info(function.info) {}
        Function(const Function&) = default;
        Function& operator=(const Function&) = default;
        ~Function() = default;

        Type GetType() override;

        template <typename... Args>
        Any Invoke(Ref instance, Args&&... args);

    private:
        const Internal::FunctionInfo* info;
    };
}

#endif //EXPLOSION_TYPE_H
