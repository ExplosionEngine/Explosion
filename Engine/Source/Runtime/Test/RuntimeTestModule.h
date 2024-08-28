//
// Created by johnk on 2024/8/22.
//

#pragma once

#include <Runtime/Engine.h>

class RuntimeTestModule final : public Runtime::IGameModule {
public:
    void OnUnload() override;
    Core::ModuleType Type() const override;
    Runtime::Engine* CreateEngine(const Runtime::EngineInitParams& inParams) override;

private:
    Common::UniqueRef<Runtime::Engine> engine;
};
