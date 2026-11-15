#pragma once

#include <Runtime/Client.h>

namespace Editor {
    class SceneClient final : public Runtime::Client {
    public:
        explicit SceneClient(Runtime::World& inWorld);
        ~SceneClient() override;

        Runtime::World& GetWorld() override;

    private:
        Runtime::World& world;
    };
}
