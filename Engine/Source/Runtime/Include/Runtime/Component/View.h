//
// Created by johnk on 2024/10/14.
//

#pragma once

#include <variant>

#include <Common/Math/Projection.h>
#include <Mirror/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass() Camera final {
        EClassBody(Camera)

        Camera();
        explicit Camera(const Common::FReversedZOrthoProjection& inProjection);
        explicit Camera(const Common::FReversedZPerspectiveProjection& inProjection);

        EProperty() std::variant<Common::FReversedZOrthoProjection, Common::FReversedZPerspectiveProjection> projection;
    };

    struct RUNTIME_API EClass() SceneCapture final {
        EClassBody(SceneCapture)

        SceneCapture();
        explicit SceneCapture(const Common::FReversedZOrthoProjection& inProjection);
        explicit SceneCapture(const Common::FReversedZPerspectiveProjection& inProjection);

        EProperty() std::variant<Common::FReversedZOrthoProjection, Common::FReversedZPerspectiveProjection> projection;
        // TODO texture render target
    };
}
