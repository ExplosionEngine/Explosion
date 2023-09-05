//
// Created by johnk on 2023/9/1.
//

#pragma once

#include <Core/Module.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API RuntimeModule : public Core::Module {
    public:
        RuntimeModule();
        ~RuntimeModule() override;
    };
}
