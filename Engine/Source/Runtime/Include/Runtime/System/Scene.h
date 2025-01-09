//
// Created by johnk on 2025/1/9.
//

#pragma once

#include <Runtime/ECS.h>
#include <Runtime/Component/Light.h>
#include <Runtime/Component/Transform.h>
#include <Render/RenderModule.h>
#include <Render/Scene.h>
#include <Render/SceneProxy/Light.h>

namespace Runtime {
    class RUNTIME_API EClass() SceneSystem final : public System {
        EPolyClassBody(SceneSystem)

    public:
        explicit SceneSystem(ECRegistry& inRegistry);
        ~SceneSystem() override;

        NonCopyable(SceneSystem)
        NonMovable(SceneSystem)

        void Tick(float inDeltaTimeMs) override;

    private:
        template <typename SP> using SPMap = std::unordered_map<Entity, Render::SPHandle<SP>>;

        static Render::LightSceneProxy MakeLightSceneProxy(const DirectionalLight& inDirectionalLight, const Transform* inTransform);
        static Render::LightSceneProxy MakeLightSceneProxy(const PointLight& inPointLight, const Transform* inTransform);
        static Render::LightSceneProxy MakeLightSceneProxy(const SpotLight& inSpotLight, const Transform* inTransform);
        template <typename L> void EmplaceLightSceneProxy(Entity inEntity);
        template <typename L> void UpdateLightSceneProxy(Entity inEntity);
        template <typename SP> void UpdateTransformForSceneProxy(SPMap<SP>& inSceneProxyMap, Entity inEntity, bool inWithScale = true);
        void RemoveLightSceneProxy(Entity e);

        Render::RenderModule& renderModule;
        Common::UniqueRef<Render::Scene> scene;
        Observer transformUpdatedObserver;
        EventsObserver<DirectionalLight> directionalLightsObserver;
        EventsObserver<PointLight> pointLightsObserver;
        EventsObserver<SpotLight> spotLightsObserver;
        SPMap<Render::LightSceneProxy> lightSceneProxies;
    };
}

namespace Runtime {
    template <typename L>
    void SceneSystem::EmplaceLightSceneProxy(Entity inEntity)
    {
        lightSceneProxies.emplace(inEntity, scene->AddLight(MakeLightSceneProxy(registry.Get<L>(inEntity), registry.Find<Transform>(inEntity))));
    }

    template <typename L>
    void SceneSystem::UpdateLightSceneProxy(Entity inEntity)
    {
        *lightSceneProxies.at(inEntity) = MakeLightSceneProxy(registry.Get<L>(inEntity), registry.Find<Transform>(inEntity));
    }

    template <typename SP>
    void SceneSystem::UpdateTransformForSceneProxy(SPMap<SP>& inSceneProxyMap, Entity inEntity, bool inWithScale)
    {
        const auto iter = inSceneProxyMap.find(inEntity);
        if (iter == inSceneProxyMap.end()) {
            return;
        }

        if (const Transform* transform = registry.Find<Transform>(inEntity);
            transform == nullptr) {
            iter->second->localToWorld = Common::FMat4x4Consts::identity;
        } else {
            iter->second->localToWorld = inWithScale ? transform->localToWorld.GetTransformMatrix() : transform->localToWorld.GetTransformMatrixNoScale();;
        }
    }
}
