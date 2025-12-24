#include <string>
#include <vector>
#include <iostream>

#if PLATFORM_WINDOWS
#include <windows.h>
#include <dxc/d3d12shader.h>
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

int main(void) {
    const std::string shaderSource = "[numthreads(8, 8, 1)] void CSMain() {  }";

    ComPtr<IDxcLibrary> library;
    if (FAILED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)))) {
        std::cout << "failed to create dxc library" << std::endl;
        return 1;
    }

    ComPtr<IDxcCompiler3> compiler;
    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)))) {
        std::cout << "failed to create dxc compiler" << std::endl;
        return 1;
    }

    ComPtr<IDxcUtils> utils;
    if (FAILED(SUCCEEDED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))))) {
        std::cout << "failed to create dxc utils" << std::endl;
        return 1;
    }

    ComPtr<IDxcIncludeHandler> includeHandler;
    if (FAILED(SUCCEEDED(utils->CreateDefaultIncludeHandler(&includeHandler)))) {
        std::cout << "failed to create dxc include handler" << std::endl;
        return 1;
    }

    ComPtr<IDxcBlobEncoding> source;
    if (FAILED(utils->CreateBlobFromPinned(shaderSource.c_str(), std::strlen(shaderSource.c_str()), CP_UTF8, &source))) {
        std::cout << "failed to create dxc blob encoding" << std::endl;
        return 1;
    }

    std::vector<LPCWSTR> arguments = {
        DXC_ARG_WARNINGS_ARE_ERRORS,
        DXC_ARG_PACK_MATRIX_ROW_MAJOR,
        L"-E", L"CSMain",
        L"-T", L"cs_6_2"
    };

    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = source->GetBufferPointer();
    sourceBuffer.Size = source->GetBufferSize();
    sourceBuffer.Encoding = 0u;

    ComPtr<IDxcResult> result;
    const HRESULT operationResult = compiler->Compile(
        &sourceBuffer,
        arguments.data(),
        arguments.size(),
        ComPtrGet(includeHandler),
        IID_PPV_ARGS(&result));

    ComPtr<IDxcBlobEncoding> errorBlob;
    if (FAILED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBlob), nullptr))) {
        std::cout << "failed to get error info" << std::endl;
        return 1;
    }

    if (FAILED(operationResult) || errorBlob->GetBufferSize() > 0) {
        std::string error(static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
        std::cout << "dxc compile error: " << error << std::endl;
        return 1;
    }
    return 0;
}
