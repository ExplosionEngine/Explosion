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
    NonCopyable(TexSamplingApplication)
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
        CreateFence();
        CreateTextureAndSampler();
        CreateBindGroupLayout();
        CreateBindGroup();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffer();
    }

    void OnDrawFrame() override
    {
        PopulateCommandBuffer();
        SubmitCommandBufferAndPresent();
    }

    void OnDestroy() override
    {
        graphicsQueue->Wait(fence.Get());
        fence->Wait();
    }

private:
    static const uint8_t backBufferCount = 2;

    void CreateInstanceAndSelectGPU()
    {
        instance = Instance::GetByType(rhiType);

        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = {{QueueType::graphics, 1}};
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(createInfo);
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        SurfaceCreateInfo surfaceCreateInfo {};
        surfaceCreateInfo.window = GetPlatformWindow();
        surface = device->CreateSurface(surfaceCreateInfo);

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = swapChainFormat;
        swapChainCreateInfo.presentMode = PresentMode::immediately;
        swapChainCreateInfo.textureNum = backBufferCount;
        swapChainCreateInfo.extent = {width, height};
        swapChainCreateInfo.surface = surface.Get();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(swapChainCreateInfo);

        for (auto i = 0; i < swapChainCreateInfo.textureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.dimension = TextureViewDimension::tv2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::color;
            viewCreateInfo.type = TextureViewType::colorAttachment;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(viewCreateInfo);
        }
    }

    void CreateVertexBuffer()
    {
        std::vector<Vertex> vertices = {
            {{-.5f, -.5f, .0f}, {.0f, 1.0f}},
            {{.5f, -.5f, .0f}, {1.0f, 1.0f}},
            {{.5f, .5f, .0f}, {1.0f, .0f}},
            {{-.5f, .5f, .0f}, {0.f, .0f}},
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc;
        bufferCreateInfo.debugName = "quadBuffer";
        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.type = BufferViewType::vertex;
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc;
        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (indexBuffer != nullptr) {
            auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            indexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.type = BufferViewType::index;
        bufferViewCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::uint32;
        indexBufferView = indexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateTextureAndSampler()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("../Test/Sample/TexSampling/Awesomeface.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        Assert(pixels != nullptr);

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = texWidth * texHeight * 4;
        bufferCreateInfo.usages = BufferUsageBits::mapWrite | BufferUsageBits::copySrc;
        pixelBuffer = device->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, pixels, bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }
        stbi_image_free(pixels);

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::rgba8Unorm;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};
        texCreateInfo.dimension = TextureDimension::t2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::copyDst | TextureUsageBits::textureBinding;
        sampleTexture = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::tv2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::color;
        sampleTextureView = sampleTexture->CreateTextureView(viewCreateInfo);

        // use the default attrib to create sampler
        SamplerCreateInfo samplerCreateInfo {};
        sampler = device->CreateSampler(samplerCreateInfo);

        texCommandBuffer = device->CreateCommandBuffer();
        UniqueRef<CommandEncoder> commandEncoder = texCommandBuffer->Begin();
        {
            commandEncoder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::undefined, TextureState::copyDst));
            TextureSubResourceInfo subResourceInfo {};
            subResourceInfo.mipLevel = 0;
            subResourceInfo.arrayLayerNum = 1;
            subResourceInfo.baseArrayLayer = 0;
            subResourceInfo.aspect = TextureAspect::color;
            commandEncoder->CopyBufferToTexture(pixelBuffer.Get(), sampleTexture.Get(), &subResourceInfo, {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1});
            commandEncoder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
        }
        commandEncoder->End();
        fence->Reset();
        graphicsQueue->Submit(texCommandBuffer.Get(), fence.Get());
        fence->Wait();
    }

    void CreateBindGroupLayout()
    {
        std::vector<BindGroupLayoutEntry> entries(2);
        entries[0].binding.type = BindingType::texture;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::sPixel);
        entries[1].binding.type = BindingType::sampler;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::sPixel);
        if (instance->GetRHIType() == RHI::RHIType::directX12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::texture, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::sampler, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }

        BindGroupLayoutCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;

        bindGroupLayout = device->CreateBindGroupLayout(createInfo);
    }

    void CreateBindGroup()
    {
        std::vector<BindGroupEntry> entries(2);
        entries[0].binding.type = BindingType::texture;
        entries[0].textureView = sampleTextureView.Get();
        entries[1].binding.type = BindingType::sampler;
        entries[1].sampler = sampler.Get();
        if (instance->GetRHIType() == RHI::RHIType::directX12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::texture, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::sampler, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }

        BindGroupCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayout.Get();

        bindGroup = device->CreateBindGroup(createInfo);
    }

    void CreatePipelineLayout()
    {
        std::vector<BindGroupLayout*> bindGroupLayouts = { bindGroupLayout.Get() };

        PipelineLayoutCreateInfo createInfo {};
        createInfo.bindGroupLayoutNum = bindGroupLayouts.size();
        createInfo.bindGroupLayouts = bindGroupLayouts.data();
        pipelineLayout = device->CreatePipelineLayout(createInfo);
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "../Test/Sample/TexSampling/TexSampling.hlsl", "VSMain", RHI::ShaderStageBits::sVertex);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "../Test/Sample/TexSampling/TexSampling.hlsl", "FSMain", RHI::ShaderStageBits::sPixel);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::array<VertexAttribute, 2> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::float32X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::float32X2;
        vertexAttributes[1].offset = offsetof(Vertex, uv);
        vertexAttributes[1].semanticName = "TEXCOORD";
        vertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::perVertex;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        std::array<ColorTargetState, 1> colorTargetStates {};
        colorTargetStates[0].format = swapChainFormat;
        colorTargetStates[0].writeFlags = ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha;

        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexShader = vertexShader.Get();
        createInfo.pixelShader = fragmentShader.Get();
        createInfo.layout = pipelineLayout.Get();
        createInfo.vertexState.bufferLayoutNum = 1;
        createInfo.vertexState.bufferLayouts = &vertexBufferLayout;
        createInfo.fragmentState.colorTargetNum = colorTargetStates.size();
        createInfo.fragmentState.colorTargets = colorTargetStates.data();
        createInfo.primitiveState.depthClip = false;
        createInfo.primitiveState.frontFace = RHI::FrontFace::ccw;
        createInfo.primitiveState.cullMode = CullMode::none;
        createInfo.primitiveState.topologyType = RHI::PrimitiveTopologyType::triangle;
        createInfo.primitiveState.stripIndexFormat = IndexFormat::uint16;
        createInfo.depthStencilState.depthEnable = false;
        createInfo.depthStencilState.stencilEnable = false;
        createInfo.multiSampleState.count = 1;
        pipeline = device->CreateGraphicsPipeline(createInfo);
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
        UniqueRef<CommandEncoder> commandEncoder = commandBuffer->Begin();
        {
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::clear;
            colorAttachments[0].storeOp = StoreOp::store;
            colorAttachments[0].view = swapChainTextureViews[backTextureIndex].Get();
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipeline.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetBindGroup(0, bindGroup.Get());
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(indexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::renderTarget, TextureState::present));
        }
        commandEncoder->SwapChainSync(swapChain.Get());
        commandEncoder->End();
    }

    void SubmitCommandBufferAndPresent()
    {
        fence->Reset();
        graphicsQueue->Submit(commandBuffer.Get(), fence.Get());
        swapChain->Present();
        fence->Wait();
    }

    PixelFormat swapChainFormat = PixelFormat::max;
    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    UniqueRef<Device> device;
    Queue* graphicsQueue = nullptr;
    UniqueRef<Surface> surface;
    UniqueRef<SwapChain> swapChain;
    UniqueRef<Buffer> vertexBuffer;
    UniqueRef<BufferView> vertexBufferView;
    UniqueRef<Buffer> indexBuffer;
    UniqueRef<BufferView> indexBufferView;
    UniqueRef<BindGroupLayout> bindGroupLayout;
    UniqueRef<BindGroup> bindGroup;
    UniqueRef<Texture> sampleTexture;
    UniqueRef<TextureView> sampleTextureView;
    UniqueRef<Sampler> sampler;
    UniqueRef<CommandBuffer> texCommandBuffer;
    UniqueRef<Buffer> pixelBuffer;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    std::array<UniqueRef<TextureView>, backBufferCount> swapChainTextureViews;
    UniqueRef<PipelineLayout> pipelineLayout;
    UniqueRef<GraphicsPipeline> pipeline;
    UniqueRef<ShaderModule> vertexShader;
    UniqueRef<ShaderModule> fragmentShader;
    UniqueRef<CommandBuffer> commandBuffer;
    UniqueRef<Fence> fence;
};

int main(int argc, char* argv[])
{
    TexSamplingApplication application("RHI-TexSampling");
    return application.Run(argc, argv);
}
