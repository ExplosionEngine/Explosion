//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

namespace Explosion {
    class Pipeline {
    public:
        struct ShaderConfig {
            std::vector<char> vertexShaderCode;
            std::vector<char> fragmentShaderCode;
        };

        struct VertexConfig {

        };

        struct Config {
            ShaderConfig shaderConfig {};
            VertexConfig vertexConfig {};
        };

        explicit Pipeline(ShaderConfig shaderConfig);
        ~Pipeline();

    private:
        void CreatePipeline();
        void DestroyPipeline();

        ShaderConfig shaderConfig;
    };
}

#endif //EXPLOSION_PIPELINE_H
