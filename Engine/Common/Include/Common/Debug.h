//
// Created by johnk on 9/4/2022.
//

#pragma once

#if BUILD_CONFIG_DEBUG
#include <cassert>
#include <debugbreak.h>
#endif

namespace Common {
    class Debug {
    public:
        static void Assert(bool expression)
        {
#if BUILD_CONFIG_DEBUG
            /* TODO platform output */
            debug_break();
#endif
        }

        ~Debug() = default;

    private:

        Debug() = default;
    };
}
