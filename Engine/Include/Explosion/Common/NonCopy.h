//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_NONCOPY_H
#define EXPLOSION_NONCOPY_H

namespace Explosion {
    class NonCopy {
    public:
        NonCopy() = default;

        virtual ~NonCopy() = default;

        NonCopy(const NonCopy &) = delete;

        NonCopy &operator=(const NonCopy &) = delete;
    };
}

#endif //EXPLOSION_NONCOPY_H
