//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <RHI/BindGroupLayout.h>
#include <Render/Shader.h>
#include <Common/Concurrent.h>

namespace Render {
    enum class ShaderByteCodeType {
        dxil,
        spirv,
        mbc,
        max
    };

    struct ShaderCompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
    };

    struct ShaderCompileOptions {
        ShaderByteCodeType byteCodeType = ShaderByteCodeType::max;
        bool withDebugInfo = false;
        std::vector<std::string> definitions;
        std::vector<std::string> includePaths;
    };

    struct ShaderCompileOutput {
        bool success;
        std::vector<uint8_t> byteCode;
        ShaderReflectionData reflectionData;
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
