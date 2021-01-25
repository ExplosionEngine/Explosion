//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_SWAP_CHAIN_H
#define EXPLOSION_SWAP_CHAIN_H

namespace Explosion {
    class SwapChain {
    public:
        virtual ~SwapChain() = 0;

    protected:
        SwapChain();
    };
}

#endif //EXPLOSION_SWAP_CHAIN_H
