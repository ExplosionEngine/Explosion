//
// Created by johnk on 2024/6/29.
//

#include <Runtime/Component.h>

namespace Runtime {
    ComponentStorage::ComponentStorage() = default;

    ComponentStorage::~ComponentStorage() = default;

    ComponentStorage::ComponentStorage(ComponentStorage&& inOther) noexcept
        : clazz(inOther.clazz)
        , storage(std::move(inOther.storage))
    {
    }

    ComponentStorage::ComponentStorage(const ComponentStorage& inOther) = default;

    ComponentStorage& ComponentStorage::operator=(const ComponentStorage& inOther) = default;
}
