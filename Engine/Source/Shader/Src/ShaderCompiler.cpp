//
// Created by johnk on 2022/7/16.
//

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

#include <Shader/ShaderCompiler.h>
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

    static std::vector<LPCWSTR> GetDXCArguments(const ShaderCompileInfo& compileInfo)
    {
        static std::vector<LPCWSTR> basicArguments = {
#if !PLATFORM_WINDOWS
            L"-spirv",
#endif
            DXC_ARG_WARNINGS_ARE_ERRORS,
            DXC_ARG_PACK_MATRIX_ROW_MAJOR
        };

        std::vector<LPCWSTR> result = basicArguments;
        if (compileInfo.debugInfo) {
            result.emplace_back(L"-Qembed_debug");
            result.emplace_back(DXC_ARG_DEBUG);
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

    std::future<std::vector<uint8_t>> ShaderCompiler::Compile(const ShaderCompileInfo& compileInfo)
    {
        return threadPool.EmplaceTask([](ShaderCompileInfo info) -> std::vector<uint8_t> {
            ComPtr<IDxcLibrary> library;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library))));

            ComPtr<IDxcCompiler> compiler;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))));

            ComPtr<IDxcUtils> utils;
            Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))));

            ComPtr<IDxcBlobEncoding> source;
            std::wstring utf8Source = Common::StringUtils::ToWideString(info.source);
            utils->CreateBlobFromPinned(utf8Source.data(), utf8Source.size(), CP_UTF8, &source);

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
                std::cout << "failed to compiler shader (" << info.shaderName << ", " << info.entryPoint << ")" << std::endl
                          << static_cast<const char*>(errorInfo->GetBufferPointer()) << std::endl;
            }
            Assert(SUCCEEDED(success));

            ComPtr<IDxcBlob> code;
            Assert(SUCCEEDED(result->GetResult(&code)));

            const auto* codeStart = static_cast<const uint8_t*>(code->GetBufferPointer());
            const auto* codeEnd = codeStart + code->GetBufferSize();
            std::vector<uint8_t> byteCode(codeStart, codeEnd);
            return byteCode;
        }, compileInfo);
    }
}
