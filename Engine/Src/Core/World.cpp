//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/World.h>

namespace Explosion {
    World::World(std::string name) : name(std::move(name)) {}

    World::~World() = default;

    Entity World::CreateEntity()
    {
        return registry.create();
    }

    void World::DestroyEntity(const Entity& entity)
    {
        registry.destroy(entity);
    }
}
