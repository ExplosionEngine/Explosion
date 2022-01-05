//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_SHADER_MODULE_H
#define EXPLOSION_RHI_DX12_SHADER_MODULE_H

#include <RHI/ShaderModule.h>

namespace RHI::DirectX12 {
    class DX12ShaderModule : public ShaderModule {
    public:
        NON_COPYABLE(DX12ShaderModule)
        explicit DX12ShaderModule(const ShaderModuleCreateInfo* createInfo);
        ~DX12ShaderModule() override;
    };
}

#endif //EXPLOSION_RHI_DX12_SHADER_MODULE_H
