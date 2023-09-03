//
// Created by johnk on 2023/8/7.
//

#include <RHI/Dummy/DummyRHIModule.h>
#include <RHI/Dummy/Instance.h>

namespace RHI::Dummy {
    DummyRHIModule::DummyRHIModule() = default;

    DummyRHIModule::~DummyRHIModule() = default;

    Instance* DummyRHIModule::GetRHIInstance() // NOLINT
    {
        return RHIGetInstance();
    }
}

IMPLEMENT_MODULE(RHI_DUMMY_API, RHI::Dummy::DummyRHIModule);
