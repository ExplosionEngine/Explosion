#pragma once

#include <Runtime/Client.h>

namespace Launch {
    class GameClient final : public Runtime::Client {
    public:
        explicit GameClient(Runtime::World& inWorld);
        ~GameClient() override;

        Runtime::World& GetWorld() override;

    private:
        Runtime::World& world;
    };
}
