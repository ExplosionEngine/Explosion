//
// Created by johnk on 2025/12/21.
//

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <Common/Platform.h>
#include <Common/Debug.h>

namespace Common {
    void PlatformUtils::SetEnvVar(const std::string& inKey, const std::string& inValue)
    {
#if PLATFORM_WINDOWS
        Assert(SetEnvironmentVariableA(inKey.c_str(), inValue.c_str()));
#else
        Unimplement();
#endif
    }
}
