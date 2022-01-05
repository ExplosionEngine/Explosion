//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_SHADER_COMPILER_H
#define EXPLOSION_RHI_DX12_SHADER_COMPILER_H

#include <RHI/ShaderCompiler.h>

namespace RHI::DirectX12 {
    class DX12ShaderCompiler : public ShaderCompiler {
    public:
        static DX12ShaderCompiler& Get();

        NON_COPYABLE(DX12ShaderCompiler)
        ~DX12ShaderCompiler() override;

        ShaderBinary* CompileHLSL(const ShaderCompileInfo* compileInfo) override;
        void DestroyShaderBinary(ShaderBinary* binary) override;

    private:
        DX12ShaderCompiler();
    };
}

#endif //EXPLOSION_RHI_DX12_SHADER_COMPILER_H
