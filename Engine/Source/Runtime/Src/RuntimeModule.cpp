//
// Created by johnk on 2023/9/1.
//

#include <Runtime/RuntimeModule.h>

namespace Runtime {
    RuntimeModule::RuntimeModule()
        : initialized(false)
    {
    }

    RuntimeModule::~RuntimeModule() = default;

    void RuntimeModule::Initialize(const RuntimeModuleInitParams& inParams)
    {
        Assert(!initialized);
        CreateEngine();
        initialized = true;
    }

    Engine* RuntimeModule::GetEngine()
    {
        return engine.Get();
    }

    World* RuntimeModule::CreateWorld(const std::string& inName) const
    {
        return new World(inName);
    }

    void RuntimeModule::DestroyWorld(World* inWorld) const
    {
        delete inWorld;
    }

    void RuntimeModule::CreateEngine()
    {
        // TODO load game/editor's main dynamic module to create custom engine
        engine = Common::UniqueRef<Engine>(new GameEngine({}));
    }
}

IMPLEMENT_MODULE(RUNTIME_API, Runtime::RuntimeModule);
