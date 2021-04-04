//
// Created by Administrator on 2021/4/4 0004.
//

#ifndef EXPLOSION_LOGGER_H
#define EXPLOSION_LOGGER_H

#include <string>

namespace Explosion {
    class Logger {
    public:
        ~Logger();

        static void Debug(const std::string& log);
        static void Warning(const std::string& log);
        static void Info(const std::string& log);
        static void Error(const std::string& log);

    private:
        Logger();

        static void Log(const std::string& tag, const std::string& log);
    };
}

#endif //EXPLOSION_LOGGER_H
