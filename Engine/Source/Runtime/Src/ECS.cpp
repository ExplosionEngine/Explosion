//
// Created by johnk on 2023/9/5.
//

#include <Runtime/ECS.h>

namespace Runtime::Internal {
    SystemSignature Internal::SignForClass(const Mirror::Class& clazz)
    {
        SystemSignature result;
        result.canReflect = true;
        result.typeId = clazz.GetTypeInfo()->id;
        result.name = clazz.GetName();
        return result;
    }
}

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

    bool SystemCommands::Valid(Entity inEntity) const
    {
        return registry.valid(inEntity);
    }
}
