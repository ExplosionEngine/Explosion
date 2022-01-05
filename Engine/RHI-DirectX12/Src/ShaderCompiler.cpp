//
// Created by johnk on 5/1/2022.
//

#include <RHI/DirectX12/ShaderCompiler.h>

namespace RHI::DirectX12 {
    DX12ShaderCompiler& DX12ShaderCompiler::Get()
    {
        static DX12ShaderCompiler instance;
        return instance;
    }

    DX12ShaderCompiler::DX12ShaderCompiler() = default;

    DX12ShaderCompiler::~DX12ShaderCompiler() = default;
}
