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
}
