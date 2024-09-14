//
// Created by johnk on 28/12/2021.
//

#include <Common/DynamicLibrary.h>
#include <Common/Debug.h>

#if PLATFORM_WINDOWS
#define DynamicLibLoad(lib, tag)       LoadLibraryEx(lib, nullptr, 0)
#define DynamicLibGetSymbol(lib, sym)  GetProcAddress(lib, sym)
#define DynamicLibUnload(lib)          FreeLibrary(lib)
#else
#include <dlfcn.h>
#define DynamicLibLoad(lib, tag)       dlopen(lib, tag)
#define DynamicLibGetSymbol(lib, sym)  dlsym(lib, sym)
#define DynamicLibUnload(lib)          dlclose(lib)
#endif

namespace Common {
    DynamicLibrary::DynamicLibrary()
        : loaded(false)
        , handle(nullptr)
    {
    }

    DynamicLibrary::DynamicLibrary(std::string inFullPath)
        : loaded(false)
        , fullPath(std::move(inFullPath))
        , handle(nullptr)
    {
    }

    DynamicLibrary::~DynamicLibrary()
    {
        if (loaded) {
            Unload();
        }
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary&& inOther) noexcept
        : loaded(inOther.loaded)
        , fullPath(std::move(inOther.fullPath))
        , handle(inOther.handle)
    {
        inOther.loaded = false;
    }

    DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& inOther) noexcept
    {
        loaded = inOther.loaded;
        fullPath = std::move(inOther.fullPath);
        handle = inOther.handle;
        inOther.loaded = false;
        return *this;
    }

    bool DynamicLibrary::IsValid() const
    {
        return !fullPath.empty();
    }

    bool DynamicLibrary::IsLoaded() const
    {
        return loaded;
    }

    void DynamicLibrary::Load()
    {
        Assert(!loaded);
        handle = DynamicLibLoad(fullPath.c_str(), RTLD_LOCAL | RTLD_LAZY);
        Assert(handle != nullptr);
        loaded = true;
    }

    void DynamicLibrary::Unload()
    {
        Assert(loaded);
        DynamicLibUnload(handle);
        loaded = false;
    }

    void* DynamicLibrary::GetSymbol(const std::string& name) const
    {
        Assert(loaded);
        return static_cast<void*>(DynamicLibGetSymbol(handle, name.c_str()));
    }

    DynamicLibHandle DynamicLibrary::GetHandle() const
    {
        return handle;
    }

    DynamicLibrary DynamicLibraryFinder::Find(const std::string& simpleName, const std::string& searchDirectory)
    {
        return DynamicLibrary(GetPlatformDynLibFullPath(simpleName, searchDirectory));
    }

    std::string DynamicLibraryFinder::GetPlatformDynLibFullPath(const std::string& simpleName, const std::string& searchDirectory)
    {
        std::string result = simpleName;
        RepairLibPrefix(result);
        RepairExtension(result);

        if (searchDirectory.empty()) {
            return result;
        }
        return searchDirectory + "/" + result;
    }

    void DynamicLibraryFinder::RepairLibPrefix(std::string& name)
    {
#if !PLATFORM_WINDOWS
        if (name.find("lib") != std::string::npos) {
            return;
        }
        name = "lib" + name;
#endif
    }

    void DynamicLibraryFinder::RepairExtension(std::string& name)
    {
#if PLATFORM_WINDOWS
        static const std::string ext = "dll";
#elif PLATFORM_MACOS
        static const std::string ext = "dylib";
#else
        static const std::string ext = "so";
#endif
        if (name.substr(name.find_last_of('.') + 1) == ext) {
            return;
        }
        name.append(".").append(ext);
    }
}
