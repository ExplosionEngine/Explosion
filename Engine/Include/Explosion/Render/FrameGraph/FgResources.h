//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRESOURCES_H
#define EXPLOSION_FGRESOURCES_H

#include <Explosion/Common/NonCopy.h>
#include <Explosion/Common/Template/RefObject.h>

namespace Explosion {
    class FgResources : public NonCopy, public URefObject<FgResources> {
    public:
        FgResources() {}

        ~FgResources() {}
    };
}

#endif //EXPLOSION_FGRESOURCES_H
