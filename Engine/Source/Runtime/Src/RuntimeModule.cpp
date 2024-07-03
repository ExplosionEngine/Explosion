//
// Created by johnk on 2023/9/1.
//

#include <Runtime/RuntimeModule.h>
#include <Common/Debug.h>

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
