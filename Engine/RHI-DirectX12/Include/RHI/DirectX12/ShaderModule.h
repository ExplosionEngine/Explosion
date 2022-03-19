//
// Created by johnk on 16/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_SHADER_MODULE_H
#define EXPLOSION_RHI_DX12_SHADER_MODULE_H

#include <d3d12.h>
#include <directx/d3dx12.h>

#include <RHI/ShaderModule.h>

namespace RHI::DirectX12 {
    class DX12ShaderModule : public ShaderModule {
    public:
        explicit DX12ShaderModule(const ShaderModuleCreateInfo* createInfo);
        ~DX12ShaderModule() override;

        void Destroy() override;

        const D3D12_SHADER_BYTECODE& GetDX12ShaderBytecode() const;

    private:
        CD3DX12_SHADER_BYTECODE dx12ShaderBytecode;
    };
}

#endif//EXPLOSION_RHI_DX12_SHADER_MODULE_H
