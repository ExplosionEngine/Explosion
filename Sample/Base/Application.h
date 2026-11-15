//
// Created by johnk on 11/1/2022.
//

#pragma once

#include <string>

#include <clipp.h>
#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include <Common/Utility.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Camera.h>

class SampleRenderTarget;

class Application {
public:
    enum class MouseButton : uint8_t {
        left,
        right,
        middle,
        max
    };

    struct ShaderCompileOutput {
        RHI::ShaderByteCodeRef byteCode;
        Render::ShaderReflectionData reflectionData;
    };

    NonCopyable(Application)
    explicit Application(std::string n);
    virtual ~Application();

    bool Initialize(int argc, char* argv[]);
    int RunLoop();

protected:
    virtual void OnStart();
    virtual void OnCreate();
    virtual void OnDestroy();
    virtual void OnDrawFrame();
    virtual void OnKeyActionReceived(int key, int action) const;
    virtual void OnCursorActionReceived(float x, float y);
    virtual void OnMouseButtonActionReceived(int button, int action);

    void SetCamera(Camera* inCamera);
    const std::string& GetApplicationName() const;
    double GetCurrentTimeSeconds() const;
    float GetDeltaTimeSeconds() const;
    void* GetNativeWindowHandle() const;
    uint32_t GetWindowWidth() const;
    uint32_t GetWindowHeight() const;
    bool IsHeadless() const;
    const std::string& GetOutputPath() const;
    RHI::RHIType GetRHIType() const;
    RHI::Instance* GetRHIInstance() const;
    RHI::Gpu* GetGpu() const;
    Camera& GetCamera() const;
    UniquePtr<SampleRenderTarget> CreateRenderTarget(RHI::Device& device) const;
    ShaderCompileOutput CompileShader(const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage, std::vector<std::string> includePaths = {}) const;

private:
    std::string name;
    GLFWwindow* window;
    UVec2 windowExtent;
    RHI::RHIType rhiType;
    RHI::Instance* instance;
    RHI::Gpu* gpu;
    bool headless;
    std::string outputPath;
    UniquePtr<Camera> camera;
    FVec2 mousePos;
    std::array<bool, static_cast<size_t>(MouseButton::max)> mouseButtonsStatus;
    double lastTimeSeconds;
    double currentTimeSeconds;
    float deltaTimeSeconds;
};
