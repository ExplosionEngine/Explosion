//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <cstdint>

namespace Engine {
    class IWindow {
    public:
        virtual void* GetCanvas() = 0;
        virtual uint32_t GetWidth() = 0;
        virtual uint32_t GetHeight() = 0;
    };

    class IApplication {
    public:
        virtual void SetMainWindow(IWindow* inWindow) = 0;
        virtual int Exec(int argc, char* argv[]) = 0;
    };
}
