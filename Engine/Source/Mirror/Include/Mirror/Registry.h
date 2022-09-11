//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <vector>

#include <Mirror/Api.h>
#include <Mirror/Type.h>
#include <Mirror/Any.h>

namespace Mirror {
    struct GlobalVariableRegisterInfo {
        std::function<void(Any&)> setter;
        std::function<Any()> getter;
    };

    class GlobalVariable {
    public:
        explicit GlobalVariable(const GlobalVariableRegisterInfo& inInfo);
        ~GlobalVariable();

        void Set(Any& value);
        Any Get();

    private:
        std::function<void(Any&)> setter;
        std::function<Any()> getter;
    };
}

namespace Mirror {
    class MIRROR_API Registry {
    public:
        static Registry Get()
        {
            static Registry instance;
            return instance;
        }

        template <typename T>
        void RegisterVariable(T* ptr)
        {
            GlobalVariableRegisterInfo info;
            info.setter = [ptr](Any& value) -> void { *ptr = value.CastTo<T>(); };
            info.getter = [ptr]() -> Any { return Any(std::ref(*ptr)); };
            globalVariables.emplace_back(info);
        }

        // TODO

    private:
        std::vector<GlobalVariable> globalVariables;
    };
}
