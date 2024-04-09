//
// Created by johnk on 16/3/2022.
//

#include <RHI/DirectX12/ShaderModule.h>

namespace RHI::DirectX12 {
    DX12ShaderModule::DX12ShaderModule(const ShaderModuleCreateInfo& inCreateInfo)
        : ShaderModule(inCreateInfo)
        , nativeShaderBytecode(inCreateInfo.byteCode, inCreateInfo.size)
        , entryPoint(inCreateInfo.entryPoint)
    {
    }

    DX12ShaderModule::~DX12ShaderModule() = default;

    const std::string& DX12ShaderModule::GetEntryPoint()
    {
        return entryPoint;
    }

    void DX12ShaderModule::Destroy()
    {
        delete this;
    }

    const D3D12_SHADER_BYTECODE& DX12ShaderModule::GetNative() const
    {
        return nativeShaderBytecode;
    }
}
