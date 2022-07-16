//
// Created by johnk on 2022/7/16.
//

#include <Shader/ShaderCompiler.h>

namespace Shader {
    ShaderCompiler& ShaderCompiler::Get()
    {
        static ShaderCompiler instance;
        return instance;
    }

    ShaderCompiler::ShaderCompiler() = default;

    ShaderCompiler::~ShaderCompiler() = default;

    void ShaderCompiler::Compile(std::vector<uint8_t>& byteCode, const std::string& source, const std::string& entryPoint, RHI::ShaderStageBits stage)
    {
        // TODO
    }
}
