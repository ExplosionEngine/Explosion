//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <Core/Module.h>
#include <RHI/RHIModule.h>
#include <RHI/Dummy/Api.h>

namespace RHI::Dummy {
    class RHI_DUMMY_API DummyRHIModule : public RHIModule {
    public:
        DummyRHIModule();
        ~DummyRHIModule() override;

        Instance* GetRHIInstance() override;
    };
}

IMPLEMENT_MODULE(RHI_DUMMY_API, RHI::Dummy::DummyRHIModule);
