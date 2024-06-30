//
// Created by johnk on 2023/9/1.
//

#pragma once

#include <Common/Memory.h>
#include <Core/Module.h>
#include <Runtime/Api.h>
#include <Runtime/Engine.h>
#include <Runtime/World.h>

namespace Runtime {
    struct RuntimeModuleInitParams {
        bool isEditor;
    };

    class RUNTIME_API RuntimeModule final : public Core::Module {
    public:
        RuntimeModule();
        ~RuntimeModule() override;

        void Initialize(const RuntimeModuleInitParams& inParams);
        Engine* GetEngine() const;
        Common::UniqueRef<World> CreateWorld(const std::string& inName) const;

    private:
        void CreateEngine();

        bool initialized;
        Common::UniqueRef<Engine> engine;
    };
}
