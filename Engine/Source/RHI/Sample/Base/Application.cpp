//
// Created by johnk on 2024/5/21.
//

#include <unordered_map>
#include <iostream>

#include <Application.h>
#include <Common/Hash.h>

template <>
struct std::hash<std::pair<int, int>> {
    size_t operator()(const std::pair<int, int>& value) const noexcept
    {
        return HashUtils::CityHash(&value, sizeof(value));
    }
};

Application::Application(std::string n)
    : name(std::move(n))
    , window(nullptr)
    , windowExtent(1024, 768)
    , rhiType(RHI::RHIType::vulkan)
    , instance(nullptr)
    , mousePos(FVec2Consts::zero)
    , mouseButtonsStatus()
    , frameTime(0.0f)
{
}

Application::~Application()
{
    instance = nullptr;
    RHI::Instance::UnloadAllInstances();
}

int Application::Run(int argc, char* argv[])
{
    std::string rhiString;
    if (const auto cli = (
            clipp::option("-w").doc("window width, 1024 by default") & clipp::value("width", windowExtent.x),
            clipp::option("-h").doc("window height, 768 by default") & clipp::value("height", windowExtent.y),
            clipp::required("-rhi").doc("RHI type, can be 'dx12' or 'vulkan'") & clipp::value("RHI type", rhiString));
        !clipp::parse(argc, argv, cli)) {
        std::cout << clipp::make_man_page(cli, argv[0]);
        return -1;
    }

    rhiType = RHI::RHIAbbrStringToRHIType(rhiString);
    instance = RHI::Instance::GetByType(rhiType);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(static_cast<int>(windowExtent.x), static_cast<int>(windowExtent.y), name.c_str(), nullptr, nullptr);
    OnCreate();

    if (camera != nullptr) {
        auto keyCallback = [](GLFWwindow* inWindow, int key, int scancode, int action, int mods) -> void {
            const auto* app = static_cast<Application*>(glfwGetWindowUserPointer(inWindow));
            app->OnKeyActionReceived(key, action);
        };
        auto cursorCallback = [](GLFWwindow* inWindow, double x, double y) -> void {
            auto* app = static_cast<Application*>(glfwGetWindowUserPointer(inWindow));
            app->OnCursorActionReceived(static_cast<float>(x), static_cast<float>(y));
        };
        auto mouseButtonCallback = [](GLFWwindow* inWindow, int button, int action, int mods) -> void {
            auto* app = static_cast<Application*>(glfwGetWindowUserPointer(inWindow));
            app->OnMouseButtonActionReceived(button, action);
        };

        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, cursorCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
    }

    while (!glfwWindowShouldClose(window)) {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<float, std::milli>(currentTime - lastTime).count();
        lastTime = currentTime;
        if (camera != nullptr) {
            camera->Update(GetFrameTime());
        }
        OnDrawFrame();
        glfwPollEvents();
    }
    OnDestroy();

    glfwTerminate();
    return 0;
}

void Application::OnStart()
{
}

void Application::OnCreate()
{
}

void Application::OnDestroy()
{
}

void Application::OnDrawFrame()
{
}

void Application::OnKeyActionReceived(int key, int action) const
{
    static std::unordered_map<std::pair<int, int>, std::function<void(const Application&)>> actionMap = {
        { { GLFW_PRESS, GLFW_KEY_W }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::front); } },
        { { GLFW_PRESS, GLFW_KEY_S }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::back); } },
        { { GLFW_PRESS, GLFW_KEY_A }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::left); } },
        { { GLFW_PRESS, GLFW_KEY_D }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::right); } },
        { { GLFW_PRESS, GLFW_KEY_Q }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::up); } },
        { { GLFW_PRESS, GLFW_KEY_E }, [](const Application& application) -> void { application.camera->PerformMove(Camera::MoveDirection::down); } },
        { { GLFW_RELEASE, GLFW_KEY_W }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::front); } },
        { { GLFW_RELEASE, GLFW_KEY_S }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::back); } },
        { { GLFW_RELEASE, GLFW_KEY_A }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::left); } },
        { { GLFW_RELEASE, GLFW_KEY_D }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::right); } },
        { { GLFW_RELEASE, GLFW_KEY_Q }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::up); } },
        { { GLFW_RELEASE, GLFW_KEY_E }, [](const Application& application) -> void { application.camera->PerformStop(Camera::MoveDirection::down); } },
    };

    const auto iter = actionMap.find(std::make_pair(action, key));
    if (iter == actionMap.end()) {
        return;
    }
    iter->second(*this);
}

void Application::OnCursorActionReceived(float x, float y)
{
    const float dx = x - mousePos.x;
    const float dy = y - mousePos.y;

    // screen space of glfw:
    // origin in left-top, x from left to rignt, y from top to bottom
    if (mouseButtonsStatus[static_cast<size_t>(MouseButton::left)]) {
        // rotate camera with mouse's left button down (positive value represents counterclockwise rotation)
        // before apply axisTransMat
        //     x+ -> from screen outer to inner
        //     y+ -> from left to right
        //     z+ -> from bottom to top
        // horizontal mouse moving(dx) causes rotation alng z axis
        // vertical mouse moving(dy) causes rotation along y axis
        camera->Rotate(FVec3(0.0, -dy * camera->GetRotateSpeed(), -dx * camera->GetRotateSpeed()));
    }

    if (mouseButtonsStatus[static_cast<size_t>(MouseButton::right)]) {
        // zoom the view with mouse's wheels down (make camera close to or away from the target)
        camera->Translate(FVec3(0.0f, 0.0f, dy * 0.005f));
    }

    if (mouseButtonsStatus[static_cast<size_t>(MouseButton::middle)]) {
        // translate camera with mouse's right button down
        camera->Translate(FVec3(-dx * 0.01f, -dy * 0.01f, 0.0f));
    }

    mousePos = { x, y };
}

void Application::OnMouseButtonActionReceived(int button, int action)
{
    static std::unordered_map<std::pair<int, int>, std::function<void(Application&)>> actionMap = {
        { { GLFW_PRESS, GLFW_MOUSE_BUTTON_LEFT }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::left)] = true; } },
        { { GLFW_PRESS, GLFW_MOUSE_BUTTON_RIGHT }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::right)] = true; } },
        { { GLFW_PRESS, GLFW_MOUSE_BUTTON_MIDDLE }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::middle)] = true; } },
        { { GLFW_RELEASE, GLFW_MOUSE_BUTTON_LEFT }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::left)] = false; } },
        { { GLFW_RELEASE, GLFW_MOUSE_BUTTON_RIGHT }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::right)] = false; } },
        { { GLFW_RELEASE, GLFW_MOUSE_BUTTON_MIDDLE }, [](Application& application) -> void { application.mouseButtonsStatus[static_cast<size_t>(MouseButton::middle)] = false; } },
    };

    const auto iter = actionMap.find(std::make_pair(action, button));
    if (iter == actionMap.end()) {
        return;
    }
    iter->second(*this);
}

float Application::GetFrameTime() const
{
    return frameTime;
}

void* Application::GetPlatformWindow() const
{
#if PLATFORM_WINDOWS
    return glfwGetWin32Window(window);
#elif PLATFORM_MACOS
    return glfwGetCocoaWindow(window);
#else
    Unimplement();
    return nullptr;
#endif
}

const std::string& Application::GetApplicationName() const
{
    return name;
}

uint32_t Application::GetWindowWidth() const
{
    return windowExtent.x;
}

uint32_t Application::GetWindowHeight() const
{
    return windowExtent.y;
}

RHI::RHIType Application::GetRHIType() const
{
    return rhiType;
}

RHI::Instance* Application::GetRHIInstance() const
{
    return instance;
}

void Application::SetCamera(Camera* inCamera)
{
    camera = inCamera;
}

Camera& Application::GetCamera() const
{
    return *camera;
}

Application::ShaderCompileOutput Application::CompileShader(const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage, std::vector<std::string> includePaths) const
{
    std::string shaderSource = FileUtils::ReadTextFile(fileName);

    Render::ShaderCompileInput info;
    info.source = shaderSource;
    info.entryPoint = entryPoint;
    info.stage = shaderStage;

    Render::ShaderCompileOptions options;
    options.includePaths.emplace_back("../Test/Sample/RHI/ShaderInclude");
    options.includePaths.insert(options.includePaths.end(), includePaths.begin(), includePaths.end());

    if (rhiType == RHI::RHIType::directX12) {
        options.byteCodeType = Render::ShaderByteCodeType::dxil;
    } else {
        options.byteCodeType = Render::ShaderByteCodeType::spirv;
    }
    options.withDebugInfo = false;
    auto future = Render::ShaderCompiler::Get().Compile(info, options);

    future.wait();
    auto compileOutput = future.get();
    if (!compileOutput.success) {
        std::cout << "failed to compiler shader (" << fileName << ", " << info.entryPoint << ")" << '\n' << compileOutput.errorInfo << std::endl;
    }
    Assert(compileOutput.success);

    ShaderCompileOutput result;
    result.byteCode = std::move(compileOutput.byteCode);
    result.reflectionData = std::move(compileOutput.reflectionData);
    return result;
}
