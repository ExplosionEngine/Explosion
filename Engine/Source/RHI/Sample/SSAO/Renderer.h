//
// Created by Junkang on 2023/3/8.
//

#pragma once
#include <random>
#include <array>

#include <Application.h>

#include "GLTFParser.h"
#include "Camera.h"

using namespace RHI;
namespace Example {
    class Renderer;
    class Renderable {
    public:
        NON_COPYABLE(Renderable)
        Renderable()=default;
        ~Renderable();

        void InitalizeWithPrimitive(Renderer* renderer, Mesh* primitive);

        uint32_t indexCount;
        uint32_t firstIndex;

        UniqueRef<BindGroup> bindGroup;
        UniqueRef<Texture> diffuseColorMap;
        UniqueRef<TextureView> diffuseColorMapView;
    };

const unsigned int SSAO_KERNEL_SIZE = 64;
const float SSAO_RADIUS  = 0.3f;
const unsigned int SSAO_NOISE_DIM = 16;

    class Renderer {
    public:
        explicit Renderer(Application* app, RHIType rhiType) : app(app), rhiType(rhiType) {};
        ~Renderer();

        void Initialize();

        void RenderFrame();

        Instance* GetInstance() {
            return instance;
        }

        Device* GetDevice()
        {
            return device.Get();
        }

        BindGroupLayout* GetLayout()
        {
            return renderableLayout.Get();
        }

        Sampler* GetSampler()
        {
            return sampler.Get();
        }

        Queue* GetQueue()
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

        Application* app;
        Model* model;
        std::vector<Renderable*> renderables;
        RHIType rhiType;
        Camera camera;

        PixelFormat swapChainFormat = PixelFormat::max;
        Instance* instance = nullptr;
        Gpu* gpu = nullptr;
        UniqueRef<Device> device = nullptr;
        Queue* graphicsQueue = nullptr;
        UniqueRef<Surface> surface = nullptr;
        UniqueRef<SwapChain> swapChain = nullptr;
        UniqueRef<Buffer> vertexBuffer = nullptr;
        UniqueRef<BufferView> vertexBufferView = nullptr;
        UniqueRef<Buffer> indexBuffer = nullptr;
        UniqueRef<BufferView> indexBufferView = nullptr;
        std::array<Texture*, BACK_BUFFER_COUNT> swapChainTextures {};
        std::array<Common::UniqueRef<TextureView>, BACK_BUFFER_COUNT> swapChainTextureViews {};

        UniqueRef<Buffer> quadVertexBuffer = nullptr;
        UniqueRef<BufferView> quadVertexBufferView = nullptr;
        UniqueRef<Buffer> quadIndexBuffer = nullptr;
        UniqueRef<BufferView> quadIndexBufferView = nullptr;

        UniqueRef<CommandBuffer> commandBuffer = nullptr;
        UniqueRef<Fence> fence = nullptr;
        UniqueRef<Sampler> sampler = nullptr;
        UniqueRef<Sampler> noiseSampler = nullptr;

        struct UBuffer {
            UniqueRef<Buffer> buf;
            UniqueRef<BufferView> bufView;
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
            int32_t ssao = 1;
            int32_t ssaoOnly = 0;
            int32_t ssaoBlur = 1;
        } ubossaoParams;

        struct Noise {
            UniqueRef<Texture> tex;
            UniqueRef<TextureView> view;
        } noise;

        struct ShaderModules {
            UniqueRef<ShaderModule> gBufferVert;
            UniqueRef<ShaderModule> gBufferFrag;
            UniqueRef<ShaderModule> quadVert;
            UniqueRef<ShaderModule> ssaoFrag;
            UniqueRef<ShaderModule> ssaoBlurFrag;
            UniqueRef<ShaderModule> compositionFrag;

        } shaderModules;

        struct Pipelines {
            UniqueRef<GraphicsPipeline> gBuffer;
            UniqueRef<GraphicsPipeline> ssao;
            UniqueRef<GraphicsPipeline> ssaoBlur;
            UniqueRef<GraphicsPipeline> composition;
        } pipelines;

        struct PipelineLayouts {
            UniqueRef<PipelineLayout> gBuffer;
            UniqueRef<PipelineLayout> ssao;
            UniqueRef<PipelineLayout> ssaoBlur;
            UniqueRef<PipelineLayout> composition;
        } pipelineLayouts;

        UniqueRef<BindGroupLayout> renderableLayout;

        struct BindGroupLayouts {
            UniqueRef<BindGroupLayout> gBuffer;
            UniqueRef<BindGroupLayout> ssao;
            UniqueRef<BindGroupLayout> ssaoBlur;
            UniqueRef<BindGroupLayout> composition;
        } bindGroupLayouts;

        struct BindGroups {
            UniqueRef<BindGroup> scene;
            UniqueRef<BindGroup> ssao;
            UniqueRef<BindGroup> ssaoBlur;
            UniqueRef<BindGroup> composition;
        } bindGroups;

        struct ColorAttachment {
            UniqueRef<Texture> texture;
            UniqueRef<TextureView> view;
        };

        ColorAttachment gBufferPos;
        ColorAttachment gBufferNormal;
        ColorAttachment gBufferAlbedo;
        ColorAttachment gBufferDepth;

        ColorAttachment ssaoOutput;
        ColorAttachment ssaoBlurOutput;

        struct QuadVertex {
            glm::vec3 pos;
            glm::vec2 uv;
        };

        // helper function
        ShaderModule* CompileShader(const std::string& fileName, const std::string& entryPoint, ShaderStageBits shaderStage);
        void CreateAttachments(PixelFormat format, TextureUsageFlags flags, TextureAspect aspect, ColorAttachment* attachment, uint32_t width, uint32_t height);
        void CreateUniformBuffer(BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data = nullptr);
    };
}
