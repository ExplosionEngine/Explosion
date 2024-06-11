//
// Created by johnk on 2024/6/6.
//

#include <Common/IO.h>

namespace Common {
    ScopedCoutFlusher::ScopedCoutFlusher() = default;

    ScopedCoutFlusher::~ScopedCoutFlusher()
    {
        std::cout << std::flush;
    }

    ScopedCerrFlusher::ScopedCerrFlusher() = default;

    ScopedCerrFlusher::~ScopedCerrFlusher()
    {
        std::cerr << std::flush;
    }
}
