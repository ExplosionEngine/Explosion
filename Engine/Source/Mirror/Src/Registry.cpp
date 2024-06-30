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

    Class& Registry::EmplaceClass(const std::string& inName, Class::ConstructParams&& inParams)
    {
        Assert(!classes.contains(inName));
        classes.emplace(inName, Mirror::Class(std::move(inParams)));
        return classes.at(inName);
    }

    Enum& Registry::EmplaceEnum(const std::string& inName, Enum::ConstructParams&& inParams)
    {
        Assert(!enums.contains(inName));
        enums.emplace(inName, Mirror::Enum(std::move(inParams)));
        return enums.at(inName);
    }
}
