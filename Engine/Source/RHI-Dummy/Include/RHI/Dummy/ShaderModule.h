//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/ShaderModule.h>

namespace RHI::Dummy {
    class DummyShaderModule : public ShaderModule {
    public:
        explicit DummyShaderModule(const ShaderModuleCreateInfo& createInfo);
        ~DummyShaderModule() override;

        void Destroy() override;
    };
}
