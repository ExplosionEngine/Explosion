//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_map>

#include <clipp.h>
#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#if !PLATFORM_WINDOWS
#define __EMULATE_UUID 1
#endif
#include <dxc/dxcapi.h>

#if PLATFORM_WINDOWS
#include <wrl/client.h>
using namespace Microsoft::WRL;
#else
// defined in dxc WinAdapter.h
template <typename T>
using ComPtr = CComPtr<T>;
#endif

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <RHI/RHI.h>
using namespace Common;

class Application {
public:
    NON_COPYABLE(Application)
    explicit Application(std::string n) : rhiType(RHI::RHIType::VULKAN), window(nullptr), name(std::move(n)), width(1024), height(768)
    {
        CreateDXCCompilerInstance();
    }

    virtual ~Application() = default;

    int Run(int argc, char* argv[])
    {
        std::string rhiString;
        auto cli = (
            clipp::option("-w").doc("window width, 1024 by default") & clipp::value("width", width),
            clipp::option("-h").doc("window height, 768 by default") & clipp::value("height", height),
            clipp::required("-rhi").doc("RHI type, can be 'DirectX12' or 'Vulkan'") & clipp::value("RHI type", rhiString)
        );
        if (!clipp::parse(argc, argv, cli)) {
            std::cout << clipp::make_man_page(cli, argv[0]);
            return -1;
        }
        rhiType = rhiString == "DirectX12" ? RHI::RHIType::DIRECTX_12 : RHI::RHIType::VULKAN;

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

        OnCreate();
        while (!glfwWindowShouldClose(window)) {
            OnDrawFrame();
            glfwPollEvents();
        }
        OnDestroy();

        glfwTerminate();
        return 0;
    }

protected:
    virtual void OnStart() {}
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnDrawFrame() {}

    void* GetPlatformWindow()
    {
#if PLATFORM_WINDOWS
        return glfwGetWin32Window(window);
#elif PLATFORM_MACOS
        return glfwGetCocoaWindow(window);
#else
        Assert(false);
        return nullptr;
#endif
    }

    void CompileShader(std::vector<uint8_t>& byteCode, const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        uint32_t codePage = CP_UTF8;
        ComPtr<IDxcBlobEncoding> shaderSource;
        Assert(SUCCEEDED(dxcLibrary->CreateBlobFromFile(StringUtils::ToWideString(fileName).c_str(), &codePage, &shaderSource)));

        std::vector<LPCWSTR> arguments = {
#if !PLATFORM_WINDOWS
            L"-spirv",
#endif
            L"-Qembed_debug",
            DXC_ARG_WARNINGS_ARE_ERRORS,
            DXC_ARG_DEBUG,
            DXC_ARG_PACK_MATRIX_ROW_MAJOR
        };

        ComPtr<IDxcOperationResult> result;
        HRESULT success = dxcCompiler->Compile(
// ComPtr
#if PLATFORM_WINDOWS
            shaderSource.Get(),
// CComPtr
#else
            shaderSource,
#endif
            nullptr,
            StringUtils::ToWideString(entryPoint).c_str(),
            GetDXCTargetProfile(shaderStage).c_str(),
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
            std::cout << "failed to compiler shader (" << fileName << ", " << entryPoint << ")" << std::endl
                << static_cast<const char*>(errorInfo->GetBufferPointer()) << std::endl;
        }
        Assert(SUCCEEDED(success));

        ComPtr<IDxcBlob> code;
        Assert(SUCCEEDED(result->GetResult(&code)));

        const auto* codeStart = static_cast<const uint8_t*>(code->GetBufferPointer());
        const auto* codeEnd = codeStart + code->GetBufferSize();
        byteCode = std::vector<uint8_t>(codeStart, codeEnd);
    }

    RHI::RHIType rhiType;
    GLFWwindow* window;
    std::string name;
    uint32_t width;
    uint32_t height;
    ComPtr<IDxcLibrary> dxcLibrary;
    ComPtr<IDxcCompiler> dxcCompiler;

private:
    static std::wstring GetDXCTargetProfile(RHI::ShaderStageBits shaderStage)
    {
        std::wstring result;
        if (shaderStage == RHI::ShaderStageBits::VERTEX) {
            result = L"vs";
        } else if (shaderStage == RHI::ShaderStageBits::FRAGMENT) {
            result = L"ps";
        } else {
            Assert(false);
        }
        return result + L"_6_2";
    }

    void CreateDXCCompilerInstance()
    {
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&dxcLibrary))));
        Assert(SUCCEEDED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler))));
    }
};
