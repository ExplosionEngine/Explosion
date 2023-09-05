//
// Created by johnk on 2023/9/1.
//

#include <Runtime/RuntimeModule.h>

namespace Runtime {
    RuntimeModule::RuntimeModule() = default;

    RuntimeModule::~RuntimeModule() = default;
}

IMPLEMENT_MODULE(RUNTIME_API, Runtime::RuntimeModule);
