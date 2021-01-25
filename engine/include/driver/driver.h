//
// Created by John Kindem on 2021/1/9 0009.
//

#ifndef EXPLOSION_DRIVER_H
#define EXPLOSION_DRIVER_H

#include <memory>

#include <driver/swap-chain.h>
#include <driver/render-pass.h>
#include <driver/pipeline.h>

namespace Explosion {
    class Driver {
    public:
        virtual ~Driver() = 0;
        virtual std::shared_ptr<SwapChain> CreateSwapChain(void* surface, uint32_t width, uint32_t height) = 0;
        virtual std::shared_ptr<RenderPass> CreateRenderPass(const RenderPass::Descriptor& descriptor) = 0;
        virtual std::shared_ptr<Pipeline> CreatePipeline(const Pipeline::Descriptor& descriptor) = 0;

    protected:
        Driver() = default;

    private:
    };
}

#endif //EXPLOSION_DRIVER_H
