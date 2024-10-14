//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <functional>

#include <Render/SceneProxy/Light.h>

namespace Render {
    template <typename T>
    using SceneProxyPatcher = std::function<void(T&)>;

    class IScene {
    public:
        virtual ~IScene() = default;
        virtual size_t GAddLight(const LightSceneProxy& inLight) = 0;
        virtual void GRemoveLight(size_t inLightIndex) = 0;
        virtual void GPatchLight(size_t inLightIndex, const SceneProxyPatcher<LightSceneProxy>& inPatcher) = 0;
    };
}
