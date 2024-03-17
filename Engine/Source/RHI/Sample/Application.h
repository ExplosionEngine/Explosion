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

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <Common/File.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Camera.h>

class Application {
public:
    NonCopyable(Application)
    explicit Application(std::string n)
        : rhiType(RHI::RHIType::vulkan)
        , window(nullptr)
        , name(std::move(n))
        , width(1024)
        , height(768)
        , instance(nullptr)
    {
    }

    virtual ~Application()
    {
        instance = nullptr;
        RHI::Instance::UnloadAllInstances();
    }

    static void KeyCallback(GLFWwindow* inWindow, int key, int scancode, int action, int mods)
    {
        auto* pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(inWindow));
        pApp->KeyCallbackImpl(key, action);
    }

    static void CursorCallback(GLFWwindow* inWindow, double x, double y)
    {
        auto* pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(inWindow));
        pApp->CursorCallbackImpl(static_cast<float>(x), static_cast<float>(y));
    }

    static void MouseButtonCallback(GLFWwindow* inWindow, int button, int action, int mods)
    {
        auto* pApp = reinterpret_cast<Application*>(glfwGetWindowUserPointer(inWindow));
        pApp->MouseButtonCallbackImlp(button, action);
    }

    void KeyCallbackImpl(int key, int action)
    {
        switch (action) {
            case GLFW_PRESS:
                switch (key) {
                    case GLFW_KEY_W:
                        camera->keys.front = true;
                        break;
                    case GLFW_KEY_S:
                        camera->keys.back = true;
                        break;
                    case GLFW_KEY_A:
                        camera->keys.left = true;
                        break;
                    case GLFW_KEY_D:
                        camera->keys.right = true;
                        break;
                    case GLFW_KEY_Q:
                        camera->keys.up = true;
                        break;
                    case GLFW_KEY_E:
                        camera->keys.down = true;
                        break;
                    default:
                        break;
                }
                break;
            case GLFW_RELEASE:
                switch (key) {
                    case GLFW_KEY_W:
                        camera->keys.front = false;
                        break;
                    case GLFW_KEY_S:
                        camera->keys.back = false;
                        break;
                    case GLFW_KEY_A:
                        camera->keys.left = false;
                        break;
                    case GLFW_KEY_D:
                        camera->keys.right = false;
                        break;
                    case GLFW_KEY_Q:
                        camera->keys.up = false;
                        break;
                    case GLFW_KEY_E:
                        camera->keys.down = false;
                        break;
                    default:
                        break;
                }
                break;
        }
    }

    void CursorCallbackImpl(float x, float y)
    {
        float dx = mousePos.x - x;
        float dy = mousePos.y - y;

        if (mouseButtons.left) {
            // rotate camera with mouse's left button down (positive value represents counterclockwise rotation)
            // horizontal mouse moving(dx) causes rotation alng y axis
            // vertical mouse moving(dy) causes rotation along x axis
            camera->Rotate(FVec3(-dy * camera->rotateSpeed, -dx * camera->rotateSpeed, 0.0f));
        }

        if (mouseButtons.right) {
            // zoom the view with mouse's wheels down (make camera close to or away from the target)
            camera->Translate(FVec3(0.0f, 0.0f, dy * 0.005f));
        }

        if (mouseButtons.middle) {
            // translate camera with mouse's right button down
            camera->Translate(FVec3(-dx * 0.01f, -dy * 0.01f, 0.0f));
        }

        mousePos = { x, y };
    }

    void MouseButtonCallbackImlp(int button, int action)
    {
        switch (action) {
            case GLFW_PRESS:
                switch (button) {
                    case GLFW_MOUSE_BUTTON_LEFT:
                        mouseButtons.left = true;
                        break;
                    case GLFW_MOUSE_BUTTON_RIGHT:
                        mouseButtons.right = true;
                        break;
                    case GLFW_MOUSE_BUTTON_MIDDLE:
                        mouseButtons.middle = true;
                        break;
                }
                break;
            case GLFW_RELEASE:
                switch (button) {
                    case GLFW_MOUSE_BUTTON_LEFT:
                        mouseButtons.left = false;
                        break;
                    case GLFW_MOUSE_BUTTON_RIGHT:
                        mouseButtons.right = false;
                        break;
                    case GLFW_MOUSE_BUTTON_MIDDLE:
                        mouseButtons.middle = false;
                        break;
                }
                break;
        }
    }

    float GetFrameDelta()
    {
        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<float, std::milli>(tEnd - tPrev).count();
        tPrev = tEnd;

        return tDiff;
    }

    int Run(int argc, char* argv[])
    {
        std::string rhiString;
        auto cli = (
            clipp::option("-w").doc("window width, 1024 by default") & clipp::value("width", width),
            clipp::option("-h").doc("window height, 768 by default") & clipp::value("height", height),
            clipp::required("-rhi").doc("RHI type, can be 'DirectX12', 'Metal' or 'Vulkan'") & clipp::value("RHI type", rhiString)
        );
        if (!clipp::parse(argc, argv, cli)) {
            std::cout << clipp::make_man_page(cli, argv[0]);
            return -1;
        }
        static const std::unordered_map<std::string, RHI::RHIType> RHI_MAP = {
            { "DirectX12", RHI::RHIType::directX12 },
            { "Vulkan", RHI::RHIType::vulkan },
            { "Metal", RHI::RHIType::metal }
        };
        auto iter = RHI_MAP.find(rhiString);
        rhiType = iter == RHI_MAP.end() ? RHI::RHIType::directX12 : iter->second;
        instance = RHI::Instance::GetByType(rhiType);

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);
        OnCreate();

        if (camera != nullptr) {
            glfwSetWindowUserPointer(window, this);
            glfwSetKeyCallback(window, KeyCallback);
            glfwSetCursorPosCallback(window, CursorCallback);
            glfwSetMouseButtonCallback(window, MouseButtonCallback);
        }

        while (!glfwWindowShouldClose(window)) {
            if (camera != nullptr) {
                camera->MoveCamera(GetFrameDelta());
            }
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
        QuickFail();
        return nullptr;
#endif
    }

    void CompileShader(std::vector<uint8_t>& byteCode, const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage, std::vector<std::string> includePaths = {})
    {
        std::string shaderSource = Common::FileUtils::ReadTextFile(fileName);

        Render::ShaderCompileInput info;
        info.source = shaderSource;
        info.entryPoint = entryPoint;
        info.stage = shaderStage;
        Render::ShaderCompileOptions options;
        if (!includePaths.empty()) {
            options.includePaths.insert(options.includePaths.end(), includePaths.begin(), includePaths.end());
        }
        if (rhiType == RHI::RHIType::directX12) {
            options.byteCodeType = Render::ShaderByteCodeType::dxil;
        } else if (rhiType == RHI::RHIType::vulkan) {
            options.byteCodeType = Render::ShaderByteCodeType::spirv;
        } else if (rhiType == RHI::RHIType::metal) {
            options.byteCodeType = Render::ShaderByteCodeType::mbc;
        }
        options.withDebugInfo = false;
        auto future = Render::ShaderCompiler::Get().Compile(info, options);

        future.wait();
        auto result = future.get();
        if (!result.success) {
            std::cout << "failed to compiler shader (" << fileName << ", " << info.entryPoint << ")" << '\n' << result.errorInfo << std::endl;
        }
        Assert(result.success);
        byteCode = result.byteCode;
    }

    RHI::Instance* instance = nullptr;
    RHI::RHIType rhiType;
    GLFWwindow* window;
    std::string name;
    uint32_t width;
    uint32_t height;

    UniqueRef<Camera> camera = nullptr;
    FVec2 mousePos = { 0, 0 };

    struct {
        bool left = false;
        bool right = false;
        bool middle = false;
    } mouseButtons;

    std::chrono::time_point<std::chrono::high_resolution_clock> tPrev;
};
