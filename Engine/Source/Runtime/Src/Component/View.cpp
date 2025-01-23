//
// Created by johnk on 2024/10/14.
//

#include <Runtime/Component/View.h>

namespace Runtime {
    Camera::Camera()
        : perspective(true)
        , nearPlane(0.1f)
        , fov(90.0f)
    {
    }
}
