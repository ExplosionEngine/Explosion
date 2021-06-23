//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <Explosion/ECS/ECS.h>

namespace Explosion {
    class World {
    public:
        World();
        ~World();
        void Tick(float time);
        void AddSystemGroup(const ECS::SystemGroup& systemGroup);
        const std::vector<ECS::SystemGroup>& GetSystemGroups();

    private:
        void TickSystem(float time);

        ECS::Registry registry;
        std::vector<ECS::SystemGroup> systemGroups;
    };
}

#endif //EXPLOSION_WORLD_H
