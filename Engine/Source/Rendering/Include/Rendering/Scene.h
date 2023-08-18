//
// Created by johnk on 2023/7/22.
//

#pragma once

#include <unordered_set>

#include <Render/Scene.h>

namespace Rendering {
    class Scene : public Render::IScene {
    public:
        void AddLight(Render::ILightSceneProxy *inProxy) override;
        void RemoveLight(Render::ILightSceneProxy *inProxy) override;
        void AddPrimitive(Render::IPrimitiveSceneProxy *inProxy) override;
        void RemovePrimitive(Render::IPrimitiveSceneProxy *inProxy) override;

    private:
        std::unordered_set<Render::ILightSceneProxy*> lights;
        std::unordered_set<Render::IPrimitiveSceneProxy*> primitives;
    };
}
