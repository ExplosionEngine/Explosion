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

        UniqueRef<CommandRecorder> commandRecorder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandRecorder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(ColorAttachment(swapChainTextureViews[backTextureIndex].Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipeline.Get());
                rasterRecorder->SetScissor(0, 0, width, height);
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetBindGroup(0, bindGroup.Get());
                rasterRecorder->SetVertexBuffer(0, vertexBufferView.Get());
                rasterRecorder->SetIndexBuffer(indexBufferView.Get());
                rasterRecorder->DrawIndexed(6, 1, 0, 0, 0);
            }
            rasterRecorder->EndPass();
            commandRecorder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::renderTarget, TextureState::present));
        }
        commandRecorder->End();

        graphicsQueue->Submit(
            commandBuffers[nextFrameIndex].Get(), QueueSubmitInfo()
                .AddWaitSemaphore(backBufferReadySemaphores[nextFrameIndex].Get())
                .AddSignalSemaphore(renderFinishedSemaphores[nextFrameIndex].Get())
                .SetSignalFence(inflightFences[nextFrameIndex].Get()));

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
                .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
        graphicsQueue = device->GetQueue(QueueType::graphics, 0);
    }

    void CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        surface = device->CreateSurface(SurfaceCreateInfo(GetPlatformWindow()));

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::max);

        swapChain = device->CreateSwapChain(
            SwapChainCreateInfo()
                .SetFormat(swapChainFormat)
                .SetPresentMode(PresentMode::immediately)
                .SetTextureNum(backBufferCount)
                .SetWidth(width)
                .SetHeight(height)
                .SetSurface(surface.Get())
                .SetPresentQueue(graphicsQueue));

        for (auto i = 0; i < backBufferCount; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
                TextureViewCreateInfo()
                    .SetDimension(TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(TextureAspect::color)
                    .SetType(TextureViewType::colorAttachment));
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
            .SetSize(vertices.size() * sizeof(Vertex))
            .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging)
            .SetDebugName("vertexBuffer");

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            vertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::vertex)
            .SetSize(vertices.size() * sizeof(Vertex))
            .SetOffset(0)
            .SetExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(indices.size() * sizeof(uint32_t))
            .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging)
            .SetDebugName("indexBuffer");

        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (indexBuffer != nullptr) {
            auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            indexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::index)
            .SetSize(indices.size() * sizeof(uint32_t))
            .SetOffset(0)
            .SetExtendIndex(IndexFormat::uint32);
        indexBufferView = indexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateTextureAndSampler()
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("../Test/Sample/RHI/TexSampling/Awesomeface.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        Assert(pixels != nullptr);

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(texWidth * texHeight * 4)
            .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(BufferState::staging)
            .SetDebugName("stagingBuffer");

        pixelBuffer = device->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, pixels, bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }
        stbi_image_free(pixels);

        sampleTexture = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba8Unorm)
                .SetMipLevels(1)
                .SetWidth(static_cast<uint32_t>(texWidth))
                .SetHeight(static_cast<uint32_t>(texHeight))
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
                .SetInitialState(TextureState::undefined));

        sampleTextureView = sampleTexture->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::color)
                .SetType(TextureViewType::textureBinding));

        // use the default attrib to create sampler
        sampler = device->CreateSampler(SamplerCreateInfo());

        texCommandBuffer = device->CreateCommandBuffer();
        UniqueRef<CommandRecorder> commandRecorder = texCommandBuffer->Begin();
        {
            UniqueRef<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
            {
                copyRecorder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::undefined, TextureState::copyDst));
                TextureSubResourceInfo subResourceInfo {};
                subResourceInfo.mipLevel = 0;
                subResourceInfo.arrayLayerNum = 1;
                subResourceInfo.baseArrayLayer = 0;
                subResourceInfo.aspect = TextureAspect::color;
                copyRecorder->CopyBufferToTexture(pixelBuffer.Get(), sampleTexture.Get(), &subResourceInfo, {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1});
                copyRecorder->ResourceBarrier(Barrier::Transition(sampleTexture.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            }
            copyRecorder->EndPass();
        }
        commandRecorder->End();

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
            .SetSize(sizeof(FMat4x4))
            .SetUsages(BufferUsageBits::uniform | BufferUsageBits::mapWrite)
            .SetInitialState(BufferState::staging)
            .SetDebugName("uniformBuffer");

        uniformBuffer = device->CreateBuffer(createInfo);

        BufferViewCreateInfo viewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::uniformBinding)
            .SetSize(createInfo.size)
            .SetOffset(0);
        uniformBufferView = uniformBuffer->CreateBufferView(viewCreateInfo);
    }

    void CreateBindGroupLayout()
    {
        BindGroupLayoutCreateInfo createInfo(0);
        // TODO use reflection
        if (instance->GetRHIType() == RHIType::directX12) {
            createInfo.AddEntry(BindGroupLayoutEntry(
                ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                , ShaderStageBits::sPixel));
            createInfo.AddEntry(BindGroupLayoutEntry(
                ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                , ShaderStageBits::sPixel));
            createInfo.AddEntry(BindGroupLayoutEntry(
                ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                , ShaderStageBits::sVertex));
        } else {
            createInfo.AddEntry(
                BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
            createInfo.AddEntry(
                BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), ShaderStageBits::sPixel));
            createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(2))
                                                     , ShaderStageBits::sVertex));
        }

        bindGroupLayout = device->CreateBindGroupLayout(createInfo);
    }

    void CreateBindGroup()
    {
        BindGroupCreateInfo createInfo(bindGroupLayout.Get());
        // TODO use reflection
        if (instance->GetRHIType() == RHIType::directX12) {
            createInfo.AddEntry(
                BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                               , sampleTextureView.Get()));
            createInfo.AddEntry(
                BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                               , sampler.Get()));
            createInfo.AddEntry(BindGroupEntry(
                ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                , uniformBufferView.Get()));
        } else {
            createInfo.AddEntry(
                BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), sampleTextureView.Get()));
            createInfo.AddEntry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), sampler.Get()));
            createInfo.AddEntry(
                BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(2)), uniformBufferView.Get()));
        }

        bindGroup = device->CreateBindGroup(createInfo);
    }

    void CreatePipelineLayout()
    {
        pipelineLayout = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayout.Get()));
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "../Test/Sample/RHI/TexSampling/TexSampling.hlsl", "VSMain", ShaderStageBits::sVertex);
        vertexShader = device->CreateShaderModule(ShaderModuleCreateInfo("VSMain", vsByteCode));

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "../Test/Sample/RHI/TexSampling/TexSampling.hlsl", "FSMain", ShaderStageBits::sPixel);
        fragmentShader = device->CreateShaderModule(ShaderModuleCreateInfo("FSMain", fsByteCode));

        RasterPipelineCreateInfo createInfo = RasterPipelineCreateInfo(pipelineLayout.Get())
            .SetVertexShader(vertexShader.Get())
            .SetPixelShader(fragmentShader.Get())
            .SetFragmentState(
                FragmentState()
                    .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all)))
            .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint16, FrontFace::ccw, CullMode::none));

        // TODO use reflection
        if (rhiType == RHIType::directX12) {
            createInfo.SetVertexState(
                VertexState()
                    .AddVertexBufferLayout(
                        VertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                            .AddAttribute(VertexAttribute(HlslVertexBinding("POSITION", 0), VertexFormat::float32X3, 0))
                            .AddAttribute(VertexAttribute(HlslVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(Vertex, uv)))));
        } else {
            createInfo.SetVertexState(
                VertexState()
                    .AddVertexBufferLayout(
                        VertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                            .AddAttribute(VertexAttribute(GlslVertexBinding(0), VertexFormat::float32X3, 0))
                            .AddAttribute(VertexAttribute(GlslVertexBinding(1), VertexFormat::float32X2, offsetof(Vertex, uv)))));
        }

        pipeline = device->CreateRasterPipeline(createInfo);
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
    UniqueRef<RasterPipeline> pipeline;
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
