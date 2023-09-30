//
// Created by johnk on 2023/9/1.
//

#include <Runtime/RuntimeModule.h>

namespace Runtime {
    RuntimeModule::RuntimeModule() = default;

    RuntimeModule::~RuntimeModule() = default;

    World* RuntimeModule::CreateWorld(const std::string& inName) const
    {
        return new World(inName);
    }

    void RuntimeModule::DestroyWorld(World* inWorld) const
    {
        delete inWorld;
    }
}

IMPLEMENT_MODULE(RUNTIME_API, Runtime::RuntimeModule);
