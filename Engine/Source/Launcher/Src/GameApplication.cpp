//
// Created by johnk on 2022/8/3.
//

#include <Launcher/GameApplication.h>
#include <Engine/Engine.h>
#include <Renderer/Renderer.h>

#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

namespace Launcher {
    GameApplication::GameApplication() : window(nullptr), width(1024), height(768) {}

    GameApplication::~GameApplication() = default;

    int GameApplication::Exec(int argc, char** argv)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        auto& engine = Engine::Engine::Get();
        auto& renderer = Renderer::Renderer::Get();
        engine.Initialize(argc, argv);
        // TODO read window size and name from engine command line parse result

        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, &GameApplication::OnResize);
        glfwSetCursorPosCallback(window, &GameApplication::OnMouseMove);
        RecreateCanvas();

        while (!glfwWindowShouldClose(window)) {
            engine.Tick();
            // TODO
            Render::SceneView sceneView;
            renderer.RenderFrame(canvas, sceneView);
            glfwPollEvents();
        }
        canvas->Destroy();
        glfwDestroyWindow(window);
        return 0;
    }

    void GameApplication::OnResize(GLFWwindow* inWindow, int inWidth, int inHeight)
    {
        // TODO recreate canvas
    }

    void GameApplication::OnMouseMove(GLFWwindow* inWindow, double inX, double inY)
    {
        Engine::Engine::Get().GetInputManager().MouseMove(inX, inY);
    }

    void* GameApplication::GetNativeWindow()
    {
        // TODO glfw native
        return nullptr;
    }

    void GameApplication::RecreateCanvas()
    {
        if (canvas != nullptr) {
            canvas->Destroy();
        }
        canvas = Renderer::Renderer::Get().CreateCanvas(GetNativeWindow(), width, height);
    }
}
