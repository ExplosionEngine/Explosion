#include <Common/Process.h>

#include <format>
#include <system_error>

#if PLATFORM_WINDOWS
#include <windows.h>

#include <Common/String.h>
#else
#include <cerrno>
#include <spawn.h>
#include <sys/wait.h>
#include <thread>

#if PLATFORM_MACOS
#include <crt_externs.h>
#else
extern char** environ;
#endif
#endif

namespace Common::Internal {
#if PLATFORM_WINDOWS
    static void AppendWindowsCommandLineArgument(std::wstring& outCommandLine, const std::wstring& inArgument)
    {
        if (!outCommandLine.empty()) {
            outCommandLine.push_back(L' ');
        }

        outCommandLine.push_back(L'"');
        size_t backslashCount = 0;
        for (const wchar_t character : inArgument) {
            if (character == L'\\') {
                ++backslashCount;
                continue;
            }
            if (character == L'"') {
                outCommandLine.append(backslashCount * 2 + 1, L'\\');
            } else {
                outCommandLine.append(backslashCount, L'\\');
            }
            backslashCount = 0;
            outCommandLine.push_back(character);
        }
        outCommandLine.append(backslashCount * 2, L'\\');
        outCommandLine.push_back(L'"');
    }

    static Result<PROCESS_INFORMATION, std::string> CreateChildProcess(const std::string& inExecutablePath, const std::vector<std::string>& inArguments, bool inDetached)
    {
        const std::wstring executablePath = StringUtils::ToWideString(inExecutablePath);
        std::wstring commandLine;
        AppendWindowsCommandLineArgument(commandLine, executablePath);
        for (const auto& argument : inArguments) {
            AppendWindowsCommandLineArgument(commandLine, StringUtils::ToWideString(argument));
        }

        STARTUPINFOW startupInfo {};
        startupInfo.cb = sizeof(startupInfo);
        PROCESS_INFORMATION processInfo {};
        const DWORD flags = inDetached ? DETACHED_PROCESS | CREATE_NEW_PROCESS_GROUP : 0;
        if (!CreateProcessW(executablePath.c_str(), commandLine.data(), nullptr, nullptr, FALSE, flags, nullptr, nullptr, &startupInfo, &processInfo)) {
            return Err(std::format("Could not start '{}': {}", inExecutablePath, std::error_code(static_cast<int>(GetLastError()), std::system_category()).message()));
        }
        return Ok(processInfo);
    }
#else
    static char** GetEnvironment()
    {
#if PLATFORM_MACOS
        return *_NSGetEnviron();
#else
        return environ;
#endif
    }

    static std::optional<int32_t> WaitForProcess(const pid_t processId)
    {
        int status = 0;
        while (waitpid(processId, &status, 0) == -1) {
            if (errno != EINTR) {
                return std::nullopt;
            }
        }
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        if (WIFSIGNALED(status)) {
            return 128 + WTERMSIG(status);
        }
        return std::nullopt;
    }

    static Result<pid_t, std::string> CreateChildProcess(const std::string& inExecutablePath, const std::vector<std::string>& inArguments)
    {
        std::vector<char*> arguments;
        arguments.reserve(inArguments.size() + 2);
        arguments.push_back(const_cast<char*>(inExecutablePath.c_str()));
        for (const auto& argument : inArguments) {
            arguments.push_back(const_cast<char*>(argument.c_str()));
        }
        arguments.push_back(nullptr);

        pid_t processId = 0;
        const int error = posix_spawn(&processId, inExecutablePath.c_str(), nullptr, nullptr, arguments.data(), GetEnvironment());
        if (error != 0) {
            return Err(std::format("Could not start '{}': {}", inExecutablePath, std::error_code(error, std::generic_category()).message()));
        }
        return Ok(processId);
    }
#endif
}

namespace Common {
    std::optional<int32_t> Process::Run(const std::string& inExecutablePath, const std::vector<std::string>& inArguments)
    {
#if PLATFORM_WINDOWS
        const auto result = Internal::CreateChildProcess(inExecutablePath, inArguments, false);
        if (result.IsErr()) {
            return std::nullopt;
        }
        const auto& processInfo = result.Value();
        const DWORD waitResult = WaitForSingleObject(processInfo.hProcess, INFINITE);
        DWORD exitCode = 0;
        const bool hasExitCode = waitResult == WAIT_OBJECT_0 && GetExitCodeProcess(processInfo.hProcess, &exitCode);
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        return hasExitCode ? std::optional<int32_t>(static_cast<int32_t>(exitCode)) : std::nullopt;
#else
        const auto result = Internal::CreateChildProcess(inExecutablePath, inArguments);
        if (result.IsErr()) {
            return std::nullopt;
        }
        return Internal::WaitForProcess(result.Value());
#endif
    }

    Result<void, std::string> Process::Launch(const std::string& inExecutablePath, const std::vector<std::string>& inArguments)
    {
#if PLATFORM_WINDOWS
        const auto result = Internal::CreateChildProcess(inExecutablePath, inArguments, true);
        if (result.IsErr()) {
            return Err(result.Error());
        }
        CloseHandle(result.Value().hThread);
        CloseHandle(result.Value().hProcess);
#else
        const auto result = Internal::CreateChildProcess(inExecutablePath, inArguments);
        if (result.IsErr()) {
            return Err(result.Error());
        }
        // Keep a waiter while the parent lives so completed children do not become zombies. The child remains
        // independent of the launcher's lifetime and is reparented normally if the launcher exits first.
        std::thread([processId = result.Value()]() -> void {
            Internal::WaitForProcess(processId);
        }).detach();
#endif
        return Ok();
    }
}
