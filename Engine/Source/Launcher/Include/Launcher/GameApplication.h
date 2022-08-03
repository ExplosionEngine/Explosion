//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Engine/Application.h>

namespace Launcher {
    class GameWindow : public Engine::IWindow {
    public:
        GameWindow();
        ~GameWindow();

        void* GetCanvas() override;
        uint32_t GetWidth() override;
        uint32_t GetHeight() override;
    };

    class GameApplication : public Engine::IApplication {
    public:
        GameApplication();
        ~GameApplication();

        void SetMainWindow(Engine::IWindow* inWindow) override;
        int Exec(int argc, char* argv[]) override;
    };
}
