//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <Common/Concurrent.h>

namespace Shader {
    enum class ByteCodeType {
        DXIL,
        SPRIV,
        MBC,
        MAX
    };

    struct CompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
    };

    struct CompileOptions {
        ByteCodeType byteCodeType = ByteCodeType::MAX;
        bool withDebugInfo = false;
    };

    struct CompileOutput {
        bool success;
        std::vector<uint8_t> byteCode;
        std::string errorInfo;
    };

    class ShaderCompiler {
    public:
        static ShaderCompiler& Get();
        ~ShaderCompiler();
        std::future<CompileOutput> Compile(const CompileInput& inInput, const CompileOptions& inOptions);

    private:
        ShaderCompiler();

        Common::ThreadPool threadPool;
    };
}
