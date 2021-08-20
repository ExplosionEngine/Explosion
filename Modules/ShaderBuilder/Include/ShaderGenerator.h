//
// Created by LiZhen on 2021/7/13.
//

#ifndef EXPLOSION_SHADERGENERATOR_H
#define EXPLOSION_SHADERGENERATOR_H

#include <string>

namespace Explosion {

    class ShaderGenerator {
    public:
        ShaderGenerator() = default;
        ~ShaderGenerator() = default;

        static std::string Generate(const std::string& url);

        static void SetBasic(const std::string& dir);
    };
}

#endif // EXPLOSION_SHADERGENERATOR_H
