//
// Created by johnk on 2022/8/1.
//

#include <Engine/Engine.h>
#include <GameEntry/GameWindow.h>

int main(int argc, char* argv[])
{
    if (!Runtime::Engine::Get().Initialize(argc, argv)) {
        return 1;
    }
    GameEntry::GameWindow gameWindow;
    Runtime::Engine::Get().BindGameWindow(&gameWindow);
    return gameWindow.Exec(argc, argv);
}
