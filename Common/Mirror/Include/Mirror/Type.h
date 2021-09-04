//
// Created by johnk on 2021/9/4.
//

#ifndef EXPLOSION_TYPE_H
#define EXPLOSION_TYPE_H

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Explosion::Mirror::Internal {
    class Any;
    class Ref;

    struct TypeInfo {
        size_t id;
        std::string name;
        std::unordered_map<std::string, std::string> metas;
    };

    struct VariableInfo : public TypeInfo {
        std::function<Any(Ref, size_t)> getter;
        std::function<void(Ref, size_t, Ref)> setter;
    };

    struct FunctionInfo : public TypeInfo {
        std::function<Any(Ref, std::vector<Ref>)> invoker;
    };

    struct ComplexTypeInfo : public TypeInfo {};

    struct StructTypeInfo : public ComplexTypeInfo {
        std::vector<VariableInfo> memberVars;
    };

    struct ClassTypeInfo : public StructTypeInfo {
        std::vector<FunctionInfo> memberFuncs;
    };

    template <typename T>
    struct TypeTraits {
        static std::string Name()
        {
            return typeid(T).name();
        }

        static size_t Id()
        {
            return typeid(T).hash_code();
        }
    };
}

#endif //EXPLOSION_TYPE_H
