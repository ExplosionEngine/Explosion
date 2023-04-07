//
// Created by johnk on 2023/3/21.
//

#include <RHI/Dummy/ShaderModule.h>

namespace RHI::Dummy {
    DummyShaderModule::DummyShaderModule(const ShaderModuleCreateInfo& createInfo)
        : ShaderModule(createInfo)
    {
    }

    DummyShaderModule::~DummyShaderModule() = default;

    void DummyShaderModule::Destroy()
    {
        delete this;
    }
}
