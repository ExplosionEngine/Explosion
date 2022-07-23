//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <Common/Concurrent.h>
#include <Common/Platform.h>

namespace Shader {
    struct ShaderCompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
        bool withDebugInfo;
        bool spriv;
    };

    struct ShaderCompileOutput {
        bool success;
        std::vector<uint8_t> byteCode;
        std::string errorInfo;
    };

    class ShaderCompiler {
    public:
        static ShaderCompiler& Get();
        ~ShaderCompiler();
        std::future<ShaderCompileOutput> Compile(const ShaderCompileInput& compileInfo);

    private:
        ShaderCompiler();

        Common::ThreadPool threadPool;
    };
}
