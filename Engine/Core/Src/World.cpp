//
// Created by John Kindem on 2021/6/3.
//

#include <algorithm>

#include <Engine/World.h>

namespace Explosion {
    World::World() = default;

    World::~World() = default;

    ECS::Registry& World::GetRegistry()
    {
        return registry;
    }

    void World::Tick(float time)
    {
        TickSystem(time);

        // TODO tick others
    }

    void World::AddSystemGroups(const ECS::SystemGroup& systemGroup)
    {
        // TODO replace me with insertion sort
        systemGroups.emplace_back(systemGroup);
    }

    void World::TickSystem(float time)
    {
        // TODO remove sort
        std::sort(systemGroups.begin(), systemGroups.end(), [](const auto& a, const auto& b) -> bool {
            return a.priority > b.priority;
        });

        // update system group on by one
        for (const auto& systemGroup : systemGroups) {
            JobSystem::TaskFlow taskFlow;
            std::unordered_map<std::string, JobSystem::Task> tasks;

            for (const auto& sysIter : systemGroup.systems) {
                tasks[sysIter.first] = taskFlow.emplace([&sysIter, time, this]() -> void { sysIter.second(registry, time); });
            }

            for (const auto& depIter : systemGroup.dependencies) {
                tasks[depIter.first].succeed(tasks[depIter.second]);
            }

            JobSystem::Executor executor;
            executor.run(taskFlow).wait();
        }
    }
}
