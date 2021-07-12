//
// Created by LiZhen on 2021/7/10.
//

#ifndef EXPLOSION_SHADERBUILDER_H
#define EXPLOSION_SHADERBUILDER_H

#include <string>

#include <Explosion/RHI/Common/Enum.h>
#include <Shader.h>

namespace Explosion::Builder {

    class ShaderBuilder {
    public:
        ShaderBuilder() {}
        ~ShaderBuilder() {}

        static void Initialize();
        static void Finalize();

        static Shader* Load(const std::string& url, RHI::ShaderStageBits shader);
    };

}

#endif //EXPLOSION_SHADERBUILDER_H
