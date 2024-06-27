//
// Created by johnk on 2024/6/27.
//

#pragma once

#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>

namespace Runtime {
    class EClass() GameObject {
        EClassBody(GameObject)

    private:
        std::unordered_map<Mirror::Class*, Mirror::Any> components;
    };
}
