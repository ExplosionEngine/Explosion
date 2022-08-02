//
// Created by johnk on 2022/8/2.
//

#include <GameEntry/GameWindow.h>

namespace GameEntry {
    GameWindow::GameWindow() = default;

    GameWindow::~GameWindow() = default;

    int GameWindow::Exec(int argc, char** argv)
    {
        // TODO
        return 0;
    }

    void GameWindow::SetOnTickListener(Runtime::OnWindowTickListener inListener)
    {
        // TODO
    }

    void* GameWindow::GetWindow()
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

    void GameWindow::SetOnResizeListener(Runtime::OnWindowResizeListener inListener)
    {
        // TODO
    }
}
