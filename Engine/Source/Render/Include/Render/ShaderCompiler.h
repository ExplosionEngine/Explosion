//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <Common/Concurrent.h>

namespace Render {
    enum class ShaderByteCodeType {
        DXIL,
        SPRIV,
        MBC,
        MAX
    };

    struct ShaderCompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
    };

    struct ShaderCompileOptions {
        ShaderByteCodeType byteCodeType = ShaderByteCodeType::MAX;
        bool withDebugInfo = false;
        std::vector<std::string> definitions;
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
        std::future<ShaderCompileOutput> Compile(const ShaderCompileInput& inInput, const ShaderCompileOptions& inOptions);

    private:
        ShaderCompiler();

        Common::ThreadPool threadPool;
    };
}
