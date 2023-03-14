//
// Created by 兰俊康 on 2023/3/8.
//

#pragma once

#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <random>

#include <clipp.h>
#include <GLFW/glfw3.h>
#if PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_MACOS
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

#include <Common/Utility.h>
#include <Common/Debug.h>
#include <Common/String.h>
#include <Common/File.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>

#include <Application.h>
#include <GLTFParser.h>
#include <Camera.h>

namespace Example {
    class Renderer;
    class Renderable {
    public:
        Renderable() {};
        ~Renderable();

        void InitalizeWithPrimitive(Renderer* renderer, Primitive* primitive);

        uint32_t indexCount;
        uint32_t firstIndex;

        RHI::BindGroup* bindGroup = nullptr;
        RHI::Texture* diffuseColorMap;
        RHI::TextureView* diffuseColorMapView;
    };

#define SSAO_KERNEL_SIZE 64
#define SSAO_RADIUS 0.3f
#define SSAO_NOISE_DIM 4

    class Renderer {
    public:
        explicit Renderer(Application* app ,RHI::RHIType rhiType) : app(app), rhiType(rhiType) {};
        ~Renderer();

        void Initialize();

        void RenderFrame();

        RHI::Device* GetDevice()
        {
            return device;
        }

        RHI::BindGroupLayout* GetLayout()
        {
            return renderableLayout;
        }

        RHI::Sampler* GetSampler()
        {
            return sampler;
        }

        RHI::Queue* GetQueue()
        {
            return graphicsQueue;
        }

        static const uint8_t BACK_BUFFER_COUNT = 2;

    private:
        void CreateInstanceAndSelectGPU();
        void RequestDeviceAndFetchQueues();
        void CreateSwapChain();
        void CreateCommandBuffer();
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateQuadBuffer();
        void PrepareUniformBuffers();
        void CreateSampler();
        void CreateFence();
        void CreatePipeline();
        void CreateBindGroupLayoutAndPipelineLayout();
        void CreateBindGroup();
        void PrepareOffscreen();

        void PopulateCommandBuffer();
        void SubmitCommandBufferAndPresent();

        void InitCamera();
        void LoadGLTF();
        void GenerateRenderables();

        static void safeDelete(void* p) {
            if (p != nullptr) {
                delete p;
            }
        }

        Application* app;
        Model* model;
        std::vector<Renderable> renderables;
        RHI::RHIType rhiType;
        Camera camera;

        RHI::Instance* instance = nullptr;
        RHI::Gpu* gpu = nullptr;
        RHI::Device* device = nullptr;
        RHI::Queue* graphicsQueue = nullptr;
        RHI::SwapChain* swapChain = nullptr;
        RHI::Buffer* vertexBuffer = nullptr;
        RHI::BufferView* vertexBufferView = nullptr;
        RHI::Buffer* indexBuffer = nullptr;
        RHI::BufferView* indexBufferView = nullptr;
        std::array<RHI::Texture*, BACK_BUFFER_COUNT> swapChainTextures {};
        std::array<RHI::TextureView*, BACK_BUFFER_COUNT> swapChainTextureViews {};

        RHI::Buffer* quadVertexBuffer = nullptr;
        RHI::BufferView* quadVertexBufferView = nullptr;
        RHI::Buffer* quadIndexBuffer = nullptr;
        RHI::BufferView* quadIndexBufferView = nullptr;

        RHI::CommandBuffer* commandBuffer = nullptr;
        RHI::Fence* fence = nullptr;
        RHI::Sampler* sampler = nullptr;
        RHI::Sampler* noiseSampler = nullptr;

        struct UBuffer {
            RHI::Buffer* buf;
            RHI::BufferView* bufView;

            ~UBuffer() {
                safeDelete(buf);
                safeDelete(bufView);
            }
        };

        struct UniformBuffers {
            UBuffer sceneParams;
            UBuffer ssaoKernel;
            UBuffer ssaoParams;

        } uniformBuffers;

        struct UBOSceneParams {
            glm::mat4 projection;
            glm::mat4 model;
            glm::mat4 view;
            float nearPlane = 0.1f;
            float farPlane = 64.0f;
        } uboSceneParams;

        struct UBOSSAOParams {
            glm::mat4 projection;
            int32_t ssao = true;
            int32_t ssaoOnly = false;
            int32_t ssaoBlur = true;
        } ubossaoParams;

        struct Noise {
            RHI::Texture* tex;
            RHI::TextureView* view;

            ~Noise() {
                safeDelete(tex);
                safeDelete(view);
            }
        } noise;

        struct ShaderModules {
            RHI::ShaderModule* gBufferVert;
            RHI::ShaderModule* gBufferFrag;
            RHI::ShaderModule* quadVert;
            RHI::ShaderModule* ssaoFrag;
            RHI::ShaderModule* ssaoBlurFrag;
            RHI::ShaderModule* compositionFrag;

            ~ShaderModules() {
                safeDelete(gBufferVert);
                safeDelete(gBufferFrag);
                safeDelete(quadVert);
                safeDelete(ssaoFrag);
                safeDelete(ssaoBlurFrag);
                safeDelete(compositionFrag);
            }

        } shaderModules;

        struct Pipelines {
            RHI::GraphicsPipeline* gBuffer = nullptr;
            RHI::GraphicsPipeline* ssao = nullptr;
            RHI::GraphicsPipeline* ssaoBlur = nullptr;
            RHI::GraphicsPipeline* composition = nullptr;

            ~Pipelines() {
                safeDelete(gBuffer);
                safeDelete(ssao);
                safeDelete(ssaoBlur);
                safeDelete(composition);
            }
        } pipelines;

        struct PipelineLayouts {
            RHI::PipelineLayout* gBuffer = nullptr;
            RHI::PipelineLayout* ssao = nullptr;
            RHI::PipelineLayout* ssaoBlur = nullptr;
            RHI::PipelineLayout* composition = nullptr;

            ~PipelineLayouts() {
                safeDelete(gBuffer);
                safeDelete(ssao);
                safeDelete(ssaoBlur);
                safeDelete(composition);
            }

        } pipelineLayouts;

        RHI::BindGroupLayout* renderableLayout = nullptr;

        struct BindGroupLayouts {
            RHI::BindGroupLayout* gBuffer = nullptr;
            RHI::BindGroupLayout* ssao = nullptr;
            RHI::BindGroupLayout* ssaoBlur = nullptr;
            RHI::BindGroupLayout* composition = nullptr;

            ~BindGroupLayouts() {
                safeDelete(gBuffer);
                safeDelete(ssao);
                safeDelete(ssaoBlur);
                safeDelete(composition);
            }
        } bindGroupLayouts;

        struct BindGroups {
            RHI::BindGroup* scene = nullptr;
            RHI::BindGroup* ssao = nullptr;
            RHI::BindGroup* ssaoBlur = nullptr;
            RHI::BindGroup* composition = nullptr;

            ~BindGroups() {
                safeDelete(scene);
                safeDelete(ssao);
                safeDelete(ssaoBlur);
                safeDelete(composition);
            }
        } bindGroups;

        struct ColorAttachment {
            RHI::Texture* texture;
            RHI::TextureView* view;

            ~ColorAttachment() {
                safeDelete(texture);
                safeDelete(view);
            }
        };

        struct GBufferOutput {
            ColorAttachment pos;
            ColorAttachment normal;
            ColorAttachment albedo;

        } gBufferOutput;

        ColorAttachment ssaoOutput;
        ColorAttachment ssaoBlurOutput;

        struct QuadVertex {
            glm::vec3 pos;
            glm::vec2 uv;
        };

        // helper function
        RHI::ShaderModule* CompileShader(const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage);
        void CreateAttachments(RHI::PixelFormat format, RHI::TextureAspect aspect, ColorAttachment* attachment, uint32_t width, uint32_t height);
        void CreateUniformBuffer(RHI::BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data = nullptr);
    };
}
