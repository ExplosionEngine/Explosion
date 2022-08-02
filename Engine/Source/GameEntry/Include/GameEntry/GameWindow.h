//
// Created by johnk on 2022/8/2.
//

#pragma once

#include <GLFW/glfw3.h>

#include <Engine/Window.h>

namespace GameEntry {
    class GameWindow : public Runtime::IWindow {
    public:
        GameWindow();
        ~GameWindow();

        int Exec(int argc, char **argv) override;
        void SetOnTickListener(Runtime::OnWindowTickListener inListener) override;
        void* GetWindow() override;
        uint32_t GetWidth() override;
        uint32_t GetHeight() override;
        void SetOnResizeListener(Runtime::OnWindowResizeListener inListener) override;

    private:
        GLFWwindow* window;
    };
}
