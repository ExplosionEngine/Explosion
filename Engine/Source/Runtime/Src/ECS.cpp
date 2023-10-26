//
// Created by johnk on 2023/9/5.
//

#include <Runtime/ECS.h>

namespace Runtime {
    bool ClassSignature::operator==(const ClassSignature& rhs) const
    {
        return id == rhs.id;
    }

    SystemCommands::SystemCommands(ECSHost& inHost)
        : registry(inHost.registry)
        , host(inHost)
    {
    }

    SystemCommands::~SystemCommands() = default;

    Entity SystemCommands::Create(Entity hint)
    {
        return registry.create(hint);
    }

    void SystemCommands::Destroy(Entity inEntity)
    {
        registry.destroy(inEntity);
    }

    bool SystemCommands::Valid(Entity inEntity) const
    {
        return registry.valid(inEntity);
    }

    ECSHost::SystemInstance::SystemInstance()
        : type(SystemType::max)
        , object(nullptr)
    {
    }

    ECSHost::SystemInstance::~SystemInstance() = default;

    ECSHost::SystemInstance::SystemInstance(ECSHost::SystemInstance&& other) noexcept
        : type(other.type)
        , object(std::move(other.object))
    {
        if (other.type == SystemType::setup) {
            setupFunc = std::move(other.setupFunc);
        } else if (other.type == SystemType::tick) {
            tickFunc = std::move(other.tickFunc);
        } else if (other.type == SystemType::event) {
            onReceiveFunc = std::move(other.onReceiveFunc);
        }
    }

    void ECSHost::Setup()
    {
        setuped = true;

        tf::Taskflow taskflow;
        std::unordered_map<SystemSignature, tf::Task> tasks;
        tasks.reserve(setupSystems.size());

        SystemCommands systemCommands(*this);
        for (const auto& system : setupSystems) {
            const auto& systemInstance = systemInstances.at(system);
            Assert(systemInstance.type == SystemType::setup);

            tasks.emplace(std::make_pair(system, taskflow.emplace([&]() -> void {
                systemInstance.setupFunc(systemCommands);
            })));
        }

        for (const auto& dependencies : setupSystemDependencies) {
            auto& task = tasks.at(dependencies.first);
            for (const auto& depend : dependencies.second) {
                task.succeed(tasks.at(depend));
            }
        }
    }

    void ECSHost::Tick(float timeMS)
    {
        Assert(setuped);

        tf::Taskflow taskflow;
        std::unordered_map<SystemSignature, tf::Task> tasks;
        tasks.reserve(tickSystems.size());

        SystemCommands systemCommands(*this);
        for (const auto& system : tickSystems) {
            const auto& systemInstance = systemInstances.at(system);
            Assert(systemInstance.type == SystemType::tick);

            tasks.emplace(std::make_pair(system, taskflow.emplace([&]() -> void {
                systemInstance.tickFunc(systemCommands, timeMS);
            })));
        }

        for (const auto& dependencies : tickSystemDependencies) {
            auto& task = tasks.at(dependencies.first);
            for (const auto& depend : dependencies.second) {
                task.succeed(tasks.at(depend));
            }
        }
    }

    void ECSHost::Shutdown()
    {
        Assert(setuped);
        setuped = false;
    }
}
