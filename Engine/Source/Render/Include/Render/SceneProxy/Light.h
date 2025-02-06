//
// Created by johnk on 2023/8/14.
//

#pragma once

#include <variant>

#include <Common/Math/Transform.h>
#include <Common/Math/Color.h>

namespace Render {
    enum class LightType : uint8_t {
        directional,
        point,
        spot,
        max
    };

    struct LightSceneProxy {
        LightType type;
        Common::FMat4x4 localToWorld;
        Common::Color color;
        float intensity;
        // point light only
        float radius;
    };
}
