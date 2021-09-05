//
// Created by LiZhen on 2021/8/23.
//

#ifndef EXPLOSION_FIXED_SIZE_ALLOCATORS_H
#define EXPLOSION_FIXED_SIZE_ALLOCATORS_H

namespace Explosion {

    template <int N, class Container, typename Counter>
    class FixedSizeAllocator {
    public:
        static constexpr int FIXED_SIZE = N;
        using ContainerType = Container;
        using CounterType = Counter;

        inline void* Allocate()
        {
            used.template Inc();
            return container.template Pop();
        }

        inline void Free(void* ptr)
        {
            container.template Push(ptr);
            used.template Dec();
        }

    private:
        ContainerType container;
        CounterType used;
    };

}

#endif