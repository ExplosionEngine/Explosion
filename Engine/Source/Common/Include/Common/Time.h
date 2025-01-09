//
// Created by johnk on 2025/1/14.
//

#pragma once

#include <chrono>

namespace Common {
    class Time;
    class AccurateTime;

    class TimePoint {
    public:
        static TimePoint Now();

        explicit TimePoint(const std::chrono::system_clock::time_point& inTimePoint);

        Time ToTime() const;
        AccurateTime ToAccurateTime() const;
        float ToSeconds() const;
        float ToMilliseconds() const;
        uint64_t ToMicroseconds() const;
        const std::chrono::system_clock::time_point& GetNative() const;

    private:
        std::chrono::system_clock::time_point timePoint;
    };

    class Time {
    public:
        explicit Time(const TimePoint& inTimePoint);

        uint16_t Year() const;
        uint8_t Month() const;
        uint8_t Day() const;
        uint8_t Hour() const;
        uint8_t Minute() const;
        uint8_t Second() const;
        std::string ToString(const std::string& inFormat = "YYYY-MM-DD-hh-mm-ss") const;

    private:
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    };

    class AccurateTime {
    public:
        explicit AccurateTime(const TimePoint& inTimePoint);

        uint16_t Year() const;
        uint8_t Month() const;
        uint8_t Day() const;
        uint8_t Hour() const;
        uint8_t Minute() const;
        uint8_t Second() const;
        uint16_t Milliseconds() const;
        uint64_t Microseconds() const;
        std::string ToString(const std::string& inFormat = "YYYY-MM-DD-hh-mm-ss:mss:uss") const;

    private:
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint16_t millisecond;
        uint16_t microsecond;
    };
}
