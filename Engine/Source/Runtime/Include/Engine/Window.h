//
// Created by johnk on 2022/8/2.
//

#pragma once

#include <functional>

namespace Runtime {
    using OnWindowTickListener = std::function<void()>;
    using OnWindowResizeListener = std::function<void()>;

    class IWindow {
    public:
        virtual int Exec(int argc, char* argv[]) = 0;
        virtual void SetOnTickListener(OnWindowTickListener inListener) = 0;
        virtual void* GetWindow() = 0;
        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;
        virtual void SetOnResizeListener(OnWindowResizeListener inListener) = 0;
    };
}
