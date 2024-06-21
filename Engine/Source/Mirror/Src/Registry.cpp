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
}
