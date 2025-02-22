//
// Created by johnk on 2025/1/9.
//

#include <Runtime/System/Scene.h>
#include <Runtime/Engine.h>
#include <Runtime/Component/Transform.h>
#include <Core/Module.h>

namespace Runtime {
    SceneSystem::SceneSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext)
        : System(inRegistry, inContext)
        , renderModule(EngineHolder::Get().GetRenderModule())
        , scene(renderModule.NewScene())
        , transformUpdatedObserver(inRegistry.Observer())
        , directionalLightsObserver(inRegistry.EventsObserver<DirectionalLight>())
        , pointLightsObserver(inRegistry.EventsObserver<PointLight>())
        , spotLightsObserver(inRegistry.EventsObserver<SpotLight>())
    {
        transformUpdatedObserver
            .ObConstructed<WorldTransform>()
            .ObUpdated<WorldTransform>();
    }

    SceneSystem::~SceneSystem() = default;

    void SceneSystem::Tick(float inDeltaTimeSeconds)
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

    void SceneSystem::RemoveLightSceneProxy(Entity e)
    {
        scene->RemoveLight(lightSceneProxies.at(e));
        lightSceneProxies.erase(e);
    }
}
