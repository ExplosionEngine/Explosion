//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <cstdint>

namespace Engine {
    class IApplication {
    public:
        virtual int Exec(int argc, char* argv[]) = 0;
    };
}
