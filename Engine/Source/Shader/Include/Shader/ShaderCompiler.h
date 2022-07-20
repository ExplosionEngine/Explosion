//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <Common/Concurrent.h>

namespace Shader {
    struct ShaderCompileInfo {
        std::string shaderName;
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
        bool debugInfo;
    };

    class ShaderCompiler {
    public:
        static ShaderCompiler& Get();
        ~ShaderCompiler();
        std::future<std::vector<uint8_t>> Compile(const ShaderCompileInfo& compileInfo);

    private:
        ShaderCompiler();

        Common::ThreadPool threadPool;
    };
}
