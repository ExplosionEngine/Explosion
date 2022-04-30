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
#include <dxc/dxcapi.h>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <RHI/RHI.h>

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
// TODO remove this when macOS dxc ready
#if PLATFORM_WINDOWS
        // TODO
#endif
        return false;
    }

    RHI::RHIType rhiType;
    GLFWwindow* window;
    std::string name;
    uint32_t width;
    uint32_t height;
};
