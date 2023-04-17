//
// Created by johnk on 2023/4/5.
//

#pragma once

#include <Runtime/Asset.h>

namespace Runtime {
    class EClass() Material : public Asset {
    public:
        // TODO this will created by material graph editor
        EProperty()
        std::string code;
    };
}
