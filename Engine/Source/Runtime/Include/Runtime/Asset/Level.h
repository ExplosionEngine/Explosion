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
        std::vector<std::string> setupSystems;

        EProperty()
        std::vector<std::string> tickSystems;

        EProperty()
        std::unordered_map<std::string, std::vector<std::string>> eventSystems;
    };
}
