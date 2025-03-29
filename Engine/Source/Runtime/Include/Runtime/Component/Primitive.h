//
// Created by johnk on 2025/3/24.
//

#pragma once

#include <Runtime/Meta.h>
#include <Runtime/Api.h>
#include <Runtime/Asset/Asset.h>
#include <Runtime/Asset/Mesh.h>

namespace Runtime {
    struct RUNTIME_API EClass() StaticPrimitive final {
        EClassBody(StaticPrimitive)

        StaticPrimitive();

        EProperty() AssetPtr<StaticMesh> mesh;
    };
}
