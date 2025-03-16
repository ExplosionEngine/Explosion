//
// Created by johnk on 2025/3/4.
//

#pragma once

#include <Runtime/ECS.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() RenderSystem final : public System {
        EPolyClassBody(RenderSystem)

    public:
        explicit RenderSystem(ECRegistry& inRegistry, const SystemSetupContext& inContext);
        ~RenderSystem() override;

        NonCopyable(RenderSystem)
        NonMovable(RenderSystem)

        void Tick(float inDeltaTimeSeconds) override;

    private:
        // TODO
    };
}
