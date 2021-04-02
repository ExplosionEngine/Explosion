//
// Created by Administrator on 2021/4/2 0002.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <string>

#include <Explosion/Core/Ecs/Component.h>
#include <Explosion/Core/Ecs/Entity.h>
#include <Explosion/Core/Ecs/System.h>

namespace Explosion {
    class World {
    public:
        World(std::string name);
        ~World();

        Entity CreateEntity();
        void DestroyEntity(const Entity& entity);

    private:
        std::string name;
        entt::registry registry;
    };
}

#endif //EXPLOSION_WORLD_H
