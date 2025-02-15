//
// Created by johnk on 2025/1/13.
//

#include <Core/Log.h>
#include <Common/FileSystem.h>
#include <Common/IO.h>

namespace Core {
    COutLogStream::COutLogStream() = default;

    COutLogStream::~COutLogStream()
    {
        Flush();
    }

    void COutLogStream::Write(const std::string& inString)
    {
        std::cout << inString << Common::newline;
    }

    void COutLogStream::Flush()
    {
        std::cout << std::flush;
    }

    FileLogStream::FileLogStream(const std::string& inFilePath)
    {
        if (const auto parentPath = Common::Path(inFilePath).Parent();
            !parentPath.Exists()) {
            parentPath.MakeDir();
        }
        file = std::ofstream(inFilePath);
    }

    FileLogStream::~FileLogStream()
    {
        Flush();
    }

    void FileLogStream::Write(const std::string& inString)
    {
        file << inString << Common::newline;
    }

    void FileLogStream::Flush()
    {
        file << std::flush;
    }

    Logger& Logger::Get()
    {
        static Logger logger;
        return logger;
    }

    Logger::~Logger()
    {
        Flush();
    }

    void Logger::Log(const std::string& inTag, const std::string& inLevel, const std::string& inContent)
    {
        const auto time = Common::AccurateTime(Common::TimePoint::Now());
        LogInternal(std::format("[{}][{}][{}] {}", time.ToString(), inTag, inLevel, inContent));
    }

    void Logger::Attach(Common::UniquePtr<LogStream>&& inStream)
    {
        streams.emplace_back(std::move(inStream));
    }

    void Logger::Flush() // NOLINT
    {
        for (const auto& stream : streams) {
            stream->Flush();
        }
    }

    Logger::Logger()
        : lastFlushTimeSec(Common::TimePoint::Now().ToSeconds())
    {
        Attach(new COutLogStream());
    }

    void Logger::LogInternal(const std::string& inString) // NOLINT
    {
#if BUILD_CONFIG_DEBUG
        const bool needFlush = true; // NOLINT
#else
        const auto timeNowSec = Common::TimePoint::Now().ToSeconds();
        const bool needFlush = timeNowSec - lastFlushTimeSec > 5.0f;
        lastFlushTimeSec = timeNowSec;
#endif

        for (const auto& stream : streams) {
            stream->Write(inString);
            if (needFlush) {
                stream->Flush();
            }
        }
    }
}
