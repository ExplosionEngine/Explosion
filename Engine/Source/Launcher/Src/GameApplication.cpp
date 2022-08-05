//
// Created by johnk on 2022/8/3.
//

#include <iostream>

#include <clipp.h>

#include <Launcher/GameApplication.h>
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include <Engine/Engine.h>
#include <Renderer/Renderer.h>

namespace Launcher {
    GameApplication::GameApplication() : window(nullptr), width(1024), height(768), canvas(nullptr) {}

    GameApplication::~GameApplication() = default;

    int GameApplication::Exec(int argc, char** argv)
    {
        struct CommandLineParseResult {
            std::string execFile;
            std::string projectFile;
            std::string map;
            std::string rhiString;
        } parseResult;

        {
            // TODO move to func
            parseResult.execFile = argv[0];
            auto cli = (
                clipp::value("projectFile", parseResult.projectFile),
                clipp::value("map", parseResult.map),
                clipp::required("-rhi").doc("RHI type") & clipp::value("RHI type", parseResult.rhiString)
            );
            if (!clipp::parse(argc, argv, cli)) {
                std::cout << clipp::make_man_page(cli, argv[0]);
                return 1;
            }
        }

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);
        RecreateCanvas();
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowSizeCallback(window, &GameApplication::OnResize);
        glfwSetCursorPosCallback(window, &GameApplication::OnMouseMove);

        auto& engine = Engine::Engine::Get();
        auto& renderer = Renderer::Renderer::Get();
        {
            Engine::EngineInitializer initializer {};
            initializer.application = this;
            initializer.execFile = parseResult.execFile;
            initializer.projectFile = parseResult.projectFile;
            // TODO
            engine.Initialize(initializer);
        }
        {
            Renderer::RendererInitializer initializer {};
            initializer.rhiString = parseResult.rhiString;
            renderer.Initialize(initializer);
        }

        while (!glfwWindowShouldClose(window)) {
            engine.Tick();
            // TODO
            Render::SceneViewCluster sceneView;
            renderer.RenderFrame(canvas, sceneView);
            glfwPollEvents();
        }
        canvas->Destroy();
        glfwDestroyWindow(window);
        return 0;
    }

    void GameApplication::ResizeMainWindow(uint32_t inWidth, uint32_t inHeight)
    {
        width = inWidth;
        height = inHeight;
        RecreateCanvas();
    }

    void GameApplication::OnResize(GLFWwindow* inWindow, int inWidth, int inHeight)
    {
        auto* gameApplication = static_cast<GameApplication*>(glfwGetWindowUserPointer(inWindow));
        gameApplication->RecreateCanvas();
    }

    void GameApplication::OnMouseMove(GLFWwindow* inWindow, double inX, double inY)
    {
        Engine::Engine::Get().GetInputManager().MouseMove(inX, inY);
    }

    void* GameApplication::GetNativeWindow()
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

    void GameApplication::RecreateCanvas()
    {
        if (canvas != nullptr) {
            canvas->Destroy();
        }
        canvas = Renderer::Renderer::Get().CreateCanvas(GetNativeWindow(), width, height);
    }
}
