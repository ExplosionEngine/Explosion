//
// Created by johnk on 2024/8/21.
//

#pragma once

#include <unordered_set>

#include <Core/Module.h>
#include <Runtime/Api.h>
#include <Render/RenderModule.h>

namespace Runtime {
    class World;

    struct EngineInitParams {
        std::string projectFile;
        std::string rhiType;
    };

    class RUNTIME_API Engine { // NOLINT
    public:
        virtual ~Engine();

        virtual bool IsEditor() = 0;

        void MountWorld(World* inWorld);
        void UnmountWorld(World* inWorld);
        Render::RenderModule& GetRenderModule() const;
        void Tick(float inTimeMs) const;

    protected:
        explicit Engine(const EngineInitParams& inParams);

        std::unordered_set<World*> worlds;
        Render::RenderModule* renderModule;
    };

    class RUNTIME_API MinEngine final : public Engine {
    public:
        explicit MinEngine(const EngineInitParams& inParams);
        ~MinEngine() override;
        
        bool IsEditor() override;
    };

    struct RUNTIME_API IGameModule : Core::Module { // NOLINT
        virtual Engine* CreateEngine(const EngineInitParams& inParams) = 0;
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
