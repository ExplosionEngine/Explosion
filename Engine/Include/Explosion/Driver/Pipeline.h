//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enums.h>

namespace Explosion {
    class Device;

    class Pipeline {
    public:
        struct ShaderConfig {
            std::vector<char> vertexShaderCode {};
            std::vector<char> fragmentShaderCode {};
        };

        struct VertexConfig {

        };

        struct ViewportScissorsConfig {
            float viewPortX = 0.f;
            float viewPortY = 0.f;
            float viewPortWidth = 0.f;
            float viewPortHeight = 0.f;
            float viewPortMinDepth = 0.f;
            float viewPortMaxDepth = 1.f;
            int32_t scissorX = 0;
            int32_t scissorY = 0;
            int32_t scissorWidth = 0;
            int32_t scissorHeight = 0;
        };

        struct RasterizationConfig {
            bool depthClamp = false;
            bool discard = false;
            PolygonMode polygonMode = PolygonMode::FILL;
            CullModeFlags cullMode = GetFlags(CullMode::BACK);
            FrontFace frontFace = FrontFace::CLOCKWISE;
        };

        struct Config {
            ShaderConfig shaderConfig;
            VertexConfig vertexConfig;
            ViewportScissorsConfig viewportScissorsConfig;
            RasterizationConfig rasterizationConfig;
            PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
        };

        explicit Pipeline(Device& device, Config config);
        ~Pipeline();

    private:
        void CreatePipeline();
        void DestroyPipeline();

        Device& device;
        Config config;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_PIPELINE_H
