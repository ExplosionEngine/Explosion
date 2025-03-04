//
// Created by johnk on 2023/7/22.
//

#pragma once

#include <Common/Debug.h>
#include <Core/Thread.h>
#include <Render/SceneProxy/Light.h>

namespace Render {
    // Render::Scene is a container of render-thread world data copy.
    // Notice all operations to scene need be down in render-thread.
    class Scene final {
    public:
        using EntityId = uint32_t;

        Scene();
        ~Scene();

        NonCopyable(Scene)
        NonMovable(Scene)

        template <typename SP> void Add(EntityId inEntity, SP&& inSceneProxy);
        template <typename SP> SP& Get(EntityId inEntity);
        template <typename SP> const SP& Get(EntityId inEntity) const;
        template <typename SP> void Remove(EntityId inEntity);

    private:
        template <typename SP> using SceneProxyContainer = std::unordered_map<EntityId, LightSceneProxy>;

        template <typename SP> SceneProxyContainer<SP>& GetSceneProxyContainer();
        template <typename SP> const SceneProxyContainer<SP>& GetSceneProxyContainer() const;

        SceneProxyContainer<LightSceneProxy> lightSceneProxies;
    };
}

namespace Render {
    template <typename SP>
    void Scene::Add(EntityId inEntity, SP&& inSceneProxy)
    {
        Assert(Core::ThreadContext::IsRenderThread());
        GetSceneProxyContainer<SP>().emplace(inEntity, std::move(inSceneProxy)); // NOLINT
    }

    template <typename SP>
    SP& Scene::Get(EntityId inEntity)
    {
        Assert(Core::ThreadContext::IsRenderThread());
        return GetSceneProxyContainer<SP>().at(inEntity);
    }

    template <typename SP>
    const SP& Scene::Get(EntityId inEntity) const
    {
        Assert(Core::ThreadContext::IsRenderThread());
        return GetSceneProxyContainer<SP>().at(inEntity);
    }

    template <typename SP>
    void Scene::Remove(EntityId inEntity)
    {
        Assert(Core::ThreadContext::IsRenderThread());
        GetSceneProxyContainer<SP>().erase(inEntity);
    }

    template <typename SP>
    Scene::SceneProxyContainer<SP>& Scene::GetSceneProxyContainer()
    {
        Unimplement();
        return *static_cast<SceneProxyContainer<SP>*>(nullptr); // NOLINT
    }

    template <typename SP>
    const Scene::SceneProxyContainer<SP>& Scene::GetSceneProxyContainer() const
    {
        Unimplement();
        return *static_cast<const SceneProxyContainer<SP>*>(nullptr); // NOLINT
    }
}

namespace Render {
    template <>
    inline Scene::SceneProxyContainer<LightSceneProxy>& Scene::GetSceneProxyContainer<LightSceneProxy>()
    {
        return lightSceneProxies;
    }

    template <>
    inline const Scene::SceneProxyContainer<LightSceneProxy>& Scene::GetSceneProxyContainer<LightSceneProxy>() const
    {
        return lightSceneProxies;
    }
}
