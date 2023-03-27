//
// Created by johnk on 2022/7/16.
//

#include <unordered_map>

#if PLATFORM_WINDOWS
#include <Windows.h>
#undef min
#undef max
#else
#define __EMULATE_UUID 1
#endif
#include <dxc/dxcapi.h>

#if PLATFORM_WINDOWS
#include <wrl/client.h>
using namespace Microsoft::WRL;
#define ComPtrGet(name) name.Get()
#else
// defined in dxc WinAdapter.h
#define ComPtr CComPtr
#define ComPtrGet(name) name
#endif

#include <d3d12shader.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_msl.hpp>

#include <Render/ShaderCompiler.h>
#include <Common/Debug.h>
#include <Common/String.h>

namespace Render {
    static std::wstring GetDXCTargetProfile(RHI::ShaderStageBits stage)
    {
        static const std::unordered_map<RHI::ShaderStageBits, std::wstring> map = {
            { RHI::ShaderStageBits::S_VERTEX, L"vs" },
            { RHI::ShaderStageBits::S_PIXEL, L"ps" },
            // TODO
        };
        auto iter = map.find(stage);
        Assert(iter != map.end());
        return iter->second + L"_6_2";
    }

    static std::vector<LPCWSTR> GetDXCBaseArguments(const ShaderCompileOptions& options)
    {
        static std::vector<LPCWSTR> basicArguments = {
            DXC_ARG_WARNINGS_ARE_ERRORS,
            DXC_ARG_PACK_MATRIX_ROW_MAJOR
        };

        std::vector<LPCWSTR> result = basicArguments;
        if (options.withDebugInfo) {
            result.emplace_back(L"-Qembed_debug");
            result.emplace_back(DXC_ARG_DEBUG);
        }
        if (options.byteCodeType != ShaderByteCodeType::DXIL) {
            result.emplace_back(L"-spirv");
        }
        return result;
    }

    static std::vector<std::wstring> GetEntryPointArguments(const ShaderCompileInput& input)
    {
        return {
            L"-E",
            Common::StringUtils::ToWideString(input.entryPoint)
        };
    }

    static std::vector<std::wstring> GetTargetProfileArguments(const ShaderCompileInput& input)
    {
        return {
            L"-T",
            GetDXCTargetProfile(input.stage)
        };
    }

    static std::vector<std::wstring> GetIncludePathArguments(const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result;
        for (const auto& includePath : options.includePaths) {
            result.emplace_back(L"-I");
            result.emplace_back(Common::StringUtils::ToWideString(includePath));
        }
        return result;
    }

    static std::vector<std::wstring> GetDefinitionArguments(const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result;
        for (const auto& definition : options.definitions) {
            result.emplace_back(L"-D");
            result.emplace_back(Common::StringUtils::ToWideString(definition));
        }
        return result;
    }

    static void FillArguments(std::vector<LPCWSTR>& result, const std::vector<std::wstring>& arguments)
    {
        for (const auto& argument : arguments) {
            result.emplace_back(argument.c_str());
        }
    }

    static void BuildHlslReflectionData(ComPtr<ID3D12ShaderReflection>& shaderReflection, ShaderReflectionData& result)
    {
        D3D12_SHADER_DESC shaderDesc;
        shaderReflection->GetDesc(&shaderDesc);

        // TODO
    }

    static void BuildGlslReflectionData(const spirv_cross::Compiler& compiler, ShaderReflectionData& result)
    {
        // TODO
    }

    static void CompileDxilOrSpriv(
        const ShaderCompileInput& input,
        const ShaderCompileOptions& options,
        ShaderCompileOutput& output)
    {
        ComPtr<IDxcLibrary> library;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library))));

        ComPtr<IDxcCompiler3> compiler;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))));

        ComPtr<IDxcUtils> utils;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))));

        ComPtr<IDxcIncludeHandler> includeHandler;
        Assert(SUCCEEDED(utils->CreateDefaultIncludeHandler(&includeHandler)));

        ComPtr<IDxcBlobEncoding> source;
        utils->CreateBlobFromPinned(input.source.c_str(), std::strlen(input.source.c_str()), CP_UTF8, &source);

        std::vector<LPCWSTR> arguments = GetDXCBaseArguments(options);
        auto entryPointArgs = GetEntryPointArguments(input);
        auto targetProfileArgs = GetTargetProfileArguments(input);
        auto includePathArgs = GetIncludePathArguments(options);
        auto definitionArgs = GetDefinitionArguments(options);
        FillArguments(arguments, entryPointArgs);
        FillArguments(arguments, targetProfileArgs);
        FillArguments(arguments, includePathArgs);
        FillArguments(arguments, definitionArgs);

        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = source->GetBufferPointer();
        sourceBuffer.Size = source->GetBufferSize();
        sourceBuffer.Encoding = 0u;

        ComPtr<IDxcResult> result;
        const HRESULT operationResult = compiler->Compile(
            &sourceBuffer,
            arguments.data(),
            arguments.size(),
            includeHandler.Get(),
            IID_PPV_ARGS(&result));

        ComPtr<IDxcBlobEncoding> errorBlob;
        Assert(SUCCEEDED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), nullptr)));

        if (FAILED(operationResult) || errorBlob->GetBufferSize() > 0) {
            output.success = false;
            output.errorInfo.resize(errorBlob->GetBufferSize());
            memcpy(output.errorInfo.data(), errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            return;
        }

        ComPtr<IDxcBlob> codeBlob;
        Assert(SUCCEEDED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&codeBlob), nullptr)));

        output.success = true;
        const auto* codeStart = static_cast<const uint8_t*>(codeBlob->GetBufferPointer());
        const auto* codeEnd = codeStart + codeBlob->GetBufferSize();
        output.byteCode = std::vector<uint8_t>(codeStart, codeEnd);

        if (options.byteCodeType == ShaderByteCodeType::DXIL) {
            ComPtr<IDxcBlob> reflectionBlob;
            Assert(SUCCEEDED(result->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr)));

            DxcBuffer reflectionBuffer;
            reflectionBuffer.Ptr = reflectionBlob->GetBufferPointer();
            reflectionBuffer.Size = reflectionBlob->GetBufferSize();
            reflectionBuffer.Encoding = 0u;

            ComPtr<ID3D12ShaderReflection> shaderReflection;
            utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection));
            BuildHlslReflectionData(shaderReflection, output.reflectionData);
        } else {
            spirv_cross::Compiler sprivCrossCompiler(reinterpret_cast<const uint32_t*>(output.byteCode.data()), output.byteCode.size() * sizeof(uint8_t) / sizeof(uint32_t));
            BuildGlslReflectionData(sprivCrossCompiler, output.reflectionData);
        }
    }

    static void ConvertSprivToMetalByteCode(
        const ShaderCompileOptions& options,
        ShaderCompileOutput& output)
    {
        spirv_cross::CompilerMSL compiler(reinterpret_cast<const uint32_t*>(output.byteCode.data()), output.byteCode.size() / sizeof(uint32_t));
        spirv_cross::CompilerMSL::Options mslOptions;
        mslOptions.platform = spirv_cross::CompilerMSL::Options::Platform::macOS;
        mslOptions.enable_decoration_binding = true;
        mslOptions.pad_fragment_output_components = true;
        compiler.set_msl_options(mslOptions);

        std::string source = compiler.compile();
        output.byteCode.resize(source.length() + 1, 0);
        memcpy(output.byteCode.data(), source.c_str(), source.length());
    }
}

namespace Render {
    ShaderCompiler& ShaderCompiler::Get()
    {
        static ShaderCompiler instance;
        return instance;
    }

    ShaderCompiler::ShaderCompiler() : threadPool("ShaderCompiler", 16)
    {
    }

    ShaderCompiler::~ShaderCompiler() = default;

    std::future<ShaderCompileOutput> ShaderCompiler::Compile(const ShaderCompileInput& inInput, const ShaderCompileOptions& inOptions)
    {
        return threadPool.EmplaceTask([](const ShaderCompileInput& input, const ShaderCompileOptions& options) -> ShaderCompileOutput {
            ShaderCompileOutput output;
            CompileDxilOrSpriv(input, options, output);
            if (!output.success || options.byteCodeType != ShaderByteCodeType::MBC) {
                return output;
            }
            ConvertSprivToMetalByteCode(options, output);
            return output;
        }, inInput, inOptions);
    }
}
