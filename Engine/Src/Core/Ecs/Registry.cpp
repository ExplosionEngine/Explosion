//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Ecs/Registry.h>

namespace Explosion {
    Registry::Registry(entt::registry& registry) : registry(registry) {}

    Registry::~Registry() = default;

    Entity Registry::CreateEntity()
    {
        return registry.create();
    }

    void Registry::DestroyEntity(const Entity& entity)
    {
        registry.destroy(entity);
    }
}
