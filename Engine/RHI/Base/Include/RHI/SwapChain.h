//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_SWAPCHAIN_H
#define EXPLOSION_SWAPCHAIN_H

#include <functional>

#include <RHI/Enum.h>

namespace Explosion::RHI {
    class Signal;
    class Image;

    using FrameJob = std::function<void(uint32_t, Signal*, Signal*)>;

    class SwapChain {
    public:
        struct Config {
            void* surface;
            uint32_t width;
            uint32_t height;
        };

        virtual ~SwapChain();
        virtual void DoFrame(const FrameJob& frameJob) = 0;
        virtual uint32_t GetColorAttachmentCount() = 0;
        virtual Format GetSurfaceFormat() = 0;
        virtual std::vector<Image*> GetColorAttachments() = 0;

    protected:
        explicit SwapChain(Config config);

        Config config;
    };
}

#endif //EXPLOSION_SWAPCHAIN_H
