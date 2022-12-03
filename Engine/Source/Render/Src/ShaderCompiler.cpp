//
// Created by johnk on 2022/7/16.
//

#include <unordered_map>

#if PLATFORM_WINDOWS
#include <Windows.h>
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

#include <Render/ShaderCompiler.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_msl.hpp>

namespace Render {
    static std::wstring GetDXCTargetProfile(RHI::ShaderStageBits stage)
    {
        static const std::unordered_map<RHI::ShaderStageBits, std::wstring> map = {
            { RHI::ShaderStageBits::VERTEX, L"vs" },
            { RHI::ShaderStageBits::FRAGMENT, L"ps" },
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

    static std::vector<std::wstring> GetDefinitions(const ShaderCompileOptions& options)
    {
        std::vector<std::wstring> result;
        for (const auto& definition : options.definitions) {
            result.emplace_back(L"-D");
            result.emplace_back(Common::StringUtils::ToWideString(definition));
        }
        return result;
    }

    static void FillDefinitionsToDXCArguments(std::vector<LPCWSTR>& result, const std::vector<std::wstring>& definitions)
    {
        for (const auto& definition : definitions) {
            result.emplace_back(definition.c_str());
        }
    }

    static void CompileDxilOrSpriv(
        const ShaderCompileInput& input,
        const ShaderCompileOptions& options,
        ShaderCompileOutput& output)
    {
        ComPtr<IDxcLibrary> library;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library))));

        ComPtr<IDxcCompiler> compiler;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))));

        ComPtr<IDxcUtils> utils;
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))));

        ComPtr<IDxcBlobEncoding> source;
        utils->CreateBlobFromPinned(input.source.c_str(), std::strlen(input.source.c_str()), CP_UTF8, &source);

        std::vector<LPCWSTR> arguments = GetDXCBaseArguments(options);
        auto definitions = GetDefinitions(options);
        FillDefinitionsToDXCArguments(arguments, definitions);

        ComPtr<IDxcOperationResult> result;
        HRESULT success = compiler->Compile(
            ComPtrGet(source),
            nullptr,
            Common::StringUtils::ToWideString(input.entryPoint).c_str(),
            GetDXCTargetProfile(input.stage).c_str(),
            arguments.data(),
            static_cast<uint32_t>(arguments.size()),
            nullptr,
            0,
            nullptr,
            &result);

        if (SUCCEEDED(success)) {
            result->GetStatus(&success);
        }
        if (FAILED(success)) {
            ComPtr<IDxcBlobEncoding> errorBlob;
            Assert(SUCCEEDED(result->GetErrorBuffer(&errorBlob)));

            output.success = false;
            output.errorInfo.resize(errorBlob->GetBufferSize());
            memcpy(output.errorInfo.data(), errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            return;
        }
        Assert(SUCCEEDED(success));

        ComPtr<IDxcBlob> code;
        Assert(SUCCEEDED(result->GetResult(&code)));

        output.success = true;
        const auto* codeStart = static_cast<const uint8_t*>(code->GetBufferPointer());
        const auto* codeEnd = codeStart + code->GetBufferSize();
        output.byteCode = std::vector<uint8_t>(codeStart, codeEnd);
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
