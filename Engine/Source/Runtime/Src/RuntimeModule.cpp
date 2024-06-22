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

    Engine* RuntimeModule::GetEngine() const // NOLINT
    {
        return engine.Get();
    }

    World* RuntimeModule::CreateWorld(const std::string& inName) const // NOLINT
    {
        return new World(inName);
    }

    void RuntimeModule::DestroyWorld(World* inWorld) const // NOLINT
    {
        delete inWorld;
    }

    void RuntimeModule::CreateEngine()
    {
#if BUILD_EDITOR
        engine = Common::UniqueRef<Engine>(new EditorEngine({}));
#else
        // TODO load game's main dynamic module to create custom engine
        engine = Common::UniqueRef<Engine>(new GameEngine({}));
#endif
    }
}

IMPLEMENT_MODULE(RUNTIME_API, Runtime::RuntimeModule);
