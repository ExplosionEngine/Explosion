//
// Created by johnk on 2023/4/4.
//

#include <Runtime/Component/EntityInfo.h>

namespace Runtime {
    EntityInfoComponent::EntityInfoComponent() = default;

    EntityInfoComponent::EntityInfoComponent(std::string inName) : name(std::move(inName)) {}
}
