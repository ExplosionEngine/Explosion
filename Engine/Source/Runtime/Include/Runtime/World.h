//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <Common/Utility.h>
#include <Runtime/ECS.h>
#include <Runtime/Asset/Level.h>

namespace Runtime {
    class Viewport;

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

        World(std::string inName, PlayType inPlayType);
        void SetSystemGraph(const SystemGraph& inSystemGraph);
        Runtime::PlayStatus PlayStatus() const;
        bool Stopped() const;
        bool Playing() const;
        bool Paused() const;
        void Play();
        void Resume();
        void Pause();
        void Stop();
        bool ShouldTick() const;
        void LoadFrom(AssetPtr<Level> inLevel);
        void SaveTo(AssetPtr<Level> inLevel);
#if BUILD_EDITOR
        void EditorAccess(const std::function<void(ECRegistry&)>& inAccessFunc);
#endif

    private:
        friend class Engine;
        friend class Viewport;

        void Tick(float inDeltaTimeSeconds);

        std::string name;
        Runtime::PlayStatus playStatus;
        SystemSetupContext systemSetupContext;
        ECRegistry ecRegistry;
        SystemGraph systemGraph;
        std::optional<SystemGraphExecutor> executor;
    };
}

