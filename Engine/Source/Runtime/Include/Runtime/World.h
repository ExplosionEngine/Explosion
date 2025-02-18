//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <cstdint>
#include <string>

#include <Common/Utility.h>
#include <Runtime/ECS.h>
#include <Runtime/Client.h>

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
        void Reset();
        PlayStatus PlayStatus() const;
        bool Stopped() const;
        bool Playing() const;
        bool Paused() const;
        void Play();
        void Resume();
        void Pause();
        void Stop();

    private:
        friend class Engine;

        void Tick(float inDeltaTimeSeconds);

        std::string name;
        Client* client;
        Runtime::PlayStatus playStatus;
        ECRegistry ecRegistry;
        SystemGraph systemGraph;
        std::optional<SystemGraphExecutor> executor;
    };
}

