//
// Created by johnk on 2022/8/8.
//

#pragma once

#include <entt/entt.hpp>
#include <taskflow/taskflow.hpp>

#include <utility>
#include <future>

#include <Common/Debug.h>
#include <Common/Memory.h>
#include <Common/Utility.h>
#include <Mirror/Type.h>
#include <Runtime/Component/EntityInfo.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class World {
    public:
        NonCopyable(World)

        explicit World(std::string inName) : alreadySetup(false), name(std::move(inName))
        {
            RegisterEngineComponents();
            MountEngineSystems();
        }

        ~World() = default;

        template <typename T>
        void RegisterComponentType()
        {
            std::string typeName = Mirror::Class::Get<T>().GetName();
            Assert(!componentTypes.contains(typeName));
            componentTypes.emplace(std::make_pair(typeName, Common::UniqueRef<IComponentLifecycleCallbackProxy>(new ComponentLifecycleCallbackProxy<T>)));
        }

        Entity CreateEntity(const std::string& inName = "")
        {
            Entity result = registry.create();
            (void) AddComponent<TransformComponent>(result);
            auto& entityInfo = AddComponent<EntityInfoComponent>(result);
            entityInfo.SetName(inName);
            return result;
        }

        void DestroyEntity(Entity entity)
        {
            registry.destroy(entity);
        }

        template <typename T, typename... Args>
        T& AddComponent(Entity entity, Args&&... args)
        {
            VerifyComponentTypeRegistered<T>();
            T& result = registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
            result.entity = entity;
            result.world = this;
            return result;
        }

        template <typename T>
        T* GetComponent(Entity entity)
        {
            VerifyComponentTypeRegistered<T>();
            return registry.try_get<T>(entity);
        }

        template <typename T>
        void RemoveComponent(Entity entity)
        {
            VerifyComponentTypeRegistered<T>();
            registry.remove<T>(entity);
        }

        template <typename... Components>
        Query<Components...> CreateQuery(ComponentSet<Components...> = {})
        {
            VerifyComponentTypesRegistered<Components...>();
            return Query<Components...>(std::move(registry.view<Components...>()));
        }

        template <typename S, typename... Args>
        void MountSystem(Args&&... args)
        {
            using SetupQueryType = typename SystemFuncTraits<decltype(&S::Setup)>::QueryTuple;
            using TickQueryType = typename SystemFuncTraits<decltype(&S::Tick)>::QueryTuple;
            using WaitSystemType = typename S::WaitSystemTypes;

            auto systemTypeName = Mirror::Class::Get<S>().GetName();
            Assert(!systems.contains(systemTypeName));

            SystemInstance systemInstance;
            systemInstance.instance = new S(std::forward<Args>(args)...);
            systemInstance.setupFunc = MakeSystemFuncProxy<S, SetupQueryType>(static_cast<S*>(systemInstance.instance.Get()), &S::Setup, std::make_index_sequence<std::tuple_size_v<SetupQueryType>> {});
            systemInstance.tickFunc = MakeSystemFuncProxy<S, TickQueryType>(static_cast<S*>(systemInstance.instance.Get()), &S::Tick, std::make_index_sequence<std::tuple_size_v<TickQueryType>> {});
            systems.emplace(systemTypeName, std::move(systemInstance));

            auto& system = systems.at(systemTypeName);
            std::vector<System*> dependencies = engineSystems;
            dependencies.reserve(dependencies.size() + std::tuple_size_v<WaitSystemType>);
            AddCustomDependenciesForSystem(dependencies, WaitSystemType {});
            systemDependencies.emplace(std::make_pair(system.instance.Get(), std::move(dependencies)));
        }

        void Setup()
        {
            Assert(!alreadySetup);
            DispatchSystemTasksAndWait([](const SystemInstance& systemInstance) -> auto{ return systemInstance.setupFunc; });
            alreadySetup = true;
        }

        void Tick()
        {
            DispatchSystemTasksAndWait([](const SystemInstance& systemInstance) -> auto{ return systemInstance.tickFunc; });
        }

    private:
        struct IComponentLifecycleCallbackProxy {
            virtual void OnConstruct(entt::registry& reg, entt::entity entity) = 0;
            virtual void OnDestroy(entt::registry& reg, entt::entity entity) = 0;
        };

        template <typename T>
        struct ComponentLifecycleCallbackProxy : public IComponentLifecycleCallbackProxy {
            void OnConstruct(entt::registry& reg, entt::entity entity) override
            {
                reg.get<T>(entity).OnConstruct();
            }

            void OnDestroy(entt::registry &reg, entt::entity entity) override
            {
                reg.get<T>(entity).OnDestroy();
            }
        };

        struct SystemInstance {
            Common::UniqueRef<System> instance;
            std::function<void()> setupFunc;
            std::function<void()> tickFunc;

            SystemInstance() = default;

            SystemInstance(SystemInstance&& inOther) noexcept
                : instance(std::move(inOther.instance))
                , setupFunc(std::move(inOther.setupFunc))
                , tickFunc(std::move(inOther.tickFunc))
            {
            }

            SystemInstance(const SystemInstance& inOther)
            {
                Assert(false);
            }
        };

        template <typename T>
        void VerifyComponentTypeRegistered()
        {
            std::string typeName = Mirror::Class::Get<T>().GetName();
            Assert(componentTypes.contains(typeName));
        }

        template <typename... TS>
        void VerifyComponentTypesRegistered()
        {
            (void) std::initializer_list<int> { ([this]() -> void { VerifyComponentTypeRegistered<TS>(); }(), 0)... };
        }

        template <typename... Args>
        void AddCustomDependenciesForSystem(std::vector<System*>& dependencies, std::tuple<Args...>)
        {
            (void) std::initializer_list<int> { ([&, this]() -> void {
                std::string typeName = Mirror::Class::Get<Args>().GetName();
                Assert(systems.contains(typeName));
                dependencies.emplace_back(systems.at(typeName).instance.Get());
            }(), 0)... };
        }

        template <typename S, typename QueryTuple, typename F, size_t... I>
        std::function<void()> MakeSystemFuncProxy(S* system, F func, std::index_sequence<I...> = {})
        {
            return [system, func, this]() -> void { (system->*func)(CreateQuery(typename QueryTraits<std::tuple_element_t<I, QueryTuple>>::ComponentSet {})...); }; // NOLINT
        }

        template <typename F>
        void DispatchSystemTasksAndWait(F&& taskGetter)
        {
            std::unordered_map<System*, tf::Task> tasks;
            tf::Taskflow taskflow;
            {
                for (const auto& pair : systems) {
                    const auto& system = pair.second;
                    tasks[system.instance.Get()] = taskflow.emplace(taskGetter(system));
                }
                for (const auto& pair : systems) {
                    const auto& system = pair.second;
                    auto& task = tasks[system.instance.Get()];

                    Assert(systemDependencies.contains(system.instance.Get()));
                    const auto& systemsToWait = systemDependencies.at(system.instance.Get());
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

        void RegisterEngineComponents()
        {
            RegisterComponentType<EntityInfoComponent>();
            RegisterComponentType<TransformComponent>();
        }

        void MountEngineSystems()
        {
            // TODO
        }

        bool alreadySetup;
        std::string name;
        entt::registry registry;
        std::unordered_map<std::string, Common::UniqueRef<IComponentLifecycleCallbackProxy>> componentTypes;
        std::unordered_map<std::string, SystemInstance> systems;
        std::unordered_map<System*, std::vector<System*>> systemDependencies;
        std::vector<System*> engineSystems;
    };
}
