//
// Created by johnk on 2023/10/17.
//

#pragma once

#include <Runtime/Asset.h>

namespace Runtime {
    class EClass() Level : public Asset {
    public:
        EClassBody(Level)

    private:
        EProperty()
        uint32_t entityNum;

        // TODO
    };
}
