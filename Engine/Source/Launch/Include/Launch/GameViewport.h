//
// Created by johnk on 2025/2/19.
//

#pragma once

#include <Runtime/Viewport.h>

namespace Launch {
    class GameViewport : public Runtime::Viewport {
    public:
        GameViewport();
        ~GameViewport() override;

        Runtime::Client& GetClient() override;
        Runtime::PresentInfo GetNextPresentInfo() override;
        size_t GetWidth() override;
        size_t GetHeight() override;

    private:
        // TODO glfw window
        // TODO rhi swap chain
        // TODO game client
    };
}
