//
// Created by johnk on 2025/1/21.
//

#pragma once

#include <Mirror/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() TransformSystem final : public System {
        EPolyClassBody(TransformSystem)

    public:
        explicit TransformSystem(ECRegistry& inRegistry);
        ~TransformSystem() override;

        NonCopyable(TransformSystem)
        NonMovable(TransformSystem)

        void Tick(float inDeltaTimeMs) override;

    private:
        Observer worldTransformUpdatedObserver;
        Observer localTransformUpdatedObserver;
    };
}
