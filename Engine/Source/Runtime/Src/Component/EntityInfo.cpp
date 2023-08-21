//
// Created by johnk on 2023/4/4.
//

#include <Runtime/Component/EntityInfo.h>

namespace Runtime {
    EntityInfoComponent::EntityInfoComponent() = default;

    const std::string& EntityInfoComponent::GetName() const
    {
        return name;
    }

    void EntityInfoComponent::SetName(std::string inName)
    {
        name = std::move(inName);
    }
}
