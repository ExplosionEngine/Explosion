//
// Created by johnk on 2022/5/2.
//

#ifndef EXPLOSION_COMMON_STRING_H
#define EXPLOSION_COMMON_STRING_H

#include <string>
#include <locale>
#include <codecvt>

namespace Common {
    class StringUtils {
    public:
        static inline std::wstring ToWideString(const std::string& src)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            return converter.from_bytes(src);
        }
    };
}

#endif//EXPLOSION_COMMON_STRING_H
