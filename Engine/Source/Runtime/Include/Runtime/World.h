//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <cstdint>
#include <string>

#include <Common/Utility.h>
#include <Runtime/ECS.h>

namespace Runtime {
    enum class PlayStatus : uint8_t {
        stopped,
        playing,
        paused,
        max
    };

    class World {
    public:
        NonCopyable(World)
        NonMovable(World)
        ~World();

    private:
        friend class Engine;

        explicit World(const std::string& inName = "");

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
        void Tick(float inTimeMs);

        std::string name;
        Runtime::PlayStatus playStatus;
        ECRegistry ecRegistry;
        SystemGraph systemGraph;
        std::optional<SystemGraphExecutor> executor;
    };
}

