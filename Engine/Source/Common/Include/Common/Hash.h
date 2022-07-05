//
// Created by johnk on 2022/6/28.
//

#pragma once

#include <cstdint>

#include <city.h>

namespace Common {
    class HashUtils {
    public:
        static uint64_t CityHash(const void* buffer, size_t length)
        {
            return CityHash64(static_cast<const char*>(buffer), length);
        }
    };
}
