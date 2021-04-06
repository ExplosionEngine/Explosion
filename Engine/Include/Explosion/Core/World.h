//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_WORLD_H
#define EXPLOSION_WORLD_H

#include <string>
#include <memory>
#include <unordered_map>

#include <Explosion/Core/Ecs/Registry.h>
#include <Explosion/Core/Ecs/System.h>

namespace Explosion {
    class World {
    public:
        explicit World(std::string name);
        ~World();

        Entity CreateEntity();
        void DestroyEntity(const Entity& entity);

        template <typename CompType>
        CompType& AddComponent()
        {
            return registry.AddComponent<CompType>();
        }

        template <typename CompType>
        CompType& GetComponent()
        {
            return registry.GetComponent<CompType>();
        }

        template <typename CompType>
        void RemoveComponent()
        {
            registry.RemoveComponent<CompType>();
        }

        template <typename CompType>
        bool HasComponent()
        {
            return registry.HasComponent<CompType>();
        }

        template <typename SystemType>
        void MountSystem()
        {
            systems[typeid(SystemType).hash_code()] = std::make_unique<SystemType>();
        }

        template <typename SystemType>
        void UnmountSystem()
        {
            SystemType type = typeid(SystemType).hash_code();
            auto iter = systems.find(type);
            if (iter == systems.end()) {
                return;
            }
            systems.erase(iter);
        }

        void Update();

    private:
        void UpdateSystems();

        std::string name;

        entt::registry originRegistry;
        Registry registry;

        std::unordered_map<SystemType, std::unique_ptr<System>> systems;
    };
}

#endif //EXPLOSION_WORLD_H
