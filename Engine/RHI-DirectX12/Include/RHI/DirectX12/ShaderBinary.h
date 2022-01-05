//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_SHADER_BINARY_H
#define EXPLOSION_RHI_DX12_SHADER_BINARY_H

#include <d3d12.h>
#include <wrl/client.h>

#include <RHI/ShaderBinary.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12ShaderBinary : public ShaderBinary {
    public:
        NON_COPYABLE(DX12ShaderBinary)
        explicit DX12ShaderBinary(const ShaderCompileInfo* compileInfo);
        ~DX12ShaderBinary() override;

    private:
        void CreateShader(const ShaderCompileInfo* compileInfo);

        ComPtr<ID3DBlob> dx12Shader;
    };
}

#endif //EXPLOSION_RHI_DX12_SHADER_BINARY_H
