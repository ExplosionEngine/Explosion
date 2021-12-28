//
// Created by johnk on 2021/12/19.
//

#ifndef EXPLOSION_COMMON_MACROS_H
#define EXPLOSION_COMMON_MACROS_H

#define NON_COPYABLE(clz) \
    clz(clz&) = delete;   \
    clz& operator=(clz&) = delete; \
    clz(const clz&) = delete; \
    clz& operator=(const clz&) = delete; \

#endif //EXPLOSION_COMMON_MACROS_H
