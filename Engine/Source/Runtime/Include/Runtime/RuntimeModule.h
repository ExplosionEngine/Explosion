//
// Created by johnk on 2023/9/1.
//

#pragma once

#include <Common/Memory.h>
#include <Core/Module.h>
#include <Runtime/Api.h>
#include <Runtime/World.h>
#include <Runtime/Engine.h>

namespace Runtime {
    struct RuntimeModuleInitParams {
        bool isEditor;
    };

    class RUNTIME_API RuntimeModule : public Core::Module {
    public:
        RuntimeModule();
        ~RuntimeModule() override;

        void Initialize(const RuntimeModuleInitParams& inParams);
        Engine* GetEngine();
        World* CreateWorld(const std::string& inName = "") const;
        void DestroyWorld(World* inWorld) const;

    private:
        void CreateEngine();

        bool initialized;
        Common::UniqueRef<Engine> engine;
    };
}
