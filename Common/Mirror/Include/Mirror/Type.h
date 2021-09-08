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
        std::unordered_map<std::string, std::string> metas;
    };

    struct VariableInfo : public TypeInfo {
        std::function<Any(Ref)> getter;
        std::function<void(Ref, Ref)> setter;
    };

    struct FunctionInfo : public TypeInfo {
        std::function<Any(Ref, std::vector<Ref>)> invoker;
        VariableInfo* retType;
        std::vector<VariableInfo*> argTypes;
    };

    struct StructInfo : public TypeInfo {
        std::vector<VariableInfo*> memberVarTypes;
    };

    struct ClassInfo : public StructInfo {
        std::vector<FunctionInfo*> memberFuncTypes;
    };

    template <typename T>
    struct TypeTraits {
        static std::string Name()
        {
            return typeid(T).name();
        }

        static size_t HashCode()
        {
            return typeid(T).hash_code();
        }
    };
}

namespace Explosion::Mirror {
    class Type;
    class Variable;
    class Function;
    class Struct;
    class Class;

    class Type {
    public:
        ~Type() = default;

        size_t GetId();
        const std::string& GetName();
        const std::string& Meta(const std::string& key);

    protected:
        explicit Type(Internal::TypeInfo* info);

    private:
        Internal::TypeInfo* info;
    };

    class Variable : public Type {
    public:
        ~Variable() = default;

        Any Get(Ref instance);
        void Set(Ref instance, Ref value);

    protected:
        explicit Variable(Internal::VariableInfo* info);

    private:
        friend Function;
        friend Struct;

        Internal::VariableInfo* info;
    };

    class Function : public Type {
    public:
        ~Function() = default;

        template <typename... Args>
        void Invoke(Ref instance, Args&&... args);

        Variable GetReturnType();
        std::vector<Variable> GetArgumentTypes();

    protected:
        explicit Function(Internal::FunctionInfo* info);

    private:
        friend Class;

        Internal::FunctionInfo* info;
    };

    class Struct : public Type {
    public:
        ~Struct() = default;

        std::vector<Variable> GetMemberVariableTypes();

    protected:
        explicit Struct(Internal::StructInfo* info);

    private:
        Internal::StructInfo* info;
    };

    class Class : public Struct {
    public:
        ~Class() = default;

        std::vector<Function> GetMemberFunctionTypes();

    protected:
        explicit Class(Internal::ClassInfo* info);

    private:
        Internal::ClassInfo* info;
    };
}

#endif //EXPLOSION_TYPE_H
