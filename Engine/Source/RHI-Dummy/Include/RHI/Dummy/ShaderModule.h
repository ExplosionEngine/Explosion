//
// Created by johnk on 2023/3/21.
//

#pragma once

#include <RHI/ShaderModule.h>

namespace RHI::Dummy {
    class DummyShaderModule : public ShaderModule {
    public:
        NonCopyable(DummyShaderModule)
        explicit DummyShaderModule(const ShaderModuleCreateInfo& createInfo);
        ~DummyShaderModule() override;

        const std::string & GetEntryPoint() override;

    private:
        std::string entryPoint;
    };
}
