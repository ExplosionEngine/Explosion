//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_GRAPHICSPIPELINE_H
#define EXPLOSION_GRAPHICSPIPELINE_H

#include <vector>

#include <RHI/Enum.h>

namespace Explosion::RHI {
    class RenderPass;

    class GraphicsPipeline {
    public:
        struct ShaderModule {
            ShaderStageBits stage;
            std::vector<char> code;
        };

        struct ShaderConfig {
            std::vector<ShaderModule> shaderModules;
        };

        struct VertexBinding {
            uint32_t binding;
            uint32_t stride;
            VertexInputRate inputRate;
        };

        struct VertexAttribute {
            uint32_t binding;
            uint32_t location;
            Format format;
            uint32_t offset;
        };

        struct VertexConfig {
            std::vector<VertexBinding> vertexBindings;
            std::vector<VertexAttribute> vertexAttributes;
        };

        struct DescriptorAttribute {
            uint32_t binding;
            DescriptorType type;
            ShaderStageFlags shaderStages;
        };

        struct DescriptorConfig {
            std::vector<DescriptorAttribute> descriptorAttributes;
        };

        struct Viewport {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        };

        struct Scissor {
            int32_t x;
            int32_t y;
            uint32_t width;
            uint32_t height;
        };

        struct ViewportScissorConfig {
            Viewport viewport;
            Scissor scissor;
        };

        struct RasterizerConfig {
            bool depthClamp;
            bool discard;
            CullModeFlags cullModes;
            FrontFace frontFace;
        };

        struct DepthStencilConfig {
            bool depthTest;
            bool depthWrite;
            bool stencilTest;
        };

        struct ColorBlendConfig {
            bool enabled;
        };

        struct AssemblyConfig {
            PrimitiveTopology topology;
        };

        struct Config {
            RenderPass* renderPass;
            ShaderConfig shaderConfig;
            VertexConfig vertexConfig;
            DescriptorConfig descriptorConfig;
            ViewportScissorConfig viewportScissorConfig;
            RasterizerConfig rasterizerConfig;
            DepthStencilConfig depthStencilConfig;
            ColorBlendConfig colorBlendConfig;
            AssemblyConfig assemblyConfig;
        };

        virtual ~GraphicsPipeline();

    protected:
        explicit GraphicsPipeline(Config config);

        Config config;
    };
}

#endif //EXPLOSION_GRAPHICSPIPELINE_H
