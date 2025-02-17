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

        void Tick(float inDeltaTimeSeconds) override;

    private:
        template <typename SP> using SPMap = std::unordered_map<Entity, Render::SPHandle<SP>>;

        template <typename L> static void FillLightSceneProxy(const Render::LightSPH& inHandle, const L& inLight, const WorldTransform* inTransform);
        template <typename L> void EmplaceLightSceneProxy(Entity inEntity);
        template <typename L> void UpdateLightSceneProxy(Entity inEntity);
        template <typename SP> void UpdateTransformForSceneProxy(SPMap<SP>& inSceneProxyMap, Entity inEntity, bool inWithScale = true);
        void RemoveLightSceneProxy(Entity e);

        Render::RenderModule& renderModule;
        Common::UniquePtr<Render::Scene> scene;
        Observer transformUpdatedObserver;
        EventsObserver<DirectionalLight> directionalLightsObserver;
        EventsObserver<PointLight> pointLightsObserver;
        EventsObserver<SpotLight> spotLightsObserver;
        SPMap<Render::LightSceneProxy> lightSceneProxies;
    };
}
namespace Runtime {
    template <typename L>
    void SceneSystem::FillLightSceneProxy(const Render::LightSPH& inHandle, const L& inLight, const WorldTransform* inTransform)
    {
        Unimplement();
    }

    template <>
    inline void SceneSystem::FillLightSceneProxy<DirectionalLight>(const Render::LightSPH& inHandle, const DirectionalLight& inLight, const WorldTransform* inTransform)
    {
        Render::LightSceneProxy& sceneProxy = *inHandle; // NOLINT
        sceneProxy.type = Render::LightType::directional;
        sceneProxy.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        sceneProxy.color = inLight.color;
        sceneProxy.intensity = inLight.intensity;
    }

    template <>
    inline void SceneSystem::FillLightSceneProxy<PointLight>(const Render::LightSPH& inHandle, const PointLight& inLight, const WorldTransform* inTransform)
    {
        Render::LightSceneProxy& sceneProxy = *inHandle; // NOLINT
        sceneProxy.type = Render::LightType::point;
        sceneProxy.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        sceneProxy.color = inLight.color;
        sceneProxy.intensity = inLight.intensity;
        sceneProxy.radius = inLight.radius;
    }

    template <>
    inline void SceneSystem::FillLightSceneProxy<SpotLight>(const Render::LightSPH& inHandle, const SpotLight& inLight, const WorldTransform* inTransform)
    {
        Render::LightSceneProxy& sceneProxy = *inHandle; // NOLINT
        sceneProxy.type = Render::LightType::spot;
        sceneProxy.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        sceneProxy.color = inLight.color;
        sceneProxy.intensity = inLight.intensity;
    }

    template <typename L>
    void SceneSystem::EmplaceLightSceneProxy(Entity inEntity)
    {
        lightSceneProxies.emplace(inEntity, scene->AddLight());
        FillLightSceneProxy(lightSceneProxies.at(inEntity), registry.Get<L>(inEntity), registry.Find<WorldTransform>(inEntity));
    }

    template <typename L>
    void SceneSystem::UpdateLightSceneProxy(Entity inEntity)
    {
        const Render::LightSPH& handle = lightSceneProxies.at(inEntity);
        FillLightSceneProxy(handle, registry.Get<L>(inEntity), registry.Find<WorldTransform>(inEntity));
        scene->UpdateLight(handle);
    }

    template <typename SP>
    void SceneSystem::UpdateTransformForSceneProxy(SPMap<SP>& inSceneProxyMap, Entity inEntity, bool inWithScale)
    {
        const auto iter = inSceneProxyMap.find(inEntity);
        if (iter == inSceneProxyMap.end()) {
            return;
        }

        const Render::LightSPH& handle = iter->second;
        if (const WorldTransform* transform = registry.Find<WorldTransform>(inEntity);
            transform == nullptr) {
            handle->localToWorld = Common::FMat4x4Consts::identity;
        } else {
            handle->localToWorld = inWithScale ? transform->localToWorld.GetTransformMatrix() : transform->localToWorld.GetTransformMatrixNoScale();;
        }
    }
}
