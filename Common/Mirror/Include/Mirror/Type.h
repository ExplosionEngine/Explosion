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
        TypeInfo* type;
        std::function<Any(Ref)> getter;
        std::function<void(Ref, Ref)> setter;
    };

    struct FunctionInfo {
        TypeInfo* retType;
        std::vector<TypeInfo*> argTypes;
        std::function<Any(Ref, std::vector<Ref>)> invoker;
    };

    struct StructInfo {
        TypeInfo* typeInfo;
        std::vector<VariableInfo*> memberVariables;
    };

    struct ClassInfo {
        TypeInfo* typeInfo;
        std::vector<VariableInfo*> memberVariables;
        std::vector<FunctionInfo*> memberFunctions;
    };

    template <typename T>
    TypeInfo* FetchTypeInfo()
    {
        static TypeInfo info {
            typeid(T).hash_code(),
            typeid(T).name()
        };
        return &info;
    }
}

namespace Explosion::Mirror {}

#endif //EXPLOSION_TYPE_H
