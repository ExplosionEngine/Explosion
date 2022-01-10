//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_COMMON_LOGGER_H
#define EXPLOSION_COMMON_LOGGER_H

#include <format>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <unordered_map>

#include <Common/Utility.h>

namespace Common {
    class LoggerDelegator;

    class Logger {
    public:
        static Logger& Singleton()
        {
            static Logger singleton;
            return singleton;
        }

        NON_COPYABLE(Logger)
        ~Logger() noexcept;

        template <typename... T>
        inline Logger& Verbose(const std::string& tag, const std::string& format, T&&... values)
        {
            return Print("Verbose", tag, format, std::forward<T...>(values...));
        }

        template <typename... T>
        inline Logger& Debug(const std::string& tag, const std::string& format, T&&... values)
        {
#if BUILD_CONFIG_DEBUG
            return Print("Debug", tag, format, std::forward<T...>(values...));
#else
            return *this;
#endif
        }

        template <typename... T>
        inline Logger& Warning(const std::string& tag, const std::string& format, T&&... values)
        {
            return Print("Warning", tag, format, std::forward<T...>(values...));
        }

        template <typename... T>
        Logger& Info(const std::string& tag, const std::string& format, T&&... values)
        {
            return Print("Info", tag, format, std::forward<T...>(values...));
        }

        template <typename... T>
        inline Logger& Error(const std::string& tag, const std::string& format, T&&... values)
        {
            return Print("Error", tag, format, std::forward<T...>(values...));
        }

        inline Logger& Verbose(const std::string& tag, const std::string& info)
        {
            return Print("Verbose", tag, info);
        }

        inline Logger& Debug(const std::string& tag, const std::string& info)
        {
#if BUILD_CONFIG_DEBUG
            return Print("Debug", tag, info);
#else
            return *this;
#endif
        }

        inline Logger& Warning(const std::string& tag, const std::string& info)
        {
            return Print("Warning", tag, info);
        }

        inline Logger& Info(const std::string& tag, const std::string& info)
        {
            return Print("Info", tag, info);
        }

        inline Logger& Error(const std::string& tag, const std::string& info)
        {
            return Print("Error", tag, info);
        }

        LoggerDelegator& FindOrCreateDelegator(const std::string& tag);

    private:
        template <typename... T>
        Logger& Print(const std::string& type, const std::string& tag, const std::string& format, T&&... values)
        {
            std::cout << std::format("[{}][{}] {}", tag, type, std::format(format, std::forward<T...>(values...))) << std::endl;
            return *this;
        }

        Logger& Print(const std::string& type, const std::string& tag, const std::string& info)
        {
            std::cout << std::format("[{}][{}] {}", tag, type, info) << std::endl;
            return *this;
        }

        Logger() noexcept;

        std::unordered_map<std::string, std::unique_ptr<LoggerDelegator>> delegators;
    };

    class LoggerDelegator {
    public:
        NON_COPYABLE(LoggerDelegator)
        ~LoggerDelegator() noexcept;

        template <typename... T>
        inline LoggerDelegator& Verbose(const std::string& format, T&&... values)
        {
            logger.Verbose(tag, format, std::forward<T...>(values...));
            return *this;
        }

        template <typename... T>
        inline LoggerDelegator& Debug(const std::string& format, T&&... values)
        {
            logger.Debug(tag, format, std::forward<T...>(values...));
            return *this;
        }

        template <typename... T>
        inline LoggerDelegator& Warning(const std::string& format, T&&... values)
        {
            logger.Warning(tag, format, std::forward<T...>(values...));
            return *this;
        }

        template <typename... T>
        inline LoggerDelegator& Info(const std::string& format, T&&... values)
        {
            logger.Info(tag, format, std::forward<T...>(values...));
            return *this;
        }

        template <typename... T>
        inline LoggerDelegator& Error(const std::string& format, T&&... values)
        {
            logger.Error(tag, format, std::forward<T...>(values...));
            return *this;
        }
        
        inline LoggerDelegator& Verbose(const std::string& info)
        {
            logger.Verbose(tag, info);
            return *this;
        }

        inline LoggerDelegator& Debug(const std::string& info)
        {
            logger.Debug(tag, info);
            return *this;
        }

        inline LoggerDelegator& Warning(const std::string& info)
        {
            logger.Warning(tag, info);
            return *this;
        }

        inline LoggerDelegator& Info(const std::string& info)
        {
            logger.Info(tag, info);
            return *this;
        }

        inline LoggerDelegator& Error(const std::string& info)
        {
            logger.Error(tag, info);
            return *this;
        }

    private:
        friend class Logger;

        LoggerDelegator(Logger& logger, std::string tag) noexcept;

        Logger& logger;
        std::string tag;
    };
}

#endif //EXPLOSION_COMMON_LOGGER_H
