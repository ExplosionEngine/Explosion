//
// Created by johnk on 2024/6/29.
//

#include <Runtime/World.h>

#include <utility>

namespace Runtime {
    World::World()
        : status(WorldStatus::stoped)
    {
    }

    World::World(std::string inName)
        : name(std::move(inName))
        , status(WorldStatus::stoped)
    {
    }

    const std::string& World::GetName() const
    {
        return name;
    }

    GameObject& World::CreateObject(const std::string& inName)
    {
        Assert(!gameObjects.contains(inName));
        gameObjects.emplace(inName, new GameObject(this, inName));
        auto& result = *gameObjects.at(inName);
        result.OnCreated();
        return result;
    }

    GameObject& World::GetObject(const std::string& inName) const
    {
        Assert(gameObjects.contains(inName));
        return *gameObjects.at(inName);
    }

    void World::DestroyObject(const std::string& inName)
    {
        Assert(gameObjects.contains(inName));
        gameObjects.at(inName)->OnDestroy();
        gameObjects.erase(inName);
    }

    void World::BroadcastGameObjects(const std::function<void(GameObject&)>& func)
    {
        for (const auto& [name, gameObject] : gameObjects) {
            func(*gameObject);
        }
    }

    void World::OnDeserialize()
    {
        BroadcastGameObjects([this](GameObject& gameObject) -> void {
            gameObject.SetWorld(this);
            gameObject.OnLoaded();
        });
    }

    void World::Play()
    {
        Assert(status == WorldStatus::stoped);
        status = WorldStatus::playing;
        BroadcastGameObjects([](GameObject& gameObject) -> void { gameObject.OnStart(); });
    }

    void World::Tick(float frameTimeMs)
    {
        Assert(status == WorldStatus::playing);
        BroadcastGameObjects([&](GameObject& gameObject) -> void { gameObject.OnTick(frameTimeMs); });
    }

    void World::Pause()
    {
        Assert(status == WorldStatus::playing);
        status = WorldStatus::paused;
    }

    void World::Continue()
    {
        Assert(status == WorldStatus::paused);
        status = WorldStatus::playing;
    }

    void World::Stop()
    {
        Assert(status == WorldStatus::playing);
        status = WorldStatus::stoped;
        BroadcastGameObjects([](GameObject& gameObject) -> void { gameObject.OnStop(); });
    }
}
