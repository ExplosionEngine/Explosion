//
// Created by LiZhen on 2021/8/23.
//

#ifndef EXPLOSION_ALLOCATORS_H
#define EXPLOSION_ALLOCATORS_H

#include <Common/Templates/FixedSizeAllocator.h>
#include <Common/Templates/LinkedFreeList.h>

namespace Explosion {

    template <int N, int BlockSize>
    using FreeListFixedSizeAllocator = FixedSizeAllocator<N, LinkedFreeList<N, BlockSize>, uint32_t>;

}

#endif