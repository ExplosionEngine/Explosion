//
// Created by johnk on 2024/8/22.
//

#include <RuntimeTestModule.h>

void RuntimeTestModule::OnUnload()
{
    Runtime::EngineHolder::Unload();
}

Core::ModuleType RuntimeTestModule::Type() const
{
    return Core::ModuleType::mStatic;
}

Runtime::Engine* RuntimeTestModule::CreateEngine(const Runtime::EngineInitParams& inParams)
{
    return new Runtime::MinEngine(inParams);
}

IMPLEMENT_STATIC_MODULE(RuntimeTest, "RuntimeTest", RuntimeTestModule)
