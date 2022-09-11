//
// Created by johnk on 2022/9/10.
//

#include <Mirror/Registry.h>

namespace Mirror {
    GlobalVariable::GlobalVariable(const GlobalVariableRegisterInfo& inInfo)
        : setter(inInfo.setter), getter(inInfo.getter)
    {
    }

    GlobalVariable::~GlobalVariable() = default;

    void GlobalVariable::Set(Any& value)
    {
        setter(value);
    }

    Any GlobalVariable::Get()
    {
        return getter();
    }
}
