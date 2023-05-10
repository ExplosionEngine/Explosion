//
// Created by johnk on 2023/4/4.
//

#pragma once

#include <Common/Math/Vector.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class EClass() LightComponent : public Component {
    public:
        EProperty(category=Light)
        Common::FVec3 color;

        EProperty(category=Light)
        float intensity;

        EProperty(category=Shadow)
        bool castShadows;
    };
}
