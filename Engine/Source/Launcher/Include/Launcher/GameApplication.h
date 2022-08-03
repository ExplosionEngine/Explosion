//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include <Engine/Application.h>
#include <Render/Canvas.h>

namespace Launcher {
    class GameApplication : public Engine::IApplication {
    public:
        GameApplication();
        ~GameApplication();

        int Exec(int argc, char* argv[]) override;

    private:
        static void OnResize(GLFWwindow* inWindow, int inWidth, int inHeight);
        static void OnMouseMove(GLFWwindow* inWindow, double inX, double inY);
        void* GetNativeWindow();
        void RecreateCanvas();

        GLFWwindow* window;
        uint32_t width;
        uint32_t height;
        std::string name;
        Render::Canvas* canvas;
    };
}
