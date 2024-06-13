//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <RHI/RHIModule.h>
#include <RHI/Dummy/Api.h>

namespace RHI::Dummy {
    class RHI_DUMMY_API DummyRHIModule final : public RHIModule {
    public:
        DummyRHIModule();
        ~DummyRHIModule() override;

        void OnLoad() override;
        void OnUnload() override;
        Instance* GetRHIInstance() override;
    };
}
