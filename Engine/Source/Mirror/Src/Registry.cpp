//
// Created by johnk on 2023/10/31.
//

#include <utility>

#include <Mirror/Registry.h>

namespace Mirror::Internal {
    ScopedReleaser::ScopedReleaser(ReleaseFunc inReleaseFunc)
        : releaseFunc(std::move(inReleaseFunc))
    {
    }

    ScopedReleaser::~ScopedReleaser()
    {
        if (releaseFunc) {
            releaseFunc();
        }
    }
} // namespace Mirror::Internal

namespace Mirror {
    GlobalRegistry::GlobalRegistry(GlobalScope& inGlobalScope)
        : MetaDataRegistry(&inGlobalScope)
        , globalScope(inGlobalScope)
    {
    }

    GlobalRegistry::~GlobalRegistry() = default;

    void GlobalRegistry::UnloadVariable(const Id& inId) // NOLINT
    {
        globalScope.variables.Erase(inId);
    }

    void GlobalRegistry::UnloadFunction(const Id& inId) // NOLINT
    {
        globalScope.functions.Erase(inId);
    }

    Registry& Registry::Get()
    {
        static Registry instance;
        return instance;
    }

    Registry::Registry() noexcept = default;

    Registry::~Registry() = default;

    GlobalRegistry Registry::Global()
    {
        return GlobalRegistry(globalScope);
    }

    Class& Registry::EmplaceClass(const Id& inId, Class::ConstructParams&& inParams)
    {
        classes.Emplace(inId, Mirror::Class(std::move(inParams)));
        return classes.At(inId);
    }

    Enum& Registry::EmplaceEnum(const Id& inId, Enum::ConstructParams&& inParams)
    {
        enums.Emplace(inId, Mirror::Enum(std::move(inParams)));
        return enums.At(inId);
    }

    void Registry::UnloadClass(const Id& inId) // NOLINT
    {
        classes.Erase(inId);
    }

    void Registry::UnloadEnum(const Id& inId) // NOLINT
    {
        enums.Erase(inId);
    }
}
