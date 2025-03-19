//
// Created by johnk on 2025/3/21.
//

#include <Runtime/Asset/Mesh.h>

namespace Runtime {
    StaticMesh::StaticMesh(Core::Uri inUri)
        : Asset(std::move(inUri))
    {
    }

    StaticMesh::~StaticMesh() = default;
}
