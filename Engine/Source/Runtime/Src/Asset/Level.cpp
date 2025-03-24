//
// Created by johnk on 2025/2/19.
//

#include <Runtime/Asset/Level.h>

namespace Runtime {
    Level::Level(Core::Uri inUri)
        : Asset(std::move(inUri))
    {
    }

    Level::~Level() = default;

    ECArchive& Level::GetArchive()
    {
        return archive;
    }
}
