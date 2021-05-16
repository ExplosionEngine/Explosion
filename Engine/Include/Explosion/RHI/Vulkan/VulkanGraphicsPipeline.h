//
// Created by John Kindem on 2021/4/26.
//

#ifndef EXPLOSION_VULKANGRAPHICSPIPELINE_H
#define EXPLOSION_VULKANGRAPHICSPIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;
    class VulkanRenderPass;

    class VulkanGraphicsPipeline {
    public:
        struct ShaderModule {
            ShaderStage stage;
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
            std::vector<ShaderStage> shaderStages;
        };

        struct DescriptorPoolSize {
            DescriptorType type;
            uint32_t count;
        };

        struct DescriptorConfig {
            std::vector<DescriptorAttribute> descriptorAttributes;
            std::vector<DescriptorPoolSize> descriptorPoolSizes;
            uint32_t maxSets;
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
            CullMode cullMode;
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

        struct Config {
            VulkanRenderPass* renderPass;
            ShaderConfig shaderConfig;
            VertexConfig vertexConfig;
            DescriptorConfig descriptorConfig;
            ViewportScissorConfig viewportScissorConfig;
            RasterizerConfig rasterizerConfig;
            DepthStencilConfig depthStencilConfig;
            ColorBlendConfig colorBlendConfig;
        };

        explicit VulkanGraphicsPipeline(VulkanDriver& driver, Config config);
        ~VulkanGraphicsPipeline();
        const VkPipelineLayout& GetVkPipelineLayout();
        const VkPipeline& GetVkPipeline();
        const VkDescriptorSetLayout& GetVkDescriptorSetLayout();

    private:
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void DestroyShaderModule(const VkShaderModule& shaderModule);

        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSetLayout();
        void DestroyDescriptorSetLayout();

        void CreatePipelineLayout();
        void DestroyPipelineLayout();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config {};
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANGRAPHICSPIPELINE_H
