//
// Created by johnk on 16/3/2022.
//

#pragma once

#include <d3d12.h>
#include <directx/d3dx12.h>

#include <RHI/ShaderModule.h>

namespace RHI::DirectX12 {
    class DX12ShaderModule final : public ShaderModule {
    public:
        NonCopyable(DX12ShaderModule)
        explicit DX12ShaderModule(const ShaderModuleCreateInfo& inCreateInfo);
        ~DX12ShaderModule() override;

        const std::string& GetEntryPoint() override;

        const D3D12_SHADER_BYTECODE& GetNative() const;

    private:
        CD3DX12_SHADER_BYTECODE nativeShaderBytecode;
        std::string entryPoint;
    };
}
