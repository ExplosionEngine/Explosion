//
// Created by johnk on 2022/8/3.
//

#include <Launcher/GameApplication.h>

namespace Launcher {
    GameWindow::GameWindow()
    {
        // TODO
    }

    GameWindow::~GameWindow()
    {
        // TODO
    }

    void* GameWindow::GetCanvas()
    {
        // TODO
        return nullptr;
    }

    uint32_t GameWindow::GetWidth()
    {
        // TODO
        return 0;
    }

    uint32_t GameWindow::GetHeight()
    {
        // TODO
        return 0;
    }

    GameApplication::GameApplication() = default;

    GameApplication::~GameApplication() = default;

    void GameApplication::SetMainWindow(Engine::IWindow* inWindow)
    {
        // TODO
    }

    int GameApplication::Exec(int argc, char** argv)
    {
        // TODO
        return 0;
    }
}
