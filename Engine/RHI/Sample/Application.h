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
#include <ShaderConductor/ShaderConductor.hpp>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <RHI/Instance.h>

static ShaderConductor::ShaderStage CastShaderStage(RHI::ShaderStageBits stage)
{
    static std::unordered_map<RHI::ShaderStageBits, ShaderConductor::ShaderStage> MAP = {
        { RHI::ShaderStageBits::VERTEX, ShaderConductor::ShaderStage::VertexShader },
        { RHI::ShaderStageBits::FRAGMENT, ShaderConductor::ShaderStage::PixelShader },
        { RHI::ShaderStageBits::COMPUTE, ShaderConductor::ShaderStage::ComputeShader }
    };
    auto iter = MAP.find(stage);
    Assert(iter != MAP.end());
    return iter->second;
}

static ShaderConductor::ShadingLanguage GetShadingLanguage(RHI::RHIType rhiType)
{
    static std::unordered_map<RHI::RHIType, ShaderConductor::ShadingLanguage> MAP = {
        { RHI::RHIType::DIRECTX_12, ShaderConductor::ShadingLanguage::Dxil },
        { RHI::RHIType::VULKAN, ShaderConductor::ShadingLanguage::SpirV }
    };
    auto iter = MAP.find(rhiType);
    Assert(iter != MAP.end());
    return iter->second;
}

class Application {
public:
    NON_COPYABLE(Application)
    explicit Application(std::string n) : rhiType(RHI::RHIType::VULKAN), window(nullptr), name(std::move(n)), width(1024), height(768) {}
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

    static std::string ReadTextFile(const std::string& fileName)
    {
        std::ifstream file(fileName, std::ios::in);
        Assert(file.is_open());
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }

    bool CompileShader(std::vector<uint8_t>& byteCode, const std::string& source, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        using ShaderConductor::Compiler;

        Compiler::SourceDesc sourceDesc {};
        sourceDesc.source = source.c_str();
        sourceDesc.entryPoint = entryPoint.c_str();
        sourceDesc.stage = CastShaderStage(shaderStage);

        Compiler::Options options {};
        options.disableOptimizations = true;
        options.enableDebugInfo = true;

        Compiler::TargetDesc targetDesc {};
        targetDesc.asModule = true;
        targetDesc.language = GetShadingLanguage(rhiType);

        auto result = Compiler::Compile(sourceDesc, options, targetDesc);
        if (result.hasError) {
            std::cout << (const char*)(result.errorWarningMsg->Data()) << std::endl;
            return false;
        }

        const auto* dataBegin = static_cast<const uint8_t*>(result.target->Data());
        const auto* dataEnd = static_cast<const uint8_t*>(result.target->Data()) + result.target->Size();
        byteCode = std::vector(dataBegin, dataEnd);
        return true;
    }

    RHI::RHIType rhiType;
    GLFWwindow* window;
    std::string name;
    uint32_t width;
    uint32_t height;
};
