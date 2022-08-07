//
// Created by johnk on 2022/8/4.
//

#include <Engine/Config.h>

namespace Engine {
    ConfigManager::ConfigManager(const Common::PathMapper& inPathMapper) : pathMapper(inPathMapper) {}

    ConfigManager::~ConfigManager() = default;
}
