//
// Created by johnk on 2023/8/7.
//

#include <RHI/Dummy/DummyRHIModule.h>
#include <RHI/Dummy/Instance.h>

namespace RHI::Dummy {
    DummyRHIModule::DummyRHIModule() = default;

    DummyRHIModule::~DummyRHIModule() = default;

    void DummyRHIModule::OnLoad()
    {
        gInstance = new DummyInstance();
    }

    void DummyRHIModule::OnUnload()
    {
        delete gInstance;
    }

    Core::ModuleType DummyRHIModule::Type() const
    {
        return Core::ModuleType::mDynamic;
    }

    Instance* DummyRHIModule::GetRHIInstance() // NOLINT
    {
        return gInstance;
    }
}

IMPLEMENT_DYNAMIC_MODULE(RHI_DUMMY_API, RHI::Dummy::DummyRHIModule);
