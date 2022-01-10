//
// Created by johnk on 10/1/2022.
//

#include <Common/Logger.h>

#include <utility>

namespace Common {
    Logger::Logger() noexcept = default;

    Logger::~Logger() noexcept = default;

    LoggerDelegator& Logger::FindOrCreateDelegator(const std::string& tag)
    {
        auto iter = delegators.find(tag);
        if (iter != delegators.end()) {
            return *iter->second;
        }
        delegators[tag].reset(new LoggerDelegator(*this, tag));
        return *delegators[tag];
    }

    LoggerDelegator::LoggerDelegator(Logger& l, std::string t) noexcept : logger(l), tag(std::move(t)) {}

    LoggerDelegator::~LoggerDelegator() noexcept = default;
}
