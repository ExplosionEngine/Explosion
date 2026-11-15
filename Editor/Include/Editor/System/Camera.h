#pragma once

#include <Common/Math/Vector.h>
#include <Runtime/ECS.h>
#include <Runtime/Meta.h>

namespace Editor {
    struct EClass(comp, transient) EditorCameraInput final {
        EClassBody(EditorCameraInput)

        EditorCameraInput();

        bool moveForward;
        bool moveBackward;
        bool moveLeft;
        bool moveRight;
        bool looking;
        Common::FVec2 lookDelta;
    };

    struct EClass(comp, transient) EditorCameraController final {
        EClassBody(EditorCameraController)

        EditorCameraController();

        bool anglesInitialized;
        float yaw;
        float pitch;
    };

    class EClass() EditorCameraSystem final : public Runtime::System {
        EPolyDerivedClassBody(EditorCameraSystem)

    public:
        explicit EditorCameraSystem(Runtime::ECRegistry& inRegistry, const Runtime::SystemSetupContext& inContext);
        ~EditorCameraSystem() override;

        NonCopyable(EditorCameraSystem)
        NonMovable(EditorCameraSystem)

        void Tick(float inDeltaTimeSeconds) override;
    };
}
