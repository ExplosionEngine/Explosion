//
// Created by johnk on 2024/8/2.
//

#include <Runtime/World.h>

namespace Runtime {
    const Mirror::Class* Internal::GetClassChecked(const std::string& inName)
    {
        return &Mirror::Class::Get(inName);
    }

    WorldTick::WorldTick(float inFrameTimeMs)
        : frameTimeMs(inFrameTimeMs)
    {
    }

    Commands::Commands(World& inWorld)
        : world(inWorld)
    {
    }

    Commands::~Commands() = default;

    Entity Commands::CreateEntity() // NOLINT
    {
        return world.registry.create();
    }

    void Commands::DestroyEntity(Entity inEntity) // NOLINT
    {
        world.registry.destroy(inEntity);
    }

    World::World(std::string inName)
        : started(false)
        , name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::Duplicate(World& inWorld) const
    {
        // TODO
    }

    void World::Start()
    {
        Assert(!started);
        started = true;
        BroadcastEvent<WorldStart>();
    }

    void World::Stop()
    {
        Assert(started);
        started = false;
        BroadcastEvent<WorldStop>();
    }

    void World::Tick(float inFrameTimeMs)
    {
        Assert(started);
        BroadcastEvent<WorldTick>(inFrameTimeMs);
    }

    EventBroadcaster::EventBroadcaster(World& inWorld)
        : world(inWorld)
    {
    }

    void EventBroadcaster::Dispatch()
    {
        tf::Executor executor;
        executor.run(taskflow);
    }

    void EventBroadcaster::AllocateEvent(EventClass inEventClass) // NOLINT
    {
        AssertWithReason(
            !allocatedEvents.contains(inEventClass),
            "system dependency do not support a event signal multi times in a dispatch graph, please check your systems");

        allocatedEvents.emplace(inEventClass, Mirror::Any());
        if (!world.listenersMap.contains(inEventClass)) {
            return;
        }

        for (const auto* listener : world.listenersMap.at(inEventClass)) {
            if (auto iter = world.signals.find(listener);
                iter != world.signals.end()) {
                AllocateEvent(iter->second);
            }
        }
    }

    void EventBroadcaster::BuildGraph(EventClass inEventClass, const tf::Task& wait) // NOLINT
    {
        if (!world.listenersMap.contains(inEventClass)) {
            return;
        }

        for (const auto* listener : world.listenersMap.at(inEventClass)) {
            const auto iter = world.signals.find(listener);
            const bool hasSignalEvent = iter != world.signals.end();

            auto task = taskflow.emplace([iter, hasSignalEvent, inEventClass, listener, this]() -> void {
                const auto& listenEvent = allocatedEvents.at(inEventClass);

                Commands commands(world);
                auto result = listener->GetMemberFunction("Execute").InvokeDyn(world.systemObjs.at(listener), { Mirror::Any(std::ref(commands)), listenEvent });
                if (hasSignalEvent) {
                    allocatedEvents.at(iter->second) = result;
                }
            });
            if (!wait.empty()) {
                task.succeed(wait);
            }

            if (hasSignalEvent) {
                BuildGraph(iter->second, task);
            }
        }
    }
}
