//
// Created by johnk on 2023/9/5.
//

#include <Runtime/ECS.h>

namespace Runtime {
    SystemCommands::SystemCommands(entt::registry& inRegistry, ECSHost& inHost)
        : registry(inRegistry)
        , host(inHost)
    {
    }

    SystemCommands::~SystemCommands() = default;

    Entity SystemCommands::AddEntity()
    {
        return registry.create();
    }

    void SystemCommands::DestroyEntity(Entity inEntity)
    {
        registry.destroy(inEntity);
    }
}
