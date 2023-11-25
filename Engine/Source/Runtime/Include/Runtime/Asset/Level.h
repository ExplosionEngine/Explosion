//
// Created by johnk on 2023/11/24.
//

#pragma once

#include <Runtime/Asset.h>

namespace Runtime {
    class EClass() Level : public Runtime::Asset {
    public:
        EClassBody(Level)

        EProperty()
        std::unordered_set<std::string> systems;
    };
}
