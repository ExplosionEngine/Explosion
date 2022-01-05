//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_SHADER_COMPILER_H
#define EXPLOSION_RHI_SHADER_COMPILER_H

#include <Common/Utility.h>

namespace RHI {
    class ShaderBinary;
    struct ShaderCompileInfo;

    class ShaderCompiler {
    public:
        NON_COPYABLE(ShaderCompiler)
        virtual ~ShaderCompiler();

        virtual ShaderBinary* CompileHLSL(const ShaderCompileInfo* compileInfo) = 0;
        virtual void DestroyShaderBinary(ShaderBinary* binary) = 0;

    protected:
        ShaderCompiler();
    };
}

#endif //EXPLOSION_RHI_SHADER_COMPILER_H
