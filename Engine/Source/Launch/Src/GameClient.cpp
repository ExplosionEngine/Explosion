//
// Created by johnk on 2025/2/19.
//

#include <Launch/GameClient.h>
#include <Launch/GameViewport.h>

namespace Launch {
    GameClient::GameClient(GameViewport& inViewport)
        : viewport(inViewport)
        , world("GameWorld", this, Runtime::PlayType::game)
    {
    }

    GameClient::~GameClient() = default;

    Runtime::Viewport& GameClient::GetViewport()
    {
        return viewport;
    }

    Runtime::World& GameClient::GetWorld()
    {
        return world;
    }
}
