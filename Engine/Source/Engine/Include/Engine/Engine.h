//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Engine/Input.h>

namespace Engine {
    class Engine {
    public:
        static Engine& Get();
        ~Engine();

        void Initialize(int argc, char* argv[]);
        void Tick();
        InputManager& GetInputManager();

    private:
        Engine();

        InputManager inputManager;
    };
}
