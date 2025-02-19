//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <cstdint>
#include <string>

#include <Common/Utility.h>
#include <Runtime/ECS.h>
#include <Runtime/Client.h>
#include <Runtime/Asset/Level.h>

namespace Runtime {
    enum class PlayStatus : uint8_t {
        stopped,
        playing,
        paused,
        max
    };

    class RUNTIME_API World {
    public:
        NonCopyable(World)
        NonMovable(World)
        ~World();

        World(std::string inName, Client* inClient);
        void SetSystemGraph(const SystemGraph& inSystemGraph);
        PlayStatus PlayStatus() const;
        bool Stopped() const;
        bool Playing() const;
        bool Paused() const;
        void Play();
        void Resume();
        void Pause();
        void Stop();
        void LoadFrom(AssetPtr<Level> inLevel);
        void SaveTo(AssetPtr<Level> inLevel);

    private:
        friend class Engine;

        void Tick(float inDeltaTimeSeconds);

        std::string name;
        Runtime::PlayStatus playStatus;
        SystemSetupContext systemSetupContext;
        ECRegistry ecRegistry;
        SystemGraph systemGraph;
        std::optional<SystemGraphExecutor> executor;
    };
}

