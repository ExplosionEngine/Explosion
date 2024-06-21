//
// Created by johnk on 2024/6/6.
//

#pragma once

#include <iostream>

namespace Common {
    constexpr char newline = '\n';

    template <uint8_t N>
    struct Tab {
        template <typename S>
        friend S& operator<<(S& stream, const Tab& tab)
        {
            for (auto i = 0; i < N * 4; i++) {
                stream << " ";
            }
            return stream;
        }
    };

    template <uint8_t N>
    const Tab<N> tab = Tab<N>();

    class ScopedCoutFlusher {
    public:
        ScopedCoutFlusher();
        ~ScopedCoutFlusher();
    };

    class ScopedCerrFlusher {
    public:
        ScopedCerrFlusher();
        ~ScopedCerrFlusher();
    };
}

#define AutoCoutFlush Common::ScopedCoutFlusher _scopedCoutFlusher;
#define AutoCerrFlush Common::ScopedCerrFlusher _scopedCerrFlusher;
