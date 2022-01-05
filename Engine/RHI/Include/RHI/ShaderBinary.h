//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_SHADER_BINARY_H
#define EXPLOSION_RHI_SHADER_BINARY_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct ShaderCompileInfo {
        std::string path;
        std::string entry;
        ShaderStage stage;
        ShaderCompileFlags flags;
    };

    class ShaderBinary {
    public:
        NON_COPYABLE(ShaderBinary)
        virtual ~ShaderBinary();

    protected:
        explicit ShaderBinary(const ShaderCompileInfo* compileInfo);
    };
}

#endif //EXPLOSION_RHI_SHADER_BINARY_H
