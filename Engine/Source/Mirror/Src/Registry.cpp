//
// Created by johnk on 2023/10/31.
//

#include <Mirror/Registry.h>

namespace Mirror {
    GlobalRegistry::GlobalRegistry(GlobalScope& inGlobalScope)
        : MetaDataRegistry(&inGlobalScope)
        , globalScope(inGlobalScope)
    {
    }

    GlobalRegistry::~GlobalRegistry() = default;

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
        Assert(!classes.contains(inId));
        classes.emplace(inId, Mirror::Class(std::move(inParams)));
        return classes.at(inId);
    }

    Enum& Registry::EmplaceEnum(const Id& inId, Enum::ConstructParams&& inParams)
    {
        Assert(!enums.contains(inId));
        enums.emplace(inId, Mirror::Enum(std::move(inParams)));
        return enums.at(inId);
    }
}
