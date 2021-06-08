//
// Created by John Kindem on 2021/6/8.
//

#include <Explosion/World/ECS/System.h>

namespace Explosion::ECS {
    System::System(Registry& registry) : registry(registry) {}

    System::~System() = default;
}
