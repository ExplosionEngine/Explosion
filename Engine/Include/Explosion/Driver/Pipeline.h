//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Device;

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

        explicit Pipeline(Device& device, Config config);
        ~Pipeline();

    private:
        void CreatePipeline();
        void DestroyPipeline();

        Device& device;
        Config config;
    };
}

#endif //EXPLOSION_PIPELINE_H
