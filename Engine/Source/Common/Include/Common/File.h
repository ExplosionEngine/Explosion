//
// Created by johnk on 2022/7/25.
//

#pragma once

#include <string>
#include <fstream>

#include <Common/Debug.h>

namespace Common {
    class FileUtils {
    public:
        static std::string ReadTextFile(const std::string& fileName);
    };
}
