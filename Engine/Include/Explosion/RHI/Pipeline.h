//
// Created by John Kindem on 2021/4/26.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Enum.h>
#include <Explosion/RHI/GpuRes.h>

namespace Explosion {
    class Driver;
    class Device;
    class RenderPass;

    class Pipeline : public GpuRes {
    public:
        struct ShaderModule {
            ShaderStage stage;
            std::vector<char> code;
        };

        explicit Pipeline(Driver& driver, RenderPass* renderPass);
        ~Pipeline() override;
        const VkPipelineLayout& GetVkPipelineLayout() const;
        const VkPipeline& GetVkPipeline() const;
        const VkDescriptorSetLayout& GetVkDescriptorSetLayout() const;

    protected:
        void OnCreate() override;
        void OnDestroy() override;
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void DestroyShaderModule(const VkShaderModule& shaderModule);

        Device& device;
        RenderPass* renderPass;
        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        VkPipeline vkPipeline = VK_NULL_HANDLE;
    };

    class GraphicsPipeline : public Pipeline {
    public:
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
            ShaderConfig shaderConfig;
            VertexConfig vertexConfig;
            DescriptorConfig descriptorConfig;
            ViewportScissorConfig viewportScissorConfig;
            RasterizerConfig rasterizerConfig;
            DepthStencilConfig depthStencilConfig;
            ColorBlendConfig colorBlendConfig;
        };

        GraphicsPipeline(Driver& driver, RenderPass* renderPass, const Config& config);
        ~GraphicsPipeline() override;

    protected:
        void OnCreate() override;
        void OnDestroy() override;

    private:
        void CreateDescriptorPool();
        void DestroyDescriptorPool();

        void CreateDescriptorSetLayout();
        void DestroyDescriptorSetLayout();

        void CreatePipelineLayout();
        void DestroyPipelineLayout();

        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        Config config {};
    };
}

#endif //EXPLOSION_PIPELINE_H
