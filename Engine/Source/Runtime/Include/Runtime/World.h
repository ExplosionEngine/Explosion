//
// Created by johnk on 2022/8/8.
//

#pragma once

#include <entt/entt.hpp>
#include <taskflow/taskflow.hpp>

#include <utility>
#include <future>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Runtime/ECS.h>
#include <Runtime/Asset.h>

namespace Runtime {
    class World {
    public:
        NON_COPYABLE(World)
        explicit World(std::string inName) : setup(false), name(std::move(inName)) {}

        ~World()
        {
            for (auto& systemInfo : systemInfos) {
                delete systemInfo.system;
            }
        }

        Entity CreateEntity()
        {
            return registry.create();
        }

        void DestroyEntity(Entity entity)
        {
            registry.destroy(entity);
        }

        template <typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args)
        {
            return registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        }

        template <typename T>
        T* GetComponent(Entity entity)
        {
            return registry.try_get<T>(entity);
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            registry.remove<T>(entity);
        }

        template <typename... Components>
        Query<Components...> CreateQuery(ComponentSet<Components...> = {})
        {
            return Query<Components...>(std::move(registry.view<Components...>()));
        }

        template <typename S>
        void AddSystem(S* system)
        {
            using SetupQueryType = typename SystemFuncTraits<decltype(&S::Setup)>::QueryTuple;
            using TickQueryType = typename SystemFuncTraits<decltype(&S::Tick)>::QueryTuple;

            SystemInfo systemInfo;
            systemInfo.system = system;
            systemInfo.setupFunc = MakeSystemFuncProxy<S, SetupQueryType>(system, &S::Setup, std::make_index_sequence<std::tuple_size_v<SetupQueryType>> {});
            systemInfo.tickFunc = MakeSystemFuncProxy<S, TickQueryType>(system, &S::Tick, std::make_index_sequence<std::tuple_size_v<TickQueryType>> {});
            systemInfos.emplace_back(std::move(systemInfo));
        }

        void Setup()
        {
            if (setup) {
                return;
            }
            DispatchSystemTasksAndWait([](const SystemInfo& systemInfo) -> auto{ return systemInfo.setupFunc; });
        }

        void Tick()
        {
            DispatchSystemTasksAndWait([](const SystemInfo& systemInfo) -> auto{ return systemInfo.tickFunc; });
        }

        [[nodiscard]] const std::vector<System*>& EngineSystems() const
        {
            return engineSystems;
        }

        template <typename S>
        S* FindEngineSystem()
        {
            auto iter = engineSystemMap.find(typeid(S).hash_code());
            return iter == engineSystemMap.end() ? nullptr : iter->second;
        }

    private:
        struct SystemInfo {
            System* system;
            std::function<void()> setupFunc;
            std::function<void()> tickFunc;
        };

        template <typename S, typename QueryTuple, typename F, size_t... I>
        std::function<void()> MakeSystemFuncProxy(S* system, F func, std::index_sequence<I...> = {})
        {
            return [system, func, this]() -> void { (system->*func)(CreateQuery(typename QueryTraits<std::tuple_element_t<I, QueryTuple>>::ComponentSet {})...); };
        }

        template <typename F>
        void DispatchSystemTasksAndWait(F&& taskGetter)
        {
            std::unordered_map<System*, tf::Task> tasks;
            tf::Taskflow taskflow;
            {
                for (const auto& systemInfo : systemInfos) {
                    tasks[systemInfo.system] = taskflow.emplace(taskGetter(systemInfo));
                }
                for (const auto& systemInfo : systemInfos) {
                    auto& task = tasks[systemInfo.system];

                    const auto& systemsToWait = systemInfo.system->GetSystemsToWait();
                    for (auto* systemToWait : systemsToWait) {
                        auto iter = tasks.find(systemToWait);
                        Assert(iter != tasks.end());
                        iter->second.succeed(task);
                    }
                }
            }
            tf::Executor executor;
            executor.run(taskflow);
        }

        bool setup;
        std::string name;
        entt::registry registry;
        std::vector<SystemInfo> systemInfos;
        std::vector<System*> engineSystems;
        std::unordered_map<size_t, System*> engineSystemMap;
    };
}
