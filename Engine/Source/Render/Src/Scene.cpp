//
// Created by johnk on 2023/8/17.
//

#include <Render/Scene.h>

namespace Render {
    Scene::Scene() = default;

    Scene::~Scene() = default;

    LightSPH Scene::AddLight()
    {
        return lights.Emplace();
    }

    void Scene::UpdateLight(const LightSPH& inHandle) {}

    void Scene::RemoveLight(const LightSPH& inHandle)
    {
        lights.Erase(inHandle);
    }
}
