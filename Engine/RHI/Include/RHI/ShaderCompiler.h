//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_SHADER_COMPILER_H
#define EXPLOSION_RHI_SHADER_COMPILER_H

#include <Common/Utility.h>

namespace RHI {
    class ShaderCompiler {
    public:
        NON_COPYABLE(ShaderCompiler)
        virtual ~ShaderCompiler();

    protected:
        ShaderCompiler();
    };
}

#endif //EXPLOSION_RHI_SHADER_COMPILER_H
