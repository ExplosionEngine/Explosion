//
// Created by johnk on 2025/1/9.
//

#include <utility>

#include <Runtime/System/Scene.h>
#include <Runtime/Engine.h>
#include <Runtime/Component/Transform.h>

namespace Runtime {
    SceneSystem::SceneSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext)
        : System(inRegistry, inContext)
        , renderModule(EngineHolder::Get().GetRenderModule())
        , transformUpdatedObserver(inRegistry.Observer())
        , directionalLightsObserver(inRegistry.EventsObserver<DirectionalLight>())
        , pointLightsObserver(inRegistry.EventsObserver<PointLight>())
        , spotLightsObserver(inRegistry.EventsObserver<SpotLight>())
    {
        transformUpdatedObserver
            .ObConstructed<WorldTransform>()
            .ObUpdated<WorldTransform>();

        inRegistry.GEmplace<SceneHolder>(renderModule.NewScene());
    }

    SceneSystem::~SceneSystem() // NOLINT
    {
        auto& sceneHolder = registry.GGet<SceneHolder>();
        renderModule.GetRenderThread().EmplaceTask([scene = std::exchange(sceneHolder.scene, nullptr)]() -> void {
            delete scene;
        });
    }

    void SceneSystem::Tick(float inDeltaTimeSeconds)
    {
        directionalLightsObserver.Constructed().Each([this](Entity e) -> void { QueueCreateSceneProxy<DirectionalLight, Render::LightSceneProxy>(e); });
        pointLightsObserver.Constructed().Each([this](Entity e) -> void { QueueCreateSceneProxy<PointLight, Render::LightSceneProxy>(e); });
        spotLightsObserver.Constructed().Each([this](Entity e) -> void { QueueCreateSceneProxy<SpotLight, Render::LightSceneProxy>(e); });
        directionalLightsObserver.Updated().Each([this](Entity e) -> void { QueueUpdateSceneProxyContent<DirectionalLight, Render::LightSceneProxy>(e); });
        pointLightsObserver.Updated().Each([this](Entity e) -> void { QueueUpdateSceneProxyContent<PointLight, Render::LightSceneProxy>(e); });
        spotLightsObserver.Updated().Each([this](Entity e) -> void { QueueUpdateSceneProxyContent<SpotLight, Render::LightSceneProxy>(e); });
        directionalLightsObserver.Removed().Each([this](Entity e) -> void { QueueRemoveSceneProxy<Render::LightSceneProxy>(e); });
        pointLightsObserver.Removed().Each([this](Entity e) -> void { QueueRemoveSceneProxy<Render::LightSceneProxy>(e); });
        spotLightsObserver.Removed().Each([this](Entity e) -> void { QueueRemoveSceneProxy<Render::LightSceneProxy>(e); });

        transformUpdatedObserver.Each([this](Entity e) -> void {
            if (registry.Has<DirectionalLight>(e) || registry.Has<PointLight>(e) || registry.Has<SpotLight>(e)) {
                QueueUpdateSceneProxyTransform<Render::LightSceneProxy>(e);
            }
        });

        transformUpdatedObserver.Clear();
        directionalLightsObserver.Clear();
        pointLightsObserver.Clear();
        spotLightsObserver.Clear();
    }
}
