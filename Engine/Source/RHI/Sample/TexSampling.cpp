//
// Created by swtpotato on 2022/10/21.
//

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include <Application.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace RHI;

struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
};

class TexSamplingApplication : public Application {
public:
    NON_COPYABLE(TexSamplingApplication)
    explicit TexSamplingApplication(const std::string& n) : Application(n) {}
    ~TexSamplingApplication() override = default;

protected:
    void OnCreate() override
    {
        CreateInstanceAndSelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateTextureAndSampler();
        CreateBindGroupLayout();
        CreateBindGroup();
        CreatePipelineLayout();
        CreatePipeline();
        CreateFence();
        CreateCommandBuffer();
    }

    void OnDrawFrame() override
    {
        PopulateCommandBuffer();
        SubmitCommandBufferAndPresent();
    }

    void OnDestroy() override
    {
        graphicsQueue->Wait(fence);
        fence->Wait();
        fence->Destroy();
        commandBuffer->Destroy();
        texCommandBuffer->Destroy();
        vertexShader->Destroy();
        fragmentShader->Destroy();
        pipeline->Destroy();
        pipelineLayout->Destroy();
        for (auto* textureView: swapChainTextureViews) {
            textureView->Destroy();
        }
        vertexBufferView->Destroy();
        vertexBuffer->Destroy();
        indexBufferView->Destroy();
        indexBuffer->Destroy();
        bindGroupLayout->Destroy();
        bindGroup->Destroy();
        sampleTextureView->Destroy();
        sampleTexture->Destroy();
        sampler->Destroy();
        pixelBuffer->Destroy();
        swapChain->Destroy();
        device->Destroy();
    }

private:
    static const uint8_t BACK_BUFFER_COUNT = 2;

    void CreateInstanceAndSelectGPU()
    {
        instance = Instance::CreateByType(rhiType);

        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = {{QueueType::GRAPHICS, 1}};
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(&createInfo);
        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);
    }

    void CreateSwapChain()
    {
        SwapChainCreateInfo swapChainCreateInfo {};
        if (rhiType == RHIType::VULKAN) {
            swapChainCreateInfo.format = PixelFormat::BGRA8_UNORM;
        } else {
            swapChainCreateInfo.format = PixelFormat::RGBA8_UNORM;
        }
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = BACK_BUFFER_COUNT;
        swapChainCreateInfo.extent = {width, height};
        swapChainCreateInfo.window = GetPlatformWindow();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(&swapChainCreateInfo);

        for (auto i = 0; i < swapChainCreateInfo.textureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            if (rhiType == RHIType::VULKAN) {
                viewCreateInfo.format = PixelFormat::BGRA8_UNORM;
            } else {
                viewCreateInfo.format = PixelFormat::RGBA8_UNORM;
            }
            viewCreateInfo.dimension = TextureViewDimension::TV_2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::COLOR;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(&viewCreateInfo);
        }
    }

    void CreateVertexBuffer()
    {
        std::vector<Vertex> vertices = {
            {{-.5f, -.5f, 0.f}, {0.f, 0.f}},
            {{.5f, -.5f, 0.f}, {1.f, 0.f}},
            {{.5f, .5f, 0.f}, {1.f, 1.f}},
            {{-.5f, .5f, 0.f}, {0.f, 1.f}},
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::INDEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        indexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (indexBuffer != nullptr) {
            auto* data = indexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            indexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::UINT32;
        indexBufferView = indexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void CreateTextureAndSampler()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("./awesomeface.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        Assert(pixels != nullptr);

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = texWidth * texHeight * 4;
        bufferCreateInfo.usages = BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        pixelBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, pixels, bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }
        stbi_image_free(pixels);

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::RGBA8_UNORM;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {static_cast<size_t>(texWidth), static_cast<size_t>(texHeight), 1};
        texCreateInfo.dimension = TextureDimension::T_2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::TEXTURE_BINDING;
        sampleTexture = device->CreateTexture(&texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.format = PixelFormat::RGBA8_UNORM;
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        sampleTextureView = sampleTexture->CreateTextureView(&viewCreateInfo);

        // use the default attrib to create sampler
        SamplerCreateInfo samplerCreateInfo {};
        sampler = device->CreateSampler(&samplerCreateInfo);

        texCommandBuffer = device->CreateCommandBuffer();
        auto* commandEncoder = texCommandBuffer->Begin();
        commandEncoder->ResourceBarrier(Barrier::Transition(sampleTexture, TextureState::UNDEFINED, TextureState::COPY_DST));
        TextureSubResourceInfo subResourceInfo {};
        subResourceInfo.mipLevel = 0;
        subResourceInfo.arrayLayerNum = 1;
        subResourceInfo.baseArrayLayer = 0;
        subResourceInfo.aspect = TextureAspect::COLOR;
        commandEncoder->CopyBufferToTexture(pixelBuffer, sampleTexture, &subResourceInfo, {static_cast<size_t>(texWidth), static_cast<size_t>(texHeight), 1});
        commandEncoder->ResourceBarrier(Barrier::Transition(sampleTexture, TextureState::COPY_DST, TextureState::SHADER_READ_ONLY));
        commandEncoder->End();

        graphicsQueue->Submit(texCommandBuffer, nullptr);
    }

    void CreateBindGroupLayout()
    {
        std::vector<BindGroupLayoutEntry> entries(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);

        BindGroupLayoutCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;

        bindGroupLayout = device->CreateBindGroupLayout(&createInfo);
    }

    void CreateBindGroup()
    {
        std::vector<BindGroupEntry> entries(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].textureView = sampleTextureView;
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].sampler = sampler;

        BindGroupCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayout;

        bindGroup = device->CreateBindGroup(&createInfo);
    }

    void CreatePipelineLayout()
    {
        PipelineLayoutCreateInfo createInfo {};
        createInfo.bindGroupNum = 1;
        createInfo.bindGroupLayouts = &bindGroupLayout;
        pipelineLayout = device->CreatePipelineLayout(&createInfo);
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "Shader/Sample/TexSampling.hlsl", "VSMain", RHI::ShaderStageBits::VERTEX);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "Shader/Sample/TexSampling.hlsl", "FSMain", RHI::ShaderStageBits::FRAGMENT);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::array<VertexAttribute, 2> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::FLOAT32_X2;
        vertexAttributes[1].offset = offsetof(Vertex, uv);
        vertexAttributes[1].semanticName = "TEXCOORD0";
        vertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        std::array<ColorTargetState, 1> colorTargetStates {};
        if (rhiType == RHIType::VULKAN) {
            colorTargetStates[0].format = PixelFormat::BGRA8_UNORM;
        } else {
            colorTargetStates[0].format = PixelFormat::RGBA8_UNORM;
        }
        colorTargetStates[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexShader = vertexShader;
        createInfo.fragmentShader = fragmentShader;
        createInfo.layout = pipelineLayout;
        createInfo.vertex.bufferLayoutNum = 1;
        createInfo.vertex.bufferLayouts = &vertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates.size();
        createInfo.fragment.colorTargets = colorTargetStates.data();
        createInfo.primitive.depthClip = false;
        createInfo.primitive.frontFace = RHI::FrontFace::CCW;
        createInfo.primitive.cullMode = CullMode::NONE;
        createInfo.primitive.topologyType = RHI::PrimitiveTopologyType::TRIANGLE;
        createInfo.primitive.stripIndexFormat = IndexFormat::UINT16;
        createInfo.depthStencil.depthEnable = false;
        createInfo.depthStencil.stencilEnable = false;
        createInfo.multiSample.count = 1;
        pipeline = device->CreateGraphicsPipeline(&createInfo);
    }

    void CreateFence()
    {
        fence = device->CreateFence();
    }

    void CreateCommandBuffer()
    {
        commandBuffer = device->CreateCommandBuffer();
    }

    void PopulateCommandBuffer()
    {
        auto backTextureIndex = swapChain->AcquireBackTexture();
        CommandEncoder* commandEncoder = commandBuffer->Begin();
        {
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = swapChainTextureViews[backTextureIndex];
            colorAttachments[0].resolveTarget = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.pipeline = pipeline;
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::PRESENT, TextureState::RENDER_TARGET));
            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroup);
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView);
                graphicsEncoder->SetIndexBuffer(indexBufferView);
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::RENDER_TARGET, TextureState::PRESENT));
        }
        commandEncoder->End();
        commandEncoder->SwapChainSync(swapChain);
    }

    void SubmitCommandBufferAndPresent()
    {
        graphicsQueue->Submit(commandBuffer, fence);
        fence->Wait();
        swapChain->Present();
    }

    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    Device* device = nullptr;
    Queue* graphicsQueue = nullptr;
    SwapChain* swapChain = nullptr;
    Buffer* vertexBuffer = nullptr;
    BufferView* vertexBufferView = nullptr;
    Buffer* indexBuffer = nullptr;
    BufferView* indexBufferView = nullptr;
    BindGroupLayout* bindGroupLayout = nullptr;
    BindGroup* bindGroup = nullptr;
    Texture* sampleTexture = nullptr;
    TextureView* sampleTextureView = nullptr;
    Sampler* sampler = nullptr;
    CommandBuffer* texCommandBuffer = nullptr;
    Buffer* pixelBuffer = nullptr;
    std::array<Texture*, BACK_BUFFER_COUNT> swapChainTextures {};
    std::array<TextureView*, BACK_BUFFER_COUNT> swapChainTextureViews {};
    PipelineLayout* pipelineLayout = nullptr;
    GraphicsPipeline* pipeline = nullptr;
    ShaderModule* vertexShader = nullptr;
    ShaderModule* fragmentShader = nullptr;
    CommandBuffer* commandBuffer = nullptr;
    Fence* fence = nullptr;
};

int main(int argc, char* argv[])
{
    TexSamplingApplication application("RHI-TexSampling");
    return application.Run(argc, argv);
}