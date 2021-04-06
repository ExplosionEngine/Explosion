//
// Created by Administrator on 2021/4/3 0003.
//

#include <Explosion/Core/Ecs/System.h>

namespace Explosion {
    System::System() = default;

    void System::Update(Registry& registry)
    {
        OnUpdate(registry);
    }
}
