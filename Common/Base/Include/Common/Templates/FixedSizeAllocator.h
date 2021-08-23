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
            used.Inc();
            return container.Pop();
        }

        inline void Free(void* ptr)
        {
            container.Push(ptr);
            used.Dec();
        }

    private:
        ContainerType container;
        CounterType used;
    };

}

#endif