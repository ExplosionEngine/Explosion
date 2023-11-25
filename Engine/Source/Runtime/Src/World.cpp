//
// Created by johnk on 2023/9/5.
//

#include <taskflow/taskflow.hpp>

#include <Runtime/World.h>

namespace Runtime {
    World::World(std::string inName)
        : ECSHost()
        , name(std::move(inName))
    {
    }

    World::~World() = default;

    void World::Setup()
    {
        ECSHost::Setup();
    }

    void World::Tick(float timeMS)
    {
        ECSHost::Tick(timeMS);
    }

    void World::Shutdown()
    {
        ECSHost::Shutdown();
    }

    bool World::Setuped()
    {
        return ECSHost::Setuped();
    }

    void World::Reset()
    {
        ECSHost::Reset();
    }

    void World::LoadFromLevel(const AssetRef<Level>& level)
    {
        Assert(!Setuped());
        for (const auto& systemClassName : level->systems) {
            const auto* systemType = SystemTypeFinder::FromSystemClassName(systemClassName);
            systemType->add(*this);
        }
    }

    void World::SaveToLevel(AssetRef<Level>& level)
    {
        auto addSystem = [&](SystemSignature signature) -> void {
            Assert(signature.type == ClassSignatureType::staticClass);
            Assert(!level->systems.contains(signature.name));
            level->systems.emplace(signature.name);
        };

        level->systems.clear();
        for (const auto& system : setupSystems) {
            addSystem(system);
        }
        for (const auto& system : tickSystems) {
            addSystem(system);
        }
        for (const auto& pair : eventSystems) {
            for (const auto& system : pair.second) {
                addSystem(system);
            }
        }
    }
}
