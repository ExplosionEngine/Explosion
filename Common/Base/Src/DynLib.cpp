#include <Common/DynLib.h>

const char TAG[] = "DynLoader";

namespace Explosion {
    void DynLib::Load()
    {
    #ifdef _WIN32
    #elif __APPLE__
        if (name.find("lib") == std::string::npos) {
            name = "lib" + name;
        }

        if (name.substr(name.find_last_of(".") + 1) != "dylib") {
            name += ".dylib";
        }
    #endif

        handle = LIB_LOAD(name.c_str(), RTLD_LOCAL | RTLD_LAZY);
    }

    void DynLib::Unload()
    {
        if (handle) {
            LIB_UNLOAD(handle);
        }
    }

    bool DynLib::Valid()
    {
        return !!handle;
    }

    void* DynLib::GetSymbol(const std::string& func) const
    {
        return (void*)LIB_GET(handle, func.c_str());
    }
}