//
// Created by Zach Lee on 2021/8/23.
//

#ifndef EXPLOSION_FIXED_SIZE_ALLOCATORS_H
#define EXPLOSION_FIXED_SIZE_ALLOCATORS_H

namespace Explosion {

    template <int Size, class Container, typename Counter>
    class FixedSizeAllocator {
    public:
        FixedSizeAllocator() = default;
        ~FixedSizeAllocator() = default;

        static constexpr int FIXED_SIZE = Size;
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