//
// Created by johnk on 2023/9/5.
//

#pragma once

#include <Common/Memory.h>
#include <Runtime/ECS.h>

namespace Runtime {
    class World;

    class RUNTIME_API World : public ECSHost {
    public:
        explicit World(std::string inName = "");
        ~World();

        void Setup() override;
        void Tick(float timeMS) override;
        void Shutdown() override;

    private:
        std::string name;
    };
}
