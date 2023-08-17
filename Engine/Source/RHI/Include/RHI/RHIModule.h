//
// Created by johnk on 2023/8/7.
//

#pragma once

#include <Core/Module.h>
#include <RHI/Instance.h>

namespace RHI {
    class RHIModule : public Core::Module {
    public:
        virtual ~RHIModule() override;
        virtual Instance* GetRHIInstance() = 0;

    protected:
        RHIModule();
    };
}
