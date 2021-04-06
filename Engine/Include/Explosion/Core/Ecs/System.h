//
// Created by Administrator on 2021/4/3 0003.
//

#ifndef EXPLOSION_SYSTEM_H
#define EXPLOSION_SYSTEM_H

#include <Explosion/Core/Ecs/Registry.h>

namespace Explosion {
    using SystemType = size_t;

    class System {
    public:
        virtual ~System() = 0;
        void Update(Registry& registry);

    protected:
        System();
        virtual void OnUpdate(Registry& registry) = 0;

    private:
    };
}

#endif //EXPLOSION_SYSTEM_H
