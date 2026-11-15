#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <Common/Result.h>

namespace Common {
    class Process {
    public:
        static std::optional<int32_t> Run(const std::string& inExecutablePath, const std::vector<std::string>& inArguments);
        static Result<void, std::string> Launch(const std::string& inExecutablePath, const std::vector<std::string>& inArguments);
    };
}
