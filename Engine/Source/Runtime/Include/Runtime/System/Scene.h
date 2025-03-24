//
// Created by johnk on 2025/1/9.
//

#pragma once

#include <optional>

#include <Runtime/ECS.h>
#include <Runtime/Component/Light.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/Component/Scene.h>
#include <Render/RenderModule.h>
#include <Render/Scene.h>
#include <Render/SceneProxy/Light.h>
#include <Render/SceneProxy/Primitive.h>

namespace Runtime {
    class RUNTIME_API EClass() SceneSystem final : public System {
        EPolyClassBody(SceneSystem)

    public:
        explicit SceneSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext);
        ~SceneSystem() override;

        NonCopyable(SceneSystem)
        NonMovable(SceneSystem)

        void Tick(float inDeltaTimeSeconds) override;

    private:
        template <typename Component, typename SceneProxy> void QueueCreateSceneProxy(Entity inEntity);
        template <typename Component, typename SceneProxy> void QueueUpdateSceneProxyContent(Entity inEntity);
        template <typename SceneProxy> void QueueUpdateSceneProxyTransform(Entity inEntity);
        template <typename SceneProxy> void QueueRemoveSceneProxy(Entity inEntity);

        Render::RenderModule& renderModule;
        Observer transformUpdatedObserver;
        EventsObserver<DirectionalLight> directionalLightsObserver;
        EventsObserver<PointLight> pointLightsObserver;
        EventsObserver<SpotLight> spotLightsObserver;
        // TODO primitive
    };
}

namespace Runtime::Internal {
    template <typename Component, typename SceneProxy>
    static void UpdateSceneProxyContent(SceneProxy& outSceneProxy, const Component& inComponent)
    {
        Unimplement();
    }

    template <typename SceneProxy>
    static void UpdateSceneProxyWorldTransform(SceneProxy& outSceneProxy, const WorldTransform& inTransform, bool withScale = true)
    {
        outSceneProxy.localToWorld = withScale ? inTransform.localToWorld.GetTransformMatrix() : inTransform.localToWorld.GetTransformMatrixNoScale();
    }

    template <typename T>
    static std::optional<T> GetOptional(const T* inObj)
    {
        return inObj == nullptr ? std::optional<T>() : *inObj;
    }
}

namespace Runtime::Internal {
    template <>
    static void UpdateSceneProxyContent<DirectionalLight, Render::LightSceneProxy>(Render::LightSceneProxy& outSceneProxy, const DirectionalLight& inComponent)
    {
        outSceneProxy.type = Render::LightType::directional;
        outSceneProxy.color = inComponent.color;
        outSceneProxy.intensity = inComponent.intensity;
    }

    template <>
    static void UpdateSceneProxyContent<PointLight, Render::LightSceneProxy>(Render::LightSceneProxy& outSceneProxy, const PointLight& inComponent)
    {
        outSceneProxy.type = Render::LightType::point;
        outSceneProxy.color = inComponent.color;
        outSceneProxy.intensity = inComponent.intensity;
        outSceneProxy.radius = inComponent.radius;
    }

    template <>
    static void UpdateSceneProxyContent<SpotLight, Render::LightSceneProxy>(Render::LightSceneProxy& outSceneProxy, const SpotLight& inComponent)
    {
        outSceneProxy.type = Render::LightType::spot;
        outSceneProxy.color = inComponent.color;
        outSceneProxy.intensity = inComponent.intensity;
    }

    // TODO primitive
}

namespace Runtime {
    template <typename Component, typename SceneProxy>
    void SceneSystem::QueueCreateSceneProxy(Entity inEntity)
    {
        const auto& sceneHolder = registry.GGet<SceneHolder>();
        const auto& component = registry.Get<Component>(inEntity);
        const auto* transform = registry.Find<WorldTransform>(inEntity);
        renderModule.GetRenderThread().EmplaceTask([scene = sceneHolder.scene.Get(), inEntity, component, transform = Internal::GetOptional(transform)]() -> void {
            SceneProxy sceneProxy;
            Internal::UpdateSceneProxyContent(sceneProxy, component);
            if (transform.has_value()) {
                Internal::UpdateSceneProxyWorldTransform(sceneProxy, transform.value(), false);
            }
            scene->Add<SceneProxy>(inEntity, std::move(sceneProxy));
        });
    }

    template <typename Component, typename SceneProxy>
    void SceneSystem::QueueUpdateSceneProxyContent(Entity inEntity)
    {
        const auto& sceneHolder = registry.GGet<SceneHolder>();
        const auto& component = registry.Get<Component>(inEntity);
        renderModule.GetRenderThread().EmplaceTask([scene = sceneHolder.scene.Get(), inEntity, component]() -> void {
            auto& sceneProxy = scene->Get<SceneProxy>(inEntity);
            Internal::UpdateSceneProxyContent(sceneProxy, component);
        });
    }

    template <typename SceneProxy>
    void SceneSystem::QueueUpdateSceneProxyTransform(Entity inEntity)
    {
        const auto& sceneHolder = registry.GGet<SceneHolder>();
        const auto& transform = registry.Get<WorldTransform>(inEntity);
        renderModule.GetRenderThread().EmplaceTask([scene = sceneHolder.scene.Get(), inEntity, transform]() -> void {
            auto& sceneProxy = scene->Get<SceneProxy>(inEntity);
            Internal::UpdateSceneProxyWorldTransform(sceneProxy, transform, false);
        });
    }

    template <typename SceneProxy>
    void SceneSystem::QueueRemoveSceneProxy(Entity inEntity)
    {
        const auto& sceneHolder = registry.GGet<SceneHolder>();
        renderModule.GetRenderThread().EmplaceTask([scene = sceneHolder.scene.Get(), inEntity]() -> void {
            scene->Remove<SceneProxy>(inEntity);
        });
    }
}
