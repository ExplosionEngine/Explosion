//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <cstdint>

namespace Runtime{
    class IApplication {
    public:
        virtual int Exec(int argc, char* argv[]) = 0;
        virtual void ResizeMainWindow(uint32_t inWidth, uint32_t inHeight) = 0;
    };
}
