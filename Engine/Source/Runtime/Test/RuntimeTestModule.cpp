//
// Created by johnk on 2024/8/22.
//

#include <RuntimeTestModule.h>

void RuntimeTestModule::OnUnload()
{
    Runtime::EngineHolder::Unload();
    engine.Reset();
}

Core::ModuleType RuntimeTestModule::Type() const
{
    return Core::ModuleType::mStatic;
}

Runtime::Engine* RuntimeTestModule::CreateEngine(const Runtime::EngineInitParams& inParams)
{
    engine = new Runtime::MinEngine(inParams);
    return engine.Get();
}

IMPLEMENT_STATIC_MODULE(RuntimeTest, "RuntimeTest", RuntimeTestModule)
