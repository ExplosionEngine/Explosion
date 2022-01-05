//
// Created by johnk on 5/1/2022.
//

#include <d3dcompiler.h>

#include <RHI/DirectX12/ShaderBinary.h>
#include <RHI/DirectX12/Enum.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    DX12ShaderBinary::DX12ShaderBinary(const ShaderCompileInfo* compileInfo) : ShaderBinary(compileInfo)
    {
        CreateShader(compileInfo);
    }

    DX12ShaderBinary::~DX12ShaderBinary() = default;

    void DX12ShaderBinary::CreateShader(const ShaderCompileInfo* compileInfo)
    {
        ThrowIfFailed(
            D3DCompileFromFile(
                reinterpret_cast<LPCWSTR>(compileInfo->path.c_str()),
                nullptr,
                nullptr,
                compileInfo->entry.c_str(),
                GetShaderStageTargetString(compileInfo->stage).c_str(),
                FlagsCast<ShaderCompileBits>(compileInfo->flags),
                0,
                &dx12Shader,
                nullptr
            ),
            "failed to create dx12 shader"
        );
    }
}
