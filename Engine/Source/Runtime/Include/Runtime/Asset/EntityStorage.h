//
// Created by johnk on 2023/11/22.
//

#pragma once

#include <Runtime/Asset.h>

namespace Runtime {
    class EClass() EntityStorage : public Runtime::Asset {
    public:
        EClassBody(EntityStorage)

        EProperty()
        std::unordered_map<std::string, std::vector<uint8>> components;
    };
}
