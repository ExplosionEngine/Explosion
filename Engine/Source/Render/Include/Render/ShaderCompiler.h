//
// Created by johnk on 2022/7/15.
//

#pragma once

#include <vector>
#include <string>

#include <RHI/Common.h>
#include <Render/Shader.h>
#include <Common/Concurrent.h>

namespace Render {
    enum class ShaderByteCodeType : uint8_t {
        dxil,
        spirv,
        max
    };

    struct ShaderCompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage = RHI::ShaderStageBits::max;
        std::vector<std::string> definitions;
        std::vector<std::string> includeDirectories;
    };

    struct ShaderCompileOptions {
        std::vector<std::string> includeDirectories;
        ShaderByteCodeType byteCodeType = ShaderByteCodeType::max;
        bool withDebugInfo = false;
    };

    struct ShaderCompileOutput {
        bool success;
        std::string entryPoint;
        std::vector<uint8_t> byteCode;
        ShaderReflectionData reflectionData;
        std::string errorInfo;
    };

    struct ShaderTypeAndVariantHashProvider {
        size_t operator()(const std::pair<ShaderTypeKey, ShaderVariantKey>& value) const;
    };

    struct ShaderTypeCompileResult {
        bool success;
        std::unordered_map<std::pair<ShaderTypeKey, ShaderVariantKey>, std::string, ShaderTypeAndVariantHashProvider> errorInfos;
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

    class ShaderTypeCompiler {
    public:
        static ShaderTypeCompiler& Get();
        ~ShaderTypeCompiler();

        std::future<ShaderTypeCompileResult> Compile(const std::vector<const ShaderType*>& inShaderTypes, const ShaderCompileOptions& inOptions);
        std::future<ShaderTypeCompileResult> CompileAll(const ShaderCompileOptions& inOptions);

    private:
        ShaderTypeCompiler();

        Common::ThreadPool threadPool;
    };
}
