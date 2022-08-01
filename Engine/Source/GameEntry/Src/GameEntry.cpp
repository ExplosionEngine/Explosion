//
// Created by johnk on 2022/8/1.
//

#include <GLFW/glfw3.h>

#include <Engine/Engine.h>

int main(int argc, char* argv[])
{
    if (!Runtime::Engine::Get().Initialize(argc, argv)) {
        return 1;
    }
    // TODO glfw main loop & engine tick
    return 0;
}
