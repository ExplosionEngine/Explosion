//
// Created by LiZhen on 2021/9/5.
//

#ifndef EXPLOSION_SCENEVIEW_H
#define EXPLOSION_SCENEVIEW_H

namespace Explosion {

    enum class ViewTag : uint32_t {
        MAIN_CAMERA = 0x01,
        SHADOW_MAP = 0x02
    };

    class SceneView {
    public:
        SceneView() = default;
        ~SceneView() = default;
    };

}

#endif//EXPLOSION_SCENEVIEW_H
