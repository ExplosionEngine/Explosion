//
// Created by John Kindem on 2021/6/3.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <Engine/ECS.h>

namespace Explosion {
    class World {
    public:
        World();
        ~World();
        ECS::Registry& GetRegistry();
        void Tick(float time);
        void AddSystemGroups(const ECS::SystemGroup& systemGroup);

    private:
        void TickSystem(float time);

        void SetupBuiltInSystems();

        ECS::Registry registry;
        std::vector<ECS::SystemGroup> systemGroups;
    };
}

#endif //EXPLOSION_WORLD_H
