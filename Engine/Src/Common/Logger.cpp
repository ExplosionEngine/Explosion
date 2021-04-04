//
// Created by Administrator on 2021/4/4 0004.
//

#include <iostream>

#include <Explosion/Common/Logger.h>

namespace Explosion {
    Logger::~Logger() = default;

    Logger::Logger() = default;

    void Logger::Debug(const std::string& log)
    {
        Log("debug", log);
    }

    void Logger::Warning(const std::string& log)
    {
        Log("warning", log);
    }

    void Logger::Info(const std::string& log)
    {
        Log("info", log);
    }

    void Logger::Error(const std::string& log)
    {
        Log("error", log);
    }

    void Logger::Log(const std::string& tag, const std::string& log)
    {
        std::cout << "[" << tag << "] " << log << std::endl;
    }
}
