//
// Created by johnk on 2023/8/17.
//

#include <Common/Debug.h>
#include <Rendering/Scene.h>

namespace Rendering {
    void Scene::AddLight(Render::ILightSceneProxy* inProxy)
    {
        Assert(!lights.contains(inProxy));
        lights.emplace(inProxy);
    }

    void Scene::RemoveLight(Render::ILightSceneProxy* inProxy)
    {
        Assert(lights.contains(inProxy));
        lights.erase(inProxy);
    }

    void Scene::AddPrimitive(Render::IPrimitiveSceneProxy* inProxy)
    {
        Assert(!primitives.contains(inProxy));
        primitives.erase(inProxy);
    }

    void Scene::RemovePrimitive(Render::IPrimitiveSceneProxy* inProxy)
    {
        Assert(primitives.contains(inProxy));
        primitives.emplace(inProxy);
    }
}
