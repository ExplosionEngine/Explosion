//
// Created by John Kindem on 2021/6/3.
//

#include <Explosion/World/World.h>

namespace Explosion {
    World::World() = default;

    World::~World() = default;

    void World::Tick(float time)
    {
        JobSystem::Executor ticker;
        JobSystem::TaskFlow tickTask;

        // tick systems
        for (auto& system : systems) {
            tickTask.emplace([this, time, &system]() -> void {
                JobSystem::Executor executor;
                JobSystem::TaskFlow jobs = system(registry, time);
                executor.run(jobs).wait();
            });
        }
        ticker.run(tickTask).wait();
    }

    void World::AddSystem(ECS::System system)
    {
        systems.emplace_back(system);
    }
}
