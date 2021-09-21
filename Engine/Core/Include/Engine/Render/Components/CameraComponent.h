//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_CAMERACOMPONENT_H
#define EXPLOSION_CAMERACOMPONENT_H

#include <Math/Math.h>

namespace Explosion {

    enum class ProjectMode : uint8_t {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct CameraComponent {
        float fov;
        float aspectRatio; // width / height
        float near;
        float far;
        ProjectMode mode;
    };

}

#endif // EXPLOSION_CAMERACOMPONENT_H
