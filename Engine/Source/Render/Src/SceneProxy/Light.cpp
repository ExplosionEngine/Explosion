//
// Created by johnk on 2023/8/14.
//

#include <Render/SceneProxy/Light.h>

namespace Render {
    ILightSceneProxy::ILightSceneProxy()
        : worldMatrix(Common::FMat4x4Consts::identity)
        , type(LightType::max)
        , color(Common::ColorConsts::white)
        , intensity(0.f)
    {
    }

    ILightSceneProxy::~ILightSceneProxy() = default;

    Common::FMat4x4 ILightSceneProxy::RGetLocalToWorld() const
    {
        return worldMatrix;
    }

    LightType ILightSceneProxy::RGetLightType() const
    {
        return type;
    }

    Common::Color ILightSceneProxy::RGetColor() const
    {
        return color;
    }

    float ILightSceneProxy::RGetIntensity() const
    {
        return intensity;
    }
}
