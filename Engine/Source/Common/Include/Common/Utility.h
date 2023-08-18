//
// Created by johnk on 2021/12/19.
//

#pragma once

#define NonCopyable(clz) \
    clz(clz&) = delete;   \
    clz& operator=(clz&) = delete; \
    clz(const clz&) = delete; \
    clz& operator=(const clz&) = delete; \
