//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRESOURCE_H
#define EXPLOSION_FGRESOURCE_H

#include <Explosion/Common/NonCopy.h>
#include <Explosion/Common/Template/RefObject.h>

class FgResource : public NonCopy, public URefObject<FgResource> {
public:
    FgResource() {}
    ~FgResource() {}
};

#endif //EXPLOSION_FGRESOURCE_H
