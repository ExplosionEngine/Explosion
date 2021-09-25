//
// Created by Zach Lee on 2021/9/25.
//

#ifndef EXPLOSION_SHADER_H
#define EXPLOSION_SHADER_H

#include <vector>
#include <RHI/Enum.h>

namespace Explosion::RHI {

    class Shader {
    public:
        struct Config {
            ShaderStageBits stage;
            std::vector<char> data;
        };

        virtual ~Shader();

    protected:
        explicit Shader(const Config& config);

        Config config;
    };
}

#endif//EXPLOSION_SHADER_H
