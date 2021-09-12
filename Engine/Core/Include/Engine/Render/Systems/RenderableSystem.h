//
// Created by LiZhen on 2021/9/8.
//

#ifndef EXPLOSION_RENDERSYSTEM_H
#define EXPLOSION_RENDERSYSTEM_H

namespace Explosion {

    struct DrawPackage {
        uint32_t viewTag = 0;
    };

    class IRender {
    public:
        virtual void OnDraw(std::vector<DrawPackage>&) = 0;
    };


}

#endif//EXPLOSION_RENDERSYSTEM_H
