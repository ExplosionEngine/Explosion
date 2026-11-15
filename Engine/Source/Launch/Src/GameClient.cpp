#include <Launch/GameClient.h>

namespace Launch {
    GameClient::GameClient(Runtime::World& inWorld)
        : world(inWorld)
    {
    }

    GameClient::~GameClient() = default;

    Runtime::World& GameClient::GetWorld()
    {
        return world;
    }
}
