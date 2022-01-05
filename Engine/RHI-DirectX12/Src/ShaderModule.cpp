//
// Created by johnk on 5/1/2022.
//

#include <RHI/DirectX12/ShaderModule.h>

namespace RHI::DirectX12 {
    DX12ShaderModule::DX12ShaderModule(const ShaderModuleCreateInfo* createInfo) : ShaderModule(createInfo) {}

    DX12ShaderModule::~DX12ShaderModule() = default;
}
