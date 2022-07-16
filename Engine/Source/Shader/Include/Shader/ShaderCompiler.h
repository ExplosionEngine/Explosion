//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>

namespace Shader {
    class ShaderCompiler {
    public:
        static ShaderCompiler& Get();

        ~ShaderCompiler();

        void Compile(std::vector<uint8_t>& byteCode, const std::string& source, const std::string& entryPoint, RHI::ShaderStageBits stage);

    private:
        ShaderCompiler();
    };
}
