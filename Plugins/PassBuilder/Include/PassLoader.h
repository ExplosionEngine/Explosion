//
// Created by LiZhen on 2021/5/7.
//

#ifndef EXPLOSION_PLUGIN_PASSBLOADER_H
#define EXPLOSION_PLUGIN_PASSBLOADER_H

#include <string>
#include "PassBuilderApi.h"

class PASS_API PassLoader {
public:
    PassLoader() {}
    ~PassLoader() {}

    void Load(const std::string& url);
};

#endif