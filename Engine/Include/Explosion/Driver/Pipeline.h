//
// Created by John Kindem on 2021/4/26.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/Driver/Enum.h>

namespace Explosion {
    class Driver;
    class Device;
    class RenderPass;

    class Pipeline {
    public:
        struct ShaderModule {
            ShaderStage stage;
            std::vector<char> code;
        };

        ~Pipeline();

    protected:
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void DestroyShaderModule(const VkShaderModule& shaderModule);

        explicit Pipeline(Driver& driver, RenderPass* renderPass);

        Driver& driver;
        Device& device;
        RenderPass* renderPass;
    };

    class GraphicsPipeline : public Pipeline {
    public:
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
            int32_t width;
            int32_t height;
        };

        struct RasterizerConfig {
            bool depthClamp;
            bool discard;
            CullMode cullMode;
            FrontFace frontFace;
        };

        struct DepthStencilConfig {
            bool depthTest;
            bool depthWrite;
            bool stencilTest;
        };

        struct DescriptorAttribute {
            uint32_t binding;
            DescriptorType type;
            std::vector<ShaderStage> shaderStages;
        };

        struct Config {
            std::vector<ShaderModule> shaderModules;
            std::vector<VertexBinding> vertexBindings;
            std::vector<VertexAttribute> vertexAttributes;
            std::vector<DescriptorAttribute> descriptorAttributes;
            Viewport viewport;
            Scissor scissor;
            RasterizerConfig rasterizerConfig;
            DepthStencilConfig depthStencilConfig;
            bool colorBlend;
        };

        GraphicsPipeline(Driver& driver, RenderPass* renderPass, const Config& config);
        ~GraphicsPipeline();

    private:
        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSetLayout();
        void DestroyDescriptorSetLayout();

        void AllocateDescriptorSet();

        void CreatePipelineLayout();
        void DestroyPipelineLayout();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        Config config {};
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_PIPELINE_H
