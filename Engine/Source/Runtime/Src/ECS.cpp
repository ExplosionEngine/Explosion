//
// Created by johnk on 2024/10/31.
//

#include <Runtime/ECS.h>

namespace Runtime {
    CompPacket::~CompPacket() = default;

    CompPacket::CompPacket() = default;

    template <typename C>
    TypedCompPacket<C>::~TypedCompPacket() = default;

    template <typename C>
    TypedCompPacket<C>::TypedCompPacket() = default;
}
