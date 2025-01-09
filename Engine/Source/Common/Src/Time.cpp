//
// Created by johnk on 2025/1/14.
//

#include <sstream>

#include <Common/Time.h>
#include <Common/Concepts.h>
#include <Common/String.h>

namespace Common::Internal {
    template <Common::CppIntegral T>
    static std::string GetNumStringFillZeros(T inNum, uint8_t inWidth)
    {
        const std::string numStr = std::to_string(inNum);
        std::stringstream stream;
        auto count = inWidth - numStr.length();
        while (count > 0) {
            stream << "0";
            --count;
        }
        stream << numStr;
        return stream.str();
    }
}

namespace Common {
    TimePoint TimePoint::Now()
    {
        return TimePoint { std::chrono::system_clock::now() };
    }

    TimePoint::TimePoint(const std::chrono::system_clock::time_point& inTimePoint)
        : timePoint(inTimePoint)
    {
    }

    Time TimePoint::ToTime() const
    {
        return Time { *this };
    }

    AccurateTime TimePoint::ToAccurateTime() const
    {
        return AccurateTime { *this };
    }

    float TimePoint::ToSeconds() const
    {
        return ToMilliseconds() / 1000.0f;
    }

    float TimePoint::ToMilliseconds() const
    {
        return static_cast<float>(ToMicroseconds()) / 1000.0f;
    }

    uint64_t TimePoint::ToMicroseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(timePoint.time_since_epoch()).count();
    }

    const std::chrono::system_clock::time_point& TimePoint::GetNative() const
    {
        return timePoint;
    }

    Time::Time(const TimePoint& inTimePoint)
    {
        const auto time = std::chrono::system_clock::to_time_t(inTimePoint.GetNative());
        const auto tm = std::localtime(&time); // NOLINT
        year = tm->tm_year + 1900;
        month = tm->tm_mon + 1;
        day = tm->tm_mday;
        hour = tm->tm_hour;
        minute = tm->tm_min;
        second = tm->tm_sec;
    }

    uint16_t Time::Year() const
    {
        return year;
    }

    uint8_t Time::Month() const
    {
        return month;
    }

    uint8_t Time::Day() const
    {
        return day;
    }

    uint8_t Time::Hour() const
    {
        return hour;
    }

    uint8_t Time::Minute() const
    {
        return minute;
    }

    uint8_t Time::Second() const
    {
        return second;
    }

    std::string Time::ToString(const std::string& inFormat) const
    {
        auto temp = StringUtils::Replace(inFormat, "YYYY", Internal::GetNumStringFillZeros(year, 4));
        temp = StringUtils::Replace(temp, "MM", Internal::GetNumStringFillZeros(month, 2));
        temp = StringUtils::Replace(temp, "DD", Internal::GetNumStringFillZeros(day, 2));
        temp = StringUtils::Replace(temp, "hh", Internal::GetNumStringFillZeros(hour, 2));
        temp = StringUtils::Replace(temp, "mm", Internal::GetNumStringFillZeros(minute, 2));
        temp = StringUtils::Replace(temp, "ss", Internal::GetNumStringFillZeros(second, 2));
        return temp;
    }

    AccurateTime::AccurateTime(const TimePoint& inTimePoint)
    {
        const auto time = std::chrono::system_clock::to_time_t(inTimePoint.GetNative());
        const auto* tm = std::localtime(&time); // NOLINT
        year = tm->tm_year + 1900;
        month = tm->tm_mon + 1;
        day = tm->tm_mday;
        hour = tm->tm_hour;
        minute = tm->tm_min;
        second = tm->tm_sec;

        const auto us = inTimePoint.ToMicroseconds() % 1000000;
        millisecond = us / 1000;
        microsecond = us % 1000;
    }

    uint16_t AccurateTime::Year() const
    {
        return year;
    }

    uint8_t AccurateTime::Month() const
    {
        return month;
    }

    uint8_t AccurateTime::Day() const
    {
        return day;
    }

    uint8_t AccurateTime::Hour() const
    {
        return hour;
    }

    uint8_t AccurateTime::Minute() const
    {
        return minute;
    }

    uint8_t AccurateTime::Second() const
    {
        return second;
    }

    uint16_t AccurateTime::Milliseconds() const
    {
        return millisecond;
    }

    uint64_t AccurateTime::Microseconds() const
    {
        return microsecond;
    }

    std::string AccurateTime::ToString(const std::string& inFormat) const
    {
        auto temp = StringUtils::Replace(inFormat, "mss", Internal::GetNumStringFillZeros(millisecond, 3));
        temp = StringUtils::Replace(temp, "uss", Internal::GetNumStringFillZeros(microsecond, 3));
        temp = StringUtils::Replace(temp, "YYYY", Internal::GetNumStringFillZeros(year, 4));
        temp = StringUtils::Replace(temp, "MM", Internal::GetNumStringFillZeros(month, 2));
        temp = StringUtils::Replace(temp, "DD", Internal::GetNumStringFillZeros(day, 2));
        temp = StringUtils::Replace(temp, "hh", Internal::GetNumStringFillZeros(hour, 2));
        temp = StringUtils::Replace(temp, "mm", Internal::GetNumStringFillZeros(minute, 2));
        temp = StringUtils::Replace(temp, "ss", Internal::GetNumStringFillZeros(second, 2));
        return temp;
    }
};
