//
// Created by johnk on 2022/8/4.
//

#pragma once

#include <Common/Path.h>

namespace Runtime{
    class ConfigManager {
    public:
        ~ConfigManager();

    private:
        friend class Engine;

        explicit ConfigManager(const Common::PathMapper& inPathMapper);

        const Common::PathMapper& pathMapper;
    };
}
