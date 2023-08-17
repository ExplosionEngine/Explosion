//
// Created by johnk on 2023/8/14.
//

#pragma once

#include <Common/Math/Transform.h>
#include <Common/Math/Color.h>

namespace Render {
    enum class LightType {
        directional,
        point,
        spot,
        max
    };

    class ILightSceneProxy {
    public:
        ILightSceneProxy();
        virtual ~ILightSceneProxy();
        virtual Common::FMat4x4 RGetWorldMatrix() const;
        virtual LightType RGetLightType() const;
        virtual Common::Color RGetColor() const;
        virtual float RGetIntensity() const;

    protected:
        Common::FMat4x4 worldMatrix;
        LightType type;
        Common::Color color;
        float intensity;
    };
}
