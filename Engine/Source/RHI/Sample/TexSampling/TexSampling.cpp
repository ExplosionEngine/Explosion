//
// Created by swtpotato on 2022/10/21.
//

#include <vector>
#include <array>

#include <Application.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace RHI;

class TexSamplingApplication : public Application {
public:
    NonCopyable(TexSamplingApplication)
    explicit TexSamplingApplication(const std::string& n) : Application(n) {}
    ~TexSamplingApplication() override = default;

protected:
    void OnCreate() override
    {
        SelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateSyncObjects();
        CreateTextureAndSampler();
        CreateUniformBuffer();
        CreateBindGroupLayout();
        CreateBindGroup();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffer();
        UpdateMVP();
    }

    void OnDrawFrame() override
    {
        inflightFences[nextFrameIndex]->Wait();
        auto backTextureIndex = swapChain->AcquireBackTexture(backBufferReadySemaphores[nextFrameIndex].Get());
        inflightFences[nextFrameIndex]->Reset();

        UniqueRef<CommandEncoder> commandEncoder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(
                GraphicsPassBeginInfo()
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(swapChainTextureViews[backTextureIndex].Get())));
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
        commandEncoder->End();

        graphicsQueue->Submit(
            commandBuffers[nextFrameIndex].Get(),
            QueueSubmitInfo()
                .WaitSemaphore(backBufferReadySemaphores[nextFrameIndex].Get())
                .SignalSemaphore(renderFinishedSemaphores[nextFrameIndex].Get())
                .SignalFence(inflightFences[nextFrameIndex].Get()));

        swapChain->Present(renderFinishedSemaphores[nextFrameIndex].Get());
        nextFrameIndex = (nextFrameIndex + 1) % backBufferCount;
    }

    void OnDestroy() override
    {
        Common::UniqueRef<Fence> fence = device->CreateFence(false);
        graphicsQueue->Flush(fence.Get());
        fence->Wait();
    }

private:
    static const uint8_t backBufferCount = 2;

    struct Vertex {
        FVec3 position;
        FVec2 uv;
    };

    FMat4x4 modelMatrix;

    void SelectGPU()
    {
        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        device = gpu->RequestDevice(
            DeviceCreateInfo()
                .Queue(QueueRequestInfo(QueueType::graphics, 1)));
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        surface = device->CreateSurface(
            SurfaceCreateInfo()
                .Window(GetPlatformWindow()));

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        swapChain = device->CreateSwapChain(
            SwapChainCreateInfo()
                .Format(swapChainFormat)
                .PresentMode(PresentMode::immediately)
                .TextureNum(backBufferCount)
                .Extent({ width, height })
                .Surface(surface.Get())
                .PresentQueue(graphicsQueue));

        for (auto i = 0; i < backBufferCount; i++) {
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

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(vertices.size() * sizeof(Vertex))
            .Usages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(BufferState::staging)
            .DebugName("vertexBuffer");

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::vertex)
            .Size(vertices.size() * sizeof(Vertex))
            .Offset(0)
            .ExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(indices.size() * sizeof(uint32_t))
            .Usages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(BufferState::staging)
            .DebugName("indexBuffer");

        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (indexBuffer != nullptr) {
            auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            indexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::index)
            .Size(indices.size() * sizeof(uint32_t))
            .Offset(0)
            .ExtendIndex(IndexFormat::uint32);
        indexBufferView = indexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateTextureAndSampler()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("../Test/Sample/TexSampling/Awesomeface.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        Assert(pixels != nullptr);

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(texWidth * texHeight * 4)
            .Usages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(BufferState::staging)
            .DebugName("stagingBuffer");

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
        sampler = device->CreateSampler(SamplerCreateInfo());

        texCommandBuffer = device->CreateCommandBuffer();
        UniqueRef<CommandEncoder> commandEncoder = texCommandBuffer->Begin();
        {
            UniqueRef<CopyPassCommandEncoder> copyPassEncoder = commandEncoder->BeginCopyPass();
            {
                copyPassEncoder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::undefined, TextureState::copyDst));
                TextureSubResourceInfo subResourceInfo {};
                subResourceInfo.mipLevel = 0;
                subResourceInfo.arrayLayerNum = 1;
                subResourceInfo.baseArrayLayer = 0;
                subResourceInfo.aspect = TextureAspect::color;
                copyPassEncoder->CopyBufferToTexture(pixelBuffer.Get(), sampleTexture.Get(), &subResourceInfo, {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1});
                copyPassEncoder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            }
            copyPassEncoder->EndPass();
        }
        commandEncoder->End();

        UniqueRef<Fence> fence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        graphicsQueue->Submit(texCommandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    void UpdateMVP()
    {
        modelMatrix = MatConsts<float, 4, 4>::identity * 0.5;
        modelMatrix.SetCol(3, 0.2f, 0.2f, 0.0f, 1.0f);

        if (uniformBuffer != nullptr) {
            auto* mapData = uniformBuffer->Map(MapMode::write, 0, sizeof(FMat4x4));
            memcpy(mapData, &modelMatrix, sizeof(FMat4x4));
            uniformBuffer->UnMap();
        }
    }

    void CreateUniformBuffer()
    {
        BufferCreateInfo createInfo = BufferCreateInfo()
            .Size(sizeof(FMat4x4))
            .Usages(BufferUsageBits::uniform | BufferUsageBits::mapWrite)
            .InitialState(BufferState::staging)
            .DebugName("uniformBuffer");

        uniformBuffer = device->CreateBuffer(createInfo);

        BufferViewCreateInfo viewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::uniformBinding)
            .Size(createInfo.size)
            .Offset(0);
        uniformBufferView = uniformBuffer->CreateBufferView(viewCreateInfo);
    }

    void CreateBindGroupLayout()
    {
        BindGroupLayoutCreateInfo createInfo(0);
        if (instance->GetRHIType() == RHIType::directX12) {
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ShaderStageBits::sPixel));
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), ShaderStageBits::sPixel));
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), ShaderStageBits::sVertex));
        } else {
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), ShaderStageBits::sPixel));
            createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(2)), ShaderStageBits::sVertex));
        }

        bindGroupLayout = device->CreateBindGroupLayout(createInfo);
    }

    void CreateBindGroup()
    {
        BindGroupCreateInfo createInfo(bindGroupLayout.Get());
        if (instance->GetRHIType() == RHIType::directX12) {
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), sampleTextureView.Get()));
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), sampler.Get()));
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), uniformBufferView.Get()));
        } else {
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), sampleTextureView.Get()));
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), sampler.Get()));
            createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(2)), uniformBufferView.Get()));
        }

        bindGroup = device->CreateBindGroup(createInfo);
    }

    void CreatePipelineLayout()
    {
        pipelineLayout = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .BindGroupLayout(bindGroupLayout.Get()));
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "../Test/Sample/TexSampling/TexSampling.hlsl", "VSMain", ShaderStageBits::sVertex);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "../Test/Sample/TexSampling/TexSampling.hlsl", "FSMain", ShaderStageBits::sPixel);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(shaderModuleCreateInfo);

        GraphicsPipelineCreateInfo createInfo = GraphicsPipelineCreateInfo()
            .Layout(pipelineLayout.Get())
            .VertexShader(vertexShader.Get())
            .PixelShader(fragmentShader.Get())
            .VertexState(
                VertexState()
                    .VertexBufferLayout(
                        VertexBufferLayout()
                            .StepMode(VertexStepMode::perVertex)
                            .Stride(sizeof(Vertex))
                            .Attribute(
                                VertexAttribute()
                                    .Format(VertexFormat::float32X3)
                                    .Offset(0)
                                    .SemanticName("POSITION")
                                    .SemanticIndex(0))
                            .Attribute(VertexAttribute().Format(VertexFormat::float32X2).Offset(offsetof(Vertex, uv)).SemanticName("TEXCOORD").SemanticIndex(0))))
            .FragmentState(
                FragmentState()
                    .ColorTarget(
                        ColorTargetState()
                            .Format(swapChainFormat)
                            .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
            .PrimitiveState(
                PrimitiveState()
                    .DepthClip(false)
                    .FrontFace(FrontFace::ccw)
                    .CullMode(CullMode::none)
                    .TopologyType(PrimitiveTopologyType::triangle)
                    .StripIndexFormat(IndexFormat::uint16))
            .DepthStencilState(
                DepthStencilState()
                    .DepthEnabled(false)
                    .StencilEnabled(false))
            .MultiSampleState(
                MultiSampleState()
                    .Count(1));

        pipeline = device->CreateGraphicsPipeline(createInfo);
    }

    void CreateSyncObjects()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            backBufferReadySemaphores[i] = device->CreateSemaphore();
            renderFinishedSemaphores[i] = device->CreateSemaphore();
            inflightFences[i] = device->CreateFence(true);
        }
    }

    void CreateCommandBuffer()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            commandBuffers[i] = device->CreateCommandBuffer();
        }
    }

    PixelFormat swapChainFormat = PixelFormat::max;
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
    UniqueRef<Buffer> uniformBuffer;
    UniqueRef<BufferView> uniformBufferView;
    UniqueRef<CommandBuffer> texCommandBuffer;
    UniqueRef<Buffer> pixelBuffer;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    std::array<UniqueRef<TextureView>, backBufferCount> swapChainTextureViews;
    UniqueRef<PipelineLayout> pipelineLayout;
    UniqueRef<GraphicsPipeline> pipeline;
    UniqueRef<ShaderModule> vertexShader;
    UniqueRef<ShaderModule> fragmentShader;
    std::array<UniqueRef<CommandBuffer>, backBufferCount> commandBuffers;
    std::array<UniqueRef<Semaphore>, backBufferCount> backBufferReadySemaphores;
    std::array<UniqueRef<Semaphore>, backBufferCount> renderFinishedSemaphores;
    std::array<UniqueRef<Fence>, backBufferCount> inflightFences;
    uint8_t nextFrameIndex = 0;
};

int main(int argc, char* argv[])
{
    TexSamplingApplication application("RHI-TexSampling");
    return application.Run(argc, argv);
}
