//
// Created by johnk on 2024/10/14.
//

#include <Runtime/Component/View.h>

namespace Runtime {
    Camera::Camera()
        : projection(Common::FReversedZPerspectiveProjection())
    {
    }

    Camera::Camera(const Common::FReversedZOrthoProjection& inProjection)
        : projection(inProjection)
    {
    }

    Camera::Camera(const Common::FReversedZPerspectiveProjection& inProjection)
        : projection(inProjection)
    {
    }

    SceneCapture::SceneCapture()
        : projection(Common::FReversedZPerspectiveProjection())
    {
    }

    SceneCapture::SceneCapture(const Common::FReversedZOrthoProjection& inProjection)
        : projection(inProjection)
    {
    }

    SceneCapture::SceneCapture(const Common::FReversedZPerspectiveProjection& inProjection)
        : projection(inProjection)
    {
    }
}
