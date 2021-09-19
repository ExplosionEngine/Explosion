//
// Created by Zach Lee on 2021/8/23.
//

#ifndef EXPLOSION_ALLOCATORS_H
#define EXPLOSION_ALLOCATORS_H

#include <Common/Templates/FixedSizeAllocator.h>
#include <Common/Templates/LinkedFreeList.h>
#include <Common/Templates/TypeTraits.h>

namespace Explosion {

    template <int Size, int NumPerBlock>
    using FreeListFixedSizeAllocator =
        FixedSizeAllocator<Size, LinkedFreeList<Size, NumPerBlock>, Counter<uint32_t>>;

}

#endif