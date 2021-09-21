//
// Created by Zach Lee on 2021/9/12.
//

#ifndef EXPLOSION_MESHCOMPONENT_H
#define EXPLOSION_MESHCOMPONENT_H

namespace Explosion {

    enum class MeshFlag : uint8_t {
        STATIC,
    };

    struct MeshComponent {
        MeshFlag flag;
    };

}

#endif // EXPLOSION_MESHCOMPONENT_H
