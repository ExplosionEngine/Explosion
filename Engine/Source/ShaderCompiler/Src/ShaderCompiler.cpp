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

#include <ShaderCompiler/ShaderCompiler.h>
#include <Common/Debug.h>
#include <Common/String.h>

namespace Shader {
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

    static std::vector<LPCWSTR> GetDXCArguments(const ShaderCompileInput& compileInfo)
    {
        static std::vector<LPCWSTR> basicArguments = {
            DXC_ARG_WARNINGS_ARE_ERRORS,
            DXC_ARG_PACK_MATRIX_ROW_MAJOR
        };

        std::vector<LPCWSTR> result = basicArguments;
        if (compileInfo.withDebugInfo) {
            result.emplace_back(L"-Qembed_debug");
            result.emplace_back(DXC_ARG_DEBUG);
        }
        if (compileInfo.spriv) {
            result.emplace_back(L"-spirv");
        }
        return result;
    }
}

namespace Shader {
    ShaderCompiler& ShaderCompiler::Get()
    {
        static ShaderCompiler instance;
        return instance;
    }

    ShaderCompiler::ShaderCompiler() : threadPool("ShaderCompiler", 16)
    {
    }

    ShaderCompiler::~ShaderCompiler() = default;

    std::future<ShaderCompileOutput> ShaderCompiler::Compile(const ShaderCompileInput& compileInfo)
    {
        return threadPool.EmplaceTask([](const ShaderCompileInput& info) -> ShaderCompileOutput {
            ComPtr<IDxcLibrary> library;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library))));

            ComPtr<IDxcCompiler> compiler;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))));

            ComPtr<IDxcUtils> utils;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))));

            ComPtr<IDxcBlobEncoding> source;
            utils->CreateBlobFromPinned(info.source.c_str(), std::strlen(info.source.c_str()), CP_UTF8, &source);

            std::vector<LPCWSTR> arguments = GetDXCArguments(info);
            ComPtr<IDxcOperationResult> result;
            HRESULT success = compiler->Compile(
                ComPtrGet(source),
                nullptr,
                Common::StringUtils::ToWideString(info.entryPoint).c_str(),
                GetDXCTargetProfile(info.stage).c_str(),
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
                ComPtr<IDxcBlobEncoding> errorInfo;
                Assert(SUCCEEDED(result->GetErrorBuffer(&errorInfo)));

                ShaderCompileOutput output;
                output.success = false;
                output.errorInfo.resize(errorInfo->GetBufferSize());
                memcpy(output.errorInfo.data(), errorInfo->GetBufferPointer(), errorInfo->GetBufferSize());
                return output;
            }
            Assert(SUCCEEDED(success));

            ComPtr<IDxcBlob> code;
            Assert(SUCCEEDED(result->GetResult(&code)));

            const auto* codeStart = static_cast<const uint8_t*>(code->GetBufferPointer());
            const auto* codeEnd = codeStart + code->GetBufferSize();
            ShaderCompileOutput output;
            output.success = true;
            output.byteCode = std::vector<uint8_t>(codeStart, codeEnd);
            return output;
        }, compileInfo);
    }
}
