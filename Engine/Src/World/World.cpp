//
// Created by John Kindem on 2021/6/3.
//

#include <Explosion/World/World.h>

namespace Explosion {
    World::World() = default;

    World::~World() = default;

    void World::Tick(float time)
    {
        TickSystem(time);

        // TODO tick others
    }

    void World::TickSystem(float time)
    {
        // TODO
    }
}
