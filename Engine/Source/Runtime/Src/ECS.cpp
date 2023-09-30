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

    Entity SystemCommands::Create()
    {
        return registry.create();
    }

    void SystemCommands::Destroy(Entity inEntity)
    {
        registry.destroy(inEntity);
    }
}
