//
// Created by John Kindem on 2021/6/3.
//

#include <Explosion/World/World.h>

namespace {
    void BuildTaskGraph(
        std::unordered_map<std::string, Explosion::JobSystem::Task>& taskMap,
        const Explosion::ECS::SystemNode& systemNode)
    {
        auto& baseTask = taskMap[systemNode.name];
        for (auto& after : systemNode.afters) {
            baseTask.succeed(taskMap[after->name]);
            BuildTaskGraph(taskMap, *after);
        }
    }
}

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
        for (auto& systemGroup : systemGroups) {
            JobSystem::TaskFlow taskFlow;

            // build all task
            std::unordered_map<std::string, JobSystem::Task> taskMap;
            for (auto& sysIter : systemGroup.systems) {
                taskMap[sysIter.first] = taskFlow.emplace([this, &sysIter, time]() -> void {
                    sysIter.second(registry, time);
                });
            }

            // build system graph to task graph
            for (auto& graphRoot : systemGroup.systemGraph.roots) {
                BuildTaskGraph(taskMap, *graphRoot);
            }

            // run all tasks
            JobSystem::Executor().run(taskFlow).wait();
        }
    }

    void World::AddSystemGroup(ECS::SystemGroup systemGroup)
    {
        auto insertIter = std::lower_bound(systemGroups.begin(), systemGroups.end(), systemGroup);
        systemGroups.insert(insertIter, std::move(systemGroup));
    }
}
