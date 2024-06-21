//
// Created by johnk on 2024/4/14.
//

#include <Common/Hash.h>

namespace Common {
    uint64_t HashUtils::CityHash(const void* buffer, const size_t length)
    {
        return CityHash64(static_cast<const char*>(buffer), length);
    }
}
