//
// Created by johnk on 2022/8/4.
//

#include <Runtime/Config.h>

namespace Runtime{
    ConfigManager::ConfigManager(const Common::PathMapper& inPathMapper) : pathMapper(inPathMapper) {}

    ConfigManager::~ConfigManager() = default;
}
