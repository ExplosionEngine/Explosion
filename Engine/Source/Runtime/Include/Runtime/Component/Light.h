//
// Created by johnk on 2023/4/4.
//

#pragma once

#include <Runtime/ECS.h>
#include <Common/Math/Color.h>

namespace Runtime {
    class EClass() LightComponent : public Component {
    public:
        EProperty(category="Light")
        Common::Color color;

        EProperty(category="Light")
        float intensity;

        EProperty(category="Shadow")
        bool castShadows;
    };
}
