//
// Created by johnk on 2025/3/4.
//

#include <Runtime/System/Render.h>

namespace Runtime {
    RenderSystem::RenderSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext)
        : System(inRegistry, inContext)
    {
    }

    RenderSystem::~RenderSystem() = default;

    void RenderSystem::Tick(float inDeltaTimeSeconds)
    {
        // TODO
    }
}
