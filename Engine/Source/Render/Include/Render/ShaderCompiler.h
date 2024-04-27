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
        max
    };

    struct ShaderCompileInput {
        std::string source;
        std::string entryPoint;
        RHI::ShaderStageBits stage;
        std::vector<std::string> definitions;
    };

    struct ShaderCompileOptions {
        ShaderByteCodeType byteCodeType = ShaderByteCodeType::max;
        bool withDebugInfo = false;
        std::vector<std::string> includePaths;
    };

    struct ShaderCompileOutput {
        bool success;
        std::vector<uint8_t> byteCode;
        ShaderReflectionData reflectionData;
        std::string errorInfo;
    };

    struct ShaderTypeAndVariantHashProvider {
        size_t operator()(const std::pair<ShaderTypeKey, VariantKey>& value) const;
    };

    struct ShaderTypeCompileResult {
        bool success;
        std::unordered_map<std::pair<ShaderTypeKey, VariantKey>, std::string, ShaderTypeAndVariantHashProvider> errorInfos;
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

        std::future<ShaderTypeCompileResult> Compile(const std::vector<IShaderType*>& inShaderTypes, const ShaderCompileOptions& inOptions);
        std::future<ShaderTypeCompileResult> CompileGlobalShaderTypes(const ShaderCompileOptions& inOptions);

    private:
        ShaderTypeCompiler();

        Common::ThreadPool threadPool;
    };
}
