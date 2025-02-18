//
// Created by johnk on 2025/2/18.
//

#pragma once

#include <RHI/Texture.h>
#include <RHI/Synchronous.h>

namespace Runtime {
    struct PresentInfo {
        PresentInfo();

        RHI::Texture* backBuffer;
        RHI::Semaphore* imageReadySemaphore;
        RHI::Semaphore* renderFinishedSemaphore;
    };

    class Viewport {
    public:
        virtual ~Viewport();
        virtual PresentInfo GetNextPresentInfo() = 0;

        // TODO mouse keyboard inputs etc.

    protected:
        Viewport();
    };
}
