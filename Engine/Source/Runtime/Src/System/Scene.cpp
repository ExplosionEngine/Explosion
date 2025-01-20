//
// Created by johnk on 2025/1/9.
//

#include <Runtime/System/Scene.h>
#include <Runtime/Component/Transform.h>
#include <Core/Module.h>

namespace Runtime {
    SceneSystem::SceneSystem(ECRegistry& inRegistry)
        : System(inRegistry)
        , renderModule(Core::ModuleManager::Get().GetTyped<Render::RenderModule>("Render"))
        , scene(renderModule.NewScene())
        , transformUpdatedObserver(inRegistry.Observer())
        , directionalLightsObserver(inRegistry.EventsObserver<DirectionalLight>())
        , pointLightsObserver(inRegistry.EventsObserver<PointLight>())
        , spotLightsObserver(inRegistry.EventsObserver<SpotLight>())
    {
        transformUpdatedObserver
            .ObConstructed<Transform>()
            .ObUpdated<Transform>();
    }

    SceneSystem::~SceneSystem() = default;

    void SceneSystem::Tick(float inDeltaTimeMs)
    {
        directionalLightsObserver.Constructed().Each([this](Entity e) -> void { EmplaceLightSceneProxy<DirectionalLight>(e); });
        pointLightsObserver.Constructed().Each([this](Entity e) -> void { EmplaceLightSceneProxy<PointLight>(e); });
        spotLightsObserver.Constructed().Each([this](Entity e) -> void { EmplaceLightSceneProxy<SpotLight>(e); });
        directionalLightsObserver.Updated().Each([this](Entity e) -> void { UpdateLightSceneProxy<DirectionalLight>(e); });
        pointLightsObserver.Updated().Each([this](Entity e) -> void { UpdateLightSceneProxy<PointLight>(e); });
        spotLightsObserver.Updated().Each([this](Entity e) -> void { UpdateLightSceneProxy<SpotLight>(e); });
        directionalLightsObserver.Removed().Each([this](Entity e) -> void { RemoveLightSceneProxy(e); });
        pointLightsObserver.Removed().Each([this](Entity e) -> void { RemoveLightSceneProxy(e); });
        spotLightsObserver.Removed().Each([this](Entity e) -> void { RemoveLightSceneProxy(e); });
        transformUpdatedObserver.Each([this](Entity e) -> void { UpdateTransformForSceneProxy<Render::LightSceneProxy>(lightSceneProxies, e); });

        transformUpdatedObserver.Clear();
        directionalLightsObserver.Clear();
        pointLightsObserver.Clear();
        spotLightsObserver.Clear();
    }

    Render::LightSceneProxy SceneSystem::MakeLightSceneProxy(const DirectionalLight& inDirectionalLight, const Transform* inTransform)
    {
        Render::LightSceneProxy result {};
        result.type = Render::LightType::directional;
        result.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        result.color = inDirectionalLight.color;
        result.intensity = inDirectionalLight.intensity;
        result.typedPart.emplace<Render::DirectionalLightSceneProxyPart>();
        return result;
    }

    Render::LightSceneProxy SceneSystem::MakeLightSceneProxy(const PointLight& inPointLight, const Transform* inTransform)
    {
        Render::LightSceneProxy result {};
        result.type = Render::LightType::point;
        result.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        result.color = inPointLight.color;
        result.intensity = inPointLight.intensity;

        auto& typedPart = result.typedPart.emplace<Render::PointLightSceneProxyPart>(); // NOLINT
        typedPart.radius = inPointLight.radius;
        return result;
    }

    Render::LightSceneProxy SceneSystem::MakeLightSceneProxy(const SpotLight& inSpotLight, const Transform* inTransform)
    {
        Render::LightSceneProxy result {};
        result.type = Render::LightType::spot;
        result.localToWorld = inTransform == nullptr ? Common::FMat4x4Consts::identity : inTransform->localToWorld.GetTransformMatrixNoScale();
        result.color = inSpotLight.color;
        result.intensity = inSpotLight.intensity;
        result.typedPart.emplace<Render::SpotLightSceneProxyPart>();
        return result;
    }

    void SceneSystem::RemoveLightSceneProxy(Entity e)
    {
        lightSceneProxies.erase(e);
    }
}
