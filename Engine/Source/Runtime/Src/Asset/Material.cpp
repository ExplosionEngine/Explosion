//
// Created by johnk on 2025/3/21.
//

#include <Runtime/Asset/Material.h>

namespace Runtime {
    Material::Material(Core::Uri inUri)
        : Asset(std::move(inUri))
        , type(MaterialType::max)
    {
    }

    Material::~Material() = default;

    MaterialInstance::MaterialInstance(Core::Uri inUri)
        : Material(std::move(inUri))
    {
    }

    MaterialInstance::~MaterialInstance() = default;
}
