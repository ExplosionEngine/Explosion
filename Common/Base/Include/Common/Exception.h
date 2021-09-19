//
// Created by Zach Lee on 2021/5/9.
//

#ifndef EXPLOSION_EXCEPTION_H
#define EXPLOSION_EXCEPTION_H

#include <exception>

#if EXPLOSION_DEBUG
#include <assert.h>
    #define EXPLOSION_ASSERT(exp, msg) assert((exp)&&(msg))
#else
    #define EXPLOSION_ASSERT(exp, msg)
#endif

#endif //EXPLOSION_EXCEPTION_H
