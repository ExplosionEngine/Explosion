//
// Created by johnk on 2024/8/21.
//

#pragma once

#include <unordered_set>

#include <Core/Module.h>
#include <Runtime/Api.h>

namespace Runtime {
    class World;

    struct EngineInitParams {
        std::string projectFile;
    };

    class RUNTIME_API Engine { // NOLINT
    public:
        virtual ~Engine();

        virtual bool IsEditor() = 0;

        void MountWorld(World* inWorld);
        void UnmountWorld(World* inWorld);

    protected:
        explicit Engine(const EngineInitParams& inParams);

        std::unordered_set<World*> worlds;
    };

    struct RUNTIME_API IGameModule : Core::Module { // NOLINT
        virtual Engine* CreateEngine(const EngineInitParams&) = 0;
    };

    class RUNTIME_API EngineHolder {
    public:
        static void Load(const std::string& inModuleName, const EngineInitParams& inInitParams);
        static void Unload();
        static Engine& Get();

    private:
        static Engine* engine;
    };
}
