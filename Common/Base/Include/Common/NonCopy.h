//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_NONCOPY_H
#define EXPLOSION_NONCOPY_H

#define EXPLOSION_NON_COPY(className)     \
    className(const className&) = delete; \
    className& operator=(const className&) = delete;

#endif //EXPLOSION_NONCOPY_H
