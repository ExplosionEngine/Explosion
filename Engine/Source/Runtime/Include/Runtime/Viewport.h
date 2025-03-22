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

        RHI::Texture* backBuffer;
        RHI::Semaphore* imageReadySemaphore;
        RHI::Semaphore* renderFinishedSemaphore;
    };

    class Viewport {
    public:
        virtual ~Viewport();

        virtual Client& GetClient() = 0;
        virtual PresentInfo GetNextPresentInfo() = 0;
        virtual size_t GetWidth() const = 0;
        virtual size_t GetHeight() const = 0;
        // TODO mouse keyboard inputs etc.

    protected:
        Viewport();
    };
}
