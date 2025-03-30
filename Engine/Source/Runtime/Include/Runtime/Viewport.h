//
// Created by johnk on 2025/2/18.
//

#pragma once

#include <RHI/Texture.h>
#include <RHI/Synchronous.h>

namespace Runtime {
    class Client;

    struct PresentInfo {
        PresentInfo();

        RHI::Texture* backTexture;
        RHI::Semaphore* imageReadySemaphore;
        RHI::Semaphore* renderFinishedSemaphore;
    };

    class Viewport {
    public:
        virtual ~Viewport();

        virtual Client& GetClient() = 0;
        virtual PresentInfo GetNextPresentInfo() = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual void Resize(uint32_t inWidth, uint32_t inHeight) = 0;
        // TODO mouse keyboard inputs etc.

    protected:
        Viewport();
    };
}
