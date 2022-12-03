//
// Created by johnk on 2022/11/18.
//

#pragma once

#include <Common/Math/Vector3.h>

namespace Runtime {
    struct TransformComponent {
    public:
        Common::Vector3 position;
        Common::Vector3 rotation;
        Common::Vector3 scale;
    };
}
