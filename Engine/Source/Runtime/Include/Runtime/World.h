//
// Created by johnk on 2024/6/29.
//

#pragma once

#include <unordered_map>

#include <Mirror/Meta.h>
#include <Runtime/GameObject.h>

namespace Runtime {
    enum class WorldStatus : uint8_t {
        playing,
        paused,
        stoped,
        max
    };

    class EClass() World {
        EClassBody(World)

        ECtor() World();
        ECtor() explicit World(std::string inName);

        EFunc() const std::string& GetName() const;
        EFunc() GameObject& CreateObject(const std::string& inName);
        EFunc() GameObject& GetObject(const std::string& inName) const;
        EFunc() void DestroyObject(const std::string& inName);

        EFunc() void Play();
        EFunc() void Tick(float frameTimeMs);
        EFunc() void Pause();
        EFunc() void Continue();
        EFunc() void Stop();

        // TODO load level
        // TODO unload level
        // TODO save level

    private:
        void BroadcastGameObjects(const std::function<void(GameObject&)>& func);

        EFunc() void OnDeserialize();

        EProperty(transient) WorldStatus status;
        EProperty() std::string name;
        EProperty() std::unordered_map<std::string, GameObjectRef> gameObjects;
    };
}
