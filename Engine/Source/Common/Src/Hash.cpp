//
// Created by johnk on 2024/4/14.
//

#include <Common/Hash.h>

namespace Common {
    uint32_t HashUtils::StrCrc32(const char* str, size_t length)
    {
        uint32_t result = 0xffffffff;
        for (auto i = 0; i < length; i++) {
            result = (result >> 8) ^ Internal::crcTable[(result ^ str[i]) & 0x000000ff];
        }
        return result ^ 0xffffffff;
    }

    uint32_t HashUtils::StrCrc32(const std::string& str)
    {
        return StrCrc32(str.c_str(), str.length());
    }

    uint64_t HashUtils::CityHash(const void* buffer, const size_t length)
    {
        return CityHash64(static_cast<const char*>(buffer), length);
    }
}
