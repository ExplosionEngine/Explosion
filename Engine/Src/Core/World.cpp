//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/World.h>

namespace Explosion {
    World::World(std::string name) : name(std::move(name)), registry(originRegistry) {}

    World::~World() = default;

    Entity World::CreateEntity()
    {
        return registry.CreateEntity();
    }

    void World::DestroyEntity(const Entity& entity)
    {
        registry.DestroyEntity(entity);
    }

    void World::Update()
    {
        UpdateSystems();
    }

    void World::UpdateSystems()
    {
        for (auto& system : systems) {
            system.second->Update(registry);
        }
    }
}
