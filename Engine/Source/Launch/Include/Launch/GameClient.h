//
// Created by johnk on 2025/2/19.
//

#pragma once

#include <Runtime/Client.h>
#include <Runtime/World.h>

namespace Launch {
    class GameViewport;

    class GameClient : public Runtime::Client {
    public:
        explicit GameClient(GameViewport& inViewport);
        ~GameClient() override;

        Runtime::Viewport& GetViewport() override;
        Runtime::World& GetWorld() override;

    private:
        GameViewport& viewport;
        Runtime::World world;
    };
}
