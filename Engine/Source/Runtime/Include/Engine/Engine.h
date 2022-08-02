//
// Created by johnk on 2022/7/18.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <Common/Path.h>
#include <Engine/Window.h>

namespace Runtime {
    class Engine {
    public:
        static Engine& Get();

        ~Engine();

        bool Initialize(int argc, char* argv[]);
        void BindGameWindow(IWindow* inGameWindow);
        [[nodiscard]] const Common::PathMapper& GetPathMapper() const;

    private:
        Engine();

        void Tick();

        IWindow* gameWindow;
        std::unique_ptr<Common::PathMapper> pathMapper;
    };
}
