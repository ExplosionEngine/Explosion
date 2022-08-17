//
// Created by johnk on 2022/8/4.
//

#pragma once

#include <Common/Path.h>

namespace Runtime{
    // TODO rewrite config manager interfaces with reflection
    class ConfigManager {
    public:
        explicit ConfigManager(const Common::PathMapper& inPathMapper);
        ~ConfigManager();

    private:
        const Common::PathMapper& pathMapper;
    };
}
