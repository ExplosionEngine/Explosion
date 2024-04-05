//
// Created by Junkang on 2023/3/8.
//

#include <vector>
#include <array>
#include <memory>
#include <random>

#include <Application.h>
#include "GLTFParser.h"

using namespace RHI;

class SSAOApplication : public Application {
public:
    NonCopyable(SSAOApplication)
    explicit SSAOApplication(const std::string& n) : Application(n) {}
    ~SSAOApplication() override = default;

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

    Fence* GetInflightFences(uint8_t index)
    {
        return inflightFences[index].Get();
    }

protected:
    void OnCreate() override
    {
        InitCamera();
        LoadGLTF();
        SelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateSyncObjects();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateQuadBuffer();
        PrepareOffscreen();
        PrepareUniformBuffers();
        CreateSampler();
        CreateBindGroupLayoutAndPipelineLayout();
        CreateBindGroup();
        CreatePipeline();
        CreateCommandBuffer();
        GenerateRenderables();
    }

    void OnDrawFrame() override
    {
        uboSceneParams.view = camera->GetViewMatrix();

        auto* pMap = uniformBuffers.sceneParams.buf->Map(MapMode::write, 0, sizeof(UBOSceneParams));
        memcpy(pMap, &uboSceneParams, sizeof(UBOSceneParams));
        uniformBuffers.sceneParams.buf->UnMap();

        inflightFences[nextFrameIndex]->Wait();
        auto backTextureIndex = swapChain->AcquireBackTexture(backBufferReadySemaphores[nextFrameIndex].Get());
        inflightFences[nextFrameIndex]->Reset();

        UniqueRef<CommandRecorder> commandRecorder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferDepth.texture.Get(), TextureState::depthStencilReadonly, TextureState::depthStencilWrite));

            UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(RHI::ColorAttachment(gBufferPos.rtv.Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .AddColorAttachment(RHI::ColorAttachment(gBufferNormal.rtv.Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .AddColorAttachment(RHI::ColorAttachment(gBufferAlbedo.rtv.Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .SetDepthStencilAttachment(DepthStencilAttachment(gBufferDepth.view.Get(), true, LoadOp::clear, StoreOp::store, 0.0f)));
            {
                rasterRecorder->SetPipeline(pipelines.gBuffer.Get());
                rasterRecorder->SetScissor(0, 0, width, height);
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetBindGroup(0, bindGroups.scene.Get());
                rasterRecorder->SetVertexBuffer(0, vertexBufferView.Get());
                rasterRecorder->SetIndexBuffer(indexBufferView.Get());

                for (auto& renderable : renderables) {
                    rasterRecorder->SetBindGroup(1, renderable->bindGroup.Get());
                    rasterRecorder->DrawIndexed(renderable->indexCount, 1, renderable->firstIndex, 0, 0);
                }
            }
            rasterRecorder->EndPass();

            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandRecorder->ResourceBarrier(Barrier::Transition(gBufferDepth.texture.Get(), TextureState::depthStencilWrite, TextureState::depthStencilReadonly));
        }

        {
            // ssao
            commandRecorder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));

            UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(RHI::ColorAttachment(ssaoOutput.rtv.Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipelines.ssao.Get());
                rasterRecorder->SetScissor(0, 0, width, height);
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetBindGroup(0, bindGroups.ssao.Get());
                rasterRecorder->SetVertexBuffer(0, quadVertexBufferView.Get());
                rasterRecorder->SetIndexBuffer(quadIndexBufferView.Get());
                rasterRecorder->DrawIndexed(6, 1, 0, 0, 0);
            }
            rasterRecorder->EndPass();
            commandRecorder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
        }

        {
            // ssaoBlur
            commandRecorder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));

            UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(RHI::ColorAttachment(ssaoBlurOutput.rtv.Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipelines.ssaoBlur.Get());
                rasterRecorder->SetScissor(0, 0, width, height);
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetBindGroup(0, bindGroups.ssaoBlur.Get());
                rasterRecorder->SetVertexBuffer(0, quadVertexBufferView.Get());
                rasterRecorder->SetIndexBuffer(quadIndexBufferView.Get());
                rasterRecorder->DrawIndexed(6, 1, 0, 0, 0);
            }
            rasterRecorder->EndPass();
            commandRecorder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
        }

        {
            // composition
            commandRecorder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::present, TextureState::renderTarget));
            UniqueRef<RasterPassCommandRecorder> rasterRecorder = commandRecorder->BeginRasterPass(
                RasterPassBeginInfo()
                    .AddColorAttachment(RHI::ColorAttachment(swapChainTextureViews[backTextureIndex].Get(), LoadOp::clear, StoreOp::store, LinearColorConsts::black)));
            {
                rasterRecorder->SetPipeline(pipelines.composition.Get());
                rasterRecorder->SetScissor(0, 0, width, height);
                rasterRecorder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                rasterRecorder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                rasterRecorder->SetBindGroup(0, bindGroups.composition.Get());
                rasterRecorder->SetVertexBuffer(0, quadVertexBufferView.Get());
                rasterRecorder->SetIndexBuffer(quadIndexBufferView.Get());
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
    static constexpr uint8_t ssaoKernelSize = 64;
    static constexpr uint8_t ssaoNoiseDim = 16;
    static constexpr uint8_t backBufferCount = 2;

    PixelFormat swapChainFormat = PixelFormat::max;
    Gpu* gpu = nullptr;
    UniqueRef<Device> device = nullptr;
    Queue* graphicsQueue = nullptr;
    UniqueRef<Surface> surface = nullptr;
    UniqueRef<SwapChain> swapChain = nullptr;
    UniqueRef<Buffer> vertexBuffer = nullptr;
    UniqueRef<BufferView> vertexBufferView = nullptr;
    UniqueRef<Buffer> indexBuffer = nullptr;
    UniqueRef<BufferView> indexBufferView = nullptr;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    std::array<Common::UniqueRef<TextureView>, backBufferCount> swapChainTextureViews {};

    UniqueRef<Buffer> quadVertexBuffer = nullptr;
    UniqueRef<BufferView> quadVertexBufferView = nullptr;
    UniqueRef<Buffer> quadIndexBuffer = nullptr;
    UniqueRef<BufferView> quadIndexBufferView = nullptr;

    std::array<UniqueRef<CommandBuffer>, backBufferCount> commandBuffers {};
    std::array<UniqueRef<Semaphore>, backBufferCount> backBufferReadySemaphores {};
    std::array<UniqueRef<Semaphore>, backBufferCount> renderFinishedSemaphores {};
    std::array<UniqueRef<Fence>, backBufferCount> inflightFences {};
    uint8_t nextFrameIndex = 0;

    UniqueRef<Sampler> sampler = nullptr;
    UniqueRef<Sampler> noiseSampler = nullptr;

    struct Renderable {
        uint32_t indexCount;
        uint32_t firstIndex;

        UniqueRef<BindGroup> bindGroup;
        UniqueRef<Texture> diffuseColorMap;
        UniqueRef<TextureView> diffuseColorMapView;

        Renderable(SSAOApplication* app, Device& device, UniqueRef<Mesh>& mesh) {
            indexCount =mesh->indexCount;
            firstIndex =mesh->firstIndex;

            // upload diffuseColorMap
            auto& texData =mesh->materialData->baseColorTexture;

            BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
                .SetSize(texData->GetSize())
                .SetUsages(BufferUsageBits::uniform | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                .SetInitialState(RHI::BufferState::staging);

            UniqueRef<Buffer> pixelBuffer = app->GetDevice()->CreateBuffer(bufferCreateInfo);
            if (pixelBuffer != nullptr) {
                auto* mapData = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
                memcpy(mapData, texData->buffer.data(), bufferCreateInfo.size);
                pixelBuffer->UnMap();
            }

            diffuseColorMap = app->GetDevice()->CreateTexture(
                TextureCreateInfo()
                    .SetFormat(PixelFormat::rgba8Unorm)
                    .SetDimension(TextureDimension::t2D)
                    .SetMipLevels(1)
                    .SetExtent({texData->width, texData->height, 1})
                    .SetSamples(1)
                    .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
                    .SetInitialState(TextureState::undefined));

            diffuseColorMapView = diffuseColorMap->CreateTextureView(
                TextureViewCreateInfo()
                    .SetDimension(TextureViewDimension::tv2D)
                    .SetMipLevels(0, 1)
                    .SetArrayLayers(0, 1)
                    .SetAspect(TextureAspect::color)
                    .SetType(TextureViewType::textureBinding));

            UniqueRef<CommandBuffer> texCommandBuffer = app->GetDevice()->CreateCommandBuffer();

            UniqueRef<CommandRecorder> commandRecorder = texCommandBuffer->Begin();
            {
                UniqueRef<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
                {
                    copyRecorder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::undefined, TextureState::copyDst));
                    TextureSubResourceInfo subResourceInfo {};
                    subResourceInfo.mipLevel = 0;
                    subResourceInfo.arrayLayerNum = 1;
                    subResourceInfo.baseArrayLayer = 0;
                    subResourceInfo.aspect = TextureAspect::color;
                    copyRecorder->CopyBufferToTexture(pixelBuffer.Get(), diffuseColorMap.Get(), &subResourceInfo, {texData->width, texData->height, 1});
                    copyRecorder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
                }
                copyRecorder->EndPass();
            }
            commandRecorder->End();

            Common::UniqueRef<Fence> fence = device.CreateFence(false);
            QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fence.Get();
            app->GetQueue()->Submit(texCommandBuffer.Get(), submitInfo);
            fence->Wait();

            // per renderable bindGroup
            BindGroupCreateInfo createInfo(app->GetLayout());
            if (app->GetInstance()->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                                   , diffuseColorMapView.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                                   , app->GetSampler()));
            } else {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), diffuseColorMapView.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), app->GetSampler()));
            }

            bindGroup = app->GetDevice()->CreateBindGroup(createInfo);
        }
    };

    UniqueRef<Model> model = nullptr;
    std::vector<UniqueRef<Renderable>> renderables;

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
        FMat4x4 projection;
        FMat4x4 model;
        FMat4x4 view;
        float nearPlane = 0.1f;
        float farPlane = 64.0f;
    } uboSceneParams;

    struct UBOSSAOParams {
        FMat4x4 projection;
        int32_t ssao = 1;
        int32_t ssaoOnly = 0;
        int32_t ssaoBlur = 1;
    } ubossaoParams;

    struct Noise {
        UniqueRef<Texture> tex;
        UniqueRef<TextureView> view;
    } noise;

    struct ShaderModules {
        UniqueRef<ShaderModule> gBufferVs;
        UniqueRef<ShaderModule> gBufferPs;
        UniqueRef<ShaderModule> ssaoVs;
        UniqueRef<ShaderModule> ssaoPs;
        UniqueRef<ShaderModule> ssaoBlurVs;
        UniqueRef<ShaderModule> ssaoBlurPs;
        UniqueRef<ShaderModule> compositionVs;
        UniqueRef<ShaderModule> compositionPs;

    } shaderModules;

    struct Pipelines {
        UniqueRef<RasterPipeline> gBuffer;
        UniqueRef<RasterPipeline> ssao;
        UniqueRef<RasterPipeline> ssaoBlur;
        UniqueRef<RasterPipeline> composition;
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
        UniqueRef<TextureView> rtv;
        UniqueRef<TextureView> srv;
    };

    ColorAttachment gBufferPos;
    ColorAttachment gBufferNormal;
    ColorAttachment gBufferAlbedo;
    struct {
        UniqueRef<Texture> texture;
        UniqueRef<TextureView> view;
    } gBufferDepth;

    ColorAttachment ssaoOutput;
    ColorAttachment ssaoBlurOutput;

    struct QuadVertex {
        FVec3 pos;
        FVec2 uv;
    };

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
                .SetExtent({width, height})
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
        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(model->raw_vertex_buffer.size() * sizeof(Vertex))
            .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(RHI::BufferState::staging);

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(vertexBuffer != nullptr);
        auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_vertex_buffer.data(), bufferCreateInfo.size);
        vertexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::vertex)
            .SetSize(bufferCreateInfo.size)
            .SetOffset(0)
            .SetExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(model->raw_index_buffer.size() * sizeof(uint32_t))
            .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(RHI::BufferState::staging);

        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(indexBuffer != nullptr);
        auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_index_buffer.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::index)
            .SetSize(bufferCreateInfo.size)
            .SetOffset(0)
            .SetExtendIndex(IndexFormat::uint32);
        indexBufferView = indexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateQuadBuffer()
    {
        // quad vertex buffer
        std::vector<QuadVertex> vertices {
            {{-1.0f, -1.0f, .0f}, {0.f, 1.0f}},
            {{1.0f, -1.0f, .0f}, {1.0f, 1.0f}},
            {{1.0f, 1.0f, .0f}, {1.0f, .0f}},
            {{-1.0f, 1.0f, .0f}, {0.f, .0f}},
        };

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(vertices.size() * sizeof(QuadVertex))
            .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(RHI::BufferState::staging);

        quadVertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadVertexBuffer != nullptr) {
            auto* data = quadVertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            quadVertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::vertex)
            .SetSize(vertices.size() * sizeof(QuadVertex))
            .SetOffset(0)
            .SetExtendVertex(sizeof(QuadVertex));
        quadVertexBufferView = quadVertexBuffer->CreateBufferView(bufferViewCreateInfo);

        // index buffer
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        bufferCreateInfo = BufferCreateInfo()
            .SetSize(indices.size() * sizeof(uint32_t))
            .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(RHI::BufferState::staging);

        quadIndexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadIndexBuffer != nullptr) {
            auto* data = quadIndexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            quadIndexBuffer->UnMap();
        }

        bufferViewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::index)
            .SetSize(indices.size() * sizeof(uint32_t))
            .SetOffset(0)
            .SetExtendIndex(IndexFormat::uint32);
        quadIndexBufferView = quadIndexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateSampler()
    {
        sampler = device->CreateSampler(SamplerCreateInfo());
    }

    void CreateCommandBuffer()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            commandBuffers[i] = device->CreateCommandBuffer();
        }
    }

    void CreateSyncObjects()
    {
        for (auto i = 0; i < backBufferCount; i++) {
            backBufferReadySemaphores[i] = device->CreateSemaphore();
            renderFinishedSemaphores[i] = device->CreateSemaphore();
            inflightFences[i] = device->CreateFence(true);
        }
    }

    void CreateBindGroupLayoutAndPipelineLayout()
    {
        //gBuffer
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)),
                    ShaderStageBits::sVertex | ShaderStageBits::sPixel));
            } else {
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(0)),
                    ShaderStageBits::sVertex | ShaderStageBits::sPixel));
            }
            bindGroupLayouts.gBuffer = device->CreateBindGroupLayout(createInfo);
        }

        // renderable layout
        {
            BindGroupLayoutCreateInfo createInfo(1);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                    , ShaderStageBits::sPixel));
            } else {
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1))
                                                         , ShaderStageBits::sPixel));
            }
            renderableLayout = device->CreateBindGroupLayout(createInfo);
        }

        pipelineLayouts.gBuffer = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayouts.gBuffer.Get())
                .AddBindGroupLayout(renderableLayout.Get()));

        //ssao
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 1))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 1))
                    , ShaderStageBits::sPixel));
            } else {
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(1))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(2))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(3))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(4))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(5))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6))
                                                         , ShaderStageBits::sPixel));
            }
            bindGroupLayouts.ssao = device->CreateBindGroupLayout(createInfo);
        }

        pipelineLayouts.ssao = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayouts.ssao.Get()));

        // ssaoBlur
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                    , ShaderStageBits::sPixel));
            } else {
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1))
                                                         , ShaderStageBits::sPixel));
            }
            bindGroupLayouts.ssaoBlur = device->CreateBindGroupLayout(createInfo);
        }

        pipelineLayouts.ssaoBlur = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayouts.ssaoBlur.Get()));

        // composition
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 3))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 4))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                    , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                    , ShaderStageBits::sPixel));
            } else {
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(1))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(2))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(3))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(4))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(5))
                                                         , ShaderStageBits::sPixel));
                createInfo.AddEntry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6))
                                                         , ShaderStageBits::sPixel));
            }
            bindGroupLayouts.composition = device->CreateBindGroupLayout(createInfo);
        }

        pipelineLayouts.composition = device->CreatePipelineLayout(
            PipelineLayoutCreateInfo()
                .AddBindGroupLayout(bindGroupLayouts.composition.Get()));
    }

    void CreateBindGroup()
    {
        // GBuffer scene
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.gBuffer.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(BindGroupEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                    , uniformBuffers.sceneParams.bufView.Get()));
            } else {
                createInfo.AddEntry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(0))
                                                   , uniformBuffers.sceneParams.bufView.Get()));
            }
            bindGroups.scene = device->CreateBindGroup(createInfo);
        }

        // ssao generation
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.ssao.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                                   , gBufferPos.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1))
                                   , gBufferNormal.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2))
                                   , noise.view.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                                   , sampler.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 1))
                                   , noiseSampler.Get()));
                createInfo.AddEntry(BindGroupEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                    , uniformBuffers.ssaoKernel.bufView.Get()));
                createInfo.AddEntry(BindGroupEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 1))
                    , uniformBuffers.ssaoParams.bufView.Get()));
            } else {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), gBufferPos.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), gBufferNormal.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), noise.view.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(3)), sampler.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(4)), noiseSampler.Get()));
                createInfo.AddEntry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(5))
                                                   , uniformBuffers.ssaoKernel.bufView.Get()));
                createInfo.AddEntry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6))
                                                   , uniformBuffers.ssaoParams.bufView.Get()));
            }
            bindGroups.ssao = device->CreateBindGroup(createInfo);
        }

        // ssao blur
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.ssaoBlur.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                                   , ssaoOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                                   , sampler.Get()));
            } else {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ssaoOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), sampler.Get()));
            }
            bindGroups.ssaoBlur = device->CreateBindGroup(createInfo);
        }

        // composition
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.composition.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0))
                                   , gBufferPos.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1))
                                   , gBufferNormal.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2))
                                   , gBufferAlbedo.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 3))
                                   , ssaoOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 4))
                                   , ssaoBlurOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0))
                                   , sampler.Get()));
                createInfo.AddEntry(BindGroupEntry(
                    ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0))
                    , uniformBuffers.ssaoParams.bufView.Get()));
            } else {
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), gBufferPos.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), gBufferNormal.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), gBufferAlbedo.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(3)), ssaoOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(4)), ssaoBlurOutput.srv.Get()));
                createInfo.AddEntry(
                    BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(5)), sampler.Get()));
                createInfo.AddEntry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6))
                                                   , uniformBuffers.ssaoParams.bufView.Get()));
            }
            bindGroups.composition = device->CreateBindGroup(createInfo);
        }
    }

    void PrepareOffscreen()
    {
        CreateAttachments(PixelFormat::rgba32Float, gBufferPos);
        CreateAttachments(PixelFormat::rgba8Unorm, gBufferNormal);
        CreateAttachments(PixelFormat::rgba8Unorm, gBufferAlbedo);
        CreateAttachments(PixelFormat::r8Unorm, ssaoOutput);
        CreateAttachments(PixelFormat::r8Unorm, ssaoBlurOutput);
        CreateDepthAttachment();
    }

    void PrepareUniformBuffers()
    {
        // gltf model axis: y up, x right, z from screen inner to outer
        // to transform gltf coords system to our local coords system: z up, y right, x from screen outer to inner
        FMat4x4 aixsTransMat = FMat4x4 {
            0, 0, -1, 0,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 1
        };

        // scene matries
        uboSceneParams.projection = camera->GetProjectionMatrix();
        uboSceneParams.view = camera->GetViewMatrix();
        uboSceneParams.model = aixsTransMat;

        CreateUniformBuffer(BufferUsageBits::uniform | BufferUsageBits::mapWrite, &uniformBuffers.sceneParams, sizeof(UBOSceneParams), &uboSceneParams);

        // ssao parameters
        ubossaoParams.projection = camera->GetProjectionMatrix();
        CreateUniformBuffer(BufferUsageBits::uniform | BufferUsageBits::mapWrite, &uniformBuffers.ssaoParams, sizeof(UBOSSAOParams), &ubossaoParams);

        // ssao kennel
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
        std::vector<FVec4> ssaoKernel(ssaoKernelSize);

        auto lerp = [](float a, float b, float f) ->float {
            return a + f * (b - a);
        };

        for (uint32_t i = 0; i < ssaoKernelSize; ++i)
        {
            FVec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
            sample.Normalize();
            sample *= rndDist(rndEngine);
            float scale = float(i) / float(ssaoKernelSize);
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample = sample * scale;
            ssaoKernel[i] = FVec4(sample.x, sample.y, sample.z, 0.0f);
        }
        CreateUniformBuffer(BufferUsageBits::uniform | BufferUsageBits::mapWrite, &uniformBuffers.ssaoKernel, ssaoKernel.size() * sizeof(FVec4), ssaoKernel.data());

        // random noise
        std::vector<FVec4> ssaoNoise(ssaoNoiseDim * ssaoNoiseDim);
        for (auto& randomVec : ssaoNoise)
        {
            randomVec = FVec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
        }

        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .SetSize(ssaoNoise.size() * sizeof(FVec4))
            .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .SetInitialState(RHI::BufferState::staging);

        UniqueRef<Buffer> pixelBuffer = device->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, ssaoNoise.data(), bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }

        noise.tex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba32Float)
                .SetMipLevels(1)
                .SetExtent({ssaoNoiseDim, ssaoNoiseDim, 1})
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
                .SetInitialState(TextureState::undefined));

        noise.view = noise.tex->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::color)
                .SetType(TextureViewType::textureBinding));

        noiseSampler = device->CreateSampler(
            SamplerCreateInfo()
                .SetAddressModeU(AddressMode::repeat)
                .SetAddressModeV(AddressMode::repeat));

        UniqueRef<CommandBuffer> texCommandBuffer = device->CreateCommandBuffer();

        UniqueRef<CommandRecorder> commandRecorder = texCommandBuffer->Begin();
        {
            UniqueRef<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
            {
                copyRecorder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::undefined, TextureState::copyDst));
                TextureSubResourceInfo subResourceInfo {};
                subResourceInfo.mipLevel = 0;
                subResourceInfo.arrayLayerNum = 1;
                subResourceInfo.baseArrayLayer = 0;
                subResourceInfo.aspect = TextureAspect::color;
                copyRecorder->CopyBufferToTexture(pixelBuffer.Get(), noise.tex.Get(), &subResourceInfo, {ssaoNoiseDim, ssaoNoiseDim, 1});
                copyRecorder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            }
            copyRecorder->EndPass();
        }
        commandRecorder->End();

        Common::UniqueRef<Fence> fence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        graphicsQueue->Submit(texCommandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    void CreateDepthAttachment() {
        gBufferDepth.texture = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::d32Float)
                .SetMipLevels(1)
                .SetExtent({width, height, 1})
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::depthStencilAttachment)
                .SetInitialState(TextureState::depthStencilReadonly));

        gBufferDepth.view = gBufferDepth.texture->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::depth)
                .SetType(TextureViewType::depthStencil));
    }

    void CreateAttachments(RHI::PixelFormat format, ColorAttachment& attachment)
    {
        attachment.texture = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(format)
                .SetMipLevels(1)
                .SetExtent({width, height, 1})
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));

        attachment.rtv = attachment.texture->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::color)
                .SetType(TextureViewType::colorAttachment));

        attachment.srv = attachment.texture->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::color)
                .SetType(TextureViewType::textureBinding));
    }

    ShaderModule* GetShaderModule(std::vector<uint8_t>& byteCode, const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        std::vector<std::string> includePath { "../Test/Sample/SSAO/Shader"};
        CompileShader(byteCode, fileName, entryPoint, shaderStage, includePath);
        return device->CreateShaderModule(ShaderModuleCreateInfo(byteCode));
    }

    void CreateUniformBuffer(RHI::BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data)
    {
        BufferCreateInfo createInfo = BufferCreateInfo()
            .SetSize(size)
            .SetUsages(flags)
            .SetInitialState(RHI::BufferState::staging);

        uBuffer->buf = device->CreateBuffer(createInfo);
        if (uBuffer->buf != nullptr && data != nullptr) {
            auto* mapData = uBuffer->buf->Map(MapMode::write, 0, size);
            memcpy(mapData, data, size);
            uBuffer->buf->UnMap();
        }

        BufferViewCreateInfo viewCreateInfo = BufferViewCreateInfo()
            .SetType(BufferViewType::uniformBinding)
            .SetSize(size)
            .SetOffset(0);
        uBuffer->bufView = uBuffer->buf->CreateBufferView(viewCreateInfo);
    }

    void CreatePipeline()
    {
        std::vector<uint8_t> gBufferVs;
        std::vector<uint8_t> gBufferPs;
        std::vector<uint8_t> ssaoVs;
        std::vector<uint8_t> ssaoPs;
        std::vector<uint8_t> blurVs;
        std::vector<uint8_t> blurPs;
        std::vector<uint8_t> compositionVs;
        std::vector<uint8_t> compositionPs;

        shaderModules.gBufferVs     = GetShaderModule(gBufferVs, "../Test/Sample/SSAO/Shader/Gbuffer.hlsl", "VSMain", ShaderStageBits::sVertex);
        shaderModules.gBufferPs     = GetShaderModule(gBufferPs, "../Test/Sample/SSAO/Shader/Gbuffer.hlsl", "FSMain", ShaderStageBits::sPixel);
        shaderModules.ssaoVs        = GetShaderModule(ssaoVs, "../Test/Sample/SSAO/Shader/SSAO.hlsl", "VSMain", ShaderStageBits::sVertex);
        shaderModules.ssaoPs        = GetShaderModule(ssaoPs, "../Test/Sample/SSAO/Shader/SSAO.hlsl", "FSMain", ShaderStageBits::sPixel);
        shaderModules.ssaoBlurVs    = GetShaderModule(blurVs, "../Test/Sample/SSAO/Shader/Blur.hlsl", "VSMain", ShaderStageBits::sVertex);
        shaderModules.ssaoBlurPs    = GetShaderModule(blurPs, "../Test/Sample/SSAO/Shader/Blur.hlsl", "FSMain", ShaderStageBits::sPixel);
        shaderModules.compositionVs = GetShaderModule(compositionVs, "../Test/Sample/SSAO/Shader/Composition.hlsl", "VSMain", ShaderStageBits::sVertex);
        shaderModules.compositionPs = GetShaderModule(compositionPs, "../Test/Sample/SSAO/Shader/Composition.hlsl", "FSMain", ShaderStageBits::sPixel);

        // Gbuffer vertex
        VertexBufferLayout vertexBufferLayout = VertexBufferLayout()
            .SetStepMode(VertexStepMode::perVertex)
            .SetStride(sizeof(Vertex))
            .AddAttribute(VertexAttribute("POSITION", 0, VertexFormat::float32X4, 0))
            .AddAttribute(VertexAttribute("TEXCOORD", 0, VertexFormat::float32X2, offsetof(Vertex, uv)))
            .AddAttribute(VertexAttribute("COLOR", 0, VertexFormat::float32X4, offsetof(Vertex, color)))
            .AddAttribute(VertexAttribute("NORMAL", 0, VertexFormat::float32X3, offsetof(Vertex, normal)));

        // quad buffer vertex
        VertexBufferLayout quadVertexBufferLayout = VertexBufferLayout()
            .SetStepMode(VertexStepMode::perVertex)
            .SetStride(sizeof(QuadVertex))
            .AddAttribute(VertexAttribute("POSITION", 0, VertexFormat::float32X3, 0))
            .AddAttribute(VertexAttribute("TEXCOORD", 0, VertexFormat::float32X2, offsetof(QuadVertex, uv)));

        // General pipeline infos
        RasterPipelineCreateInfo createInfo = RasterPipelineCreateInfo()
            .SetPrimitiveState(
                PrimitiveState()
                    .SetDepthClip(false)
                    .SetFrontFace(FrontFace::ccw)
                    .SetCullMode(CullMode::none)
                    .SetTopologyType(PrimitiveTopologyType::triangle)
                    .SetStripIndexFormat(IndexFormat::uint32));

        // Gbuffer
        {
            createInfo
                .SetDepthStencilState(
                    DepthStencilState()
                        .SetDepthEnabled(true)
                        .SetDepthComparisonFunc(ComparisonFunc::greaterEqual)
                        .SetFormat(PixelFormat::d32Float))
                .SetVertexState(
                    VertexState()
                        .AddVertexBufferLayout(vertexBufferLayout))
                .SetFragmentState(
                    FragmentState()
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(PixelFormat::rgba32Float)
                                .SetWriteFlags(ColorWriteBits::all))
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(PixelFormat::rgba8Unorm)
                                .SetWriteFlags(ColorWriteBits::all))
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(PixelFormat::rgba8Unorm)
                                .SetWriteFlags(ColorWriteBits::all)))
                .SetVertexShader(shaderModules.gBufferVs.Get())
                .SetPixelShader(shaderModules.gBufferPs.Get())
                .SetLayout(pipelineLayouts.gBuffer.Get());

            pipelines.gBuffer = device->CreateRasterPipeline(createInfo);
        }

        // ssao
        {
            createInfo
                .SetDepthStencilState(DepthStencilState())
                .SetVertexState(
                    VertexState()
                        .AddVertexBufferLayout(quadVertexBufferLayout))
                .SetFragmentState(
                    FragmentState()
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(PixelFormat::r8Unorm)
                                .SetWriteFlags(ColorWriteBits::all)))
                .SetVertexShader(shaderModules.ssaoVs.Get())
                .SetPixelShader(shaderModules.ssaoPs.Get())
                .SetLayout(pipelineLayouts.ssao.Get());

            pipelines.ssao = device->CreateRasterPipeline(createInfo);
        }

        // ssaoBlur
        {
            createInfo
                .SetDepthStencilState(DepthStencilState())
                .SetVertexState(
                    VertexState()
                        .AddVertexBufferLayout(quadVertexBufferLayout))
                .SetFragmentState(
                    FragmentState()
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(PixelFormat::r8Unorm)
                                .SetWriteFlags(ColorWriteBits::all)))
                .SetVertexShader(shaderModules.ssaoBlurVs.Get())
                .SetPixelShader(shaderModules.ssaoBlurPs.Get())
                .SetLayout(pipelineLayouts.ssaoBlur.Get());

            pipelines.ssaoBlur = device->CreateRasterPipeline(createInfo);
        }

        // composition
        {
            createInfo
                .SetDepthStencilState(DepthStencilState())
                .SetVertexState(
                    VertexState()
                        .AddVertexBufferLayout(quadVertexBufferLayout))
                .SetFragmentState(
                    FragmentState()
                        .AddColorTarget(
                            ColorTargetState()
                                .SetFormat(swapChainFormat)
                                .SetWriteFlags(ColorWriteBits::all)))
                .SetVertexShader(shaderModules.compositionVs.Get())
                .SetPixelShader(shaderModules.compositionPs.Get())
                .SetLayout(pipelineLayouts.composition.Get());

            pipelines.composition = device->CreateRasterPipeline(createInfo);
        }
    }

    void InitCamera()
    {
        camera = Common::MakeUnique<Camera>(
            FVec3(.0f, -5.0f, 2.0f),
            FVec3(.0f, .0f, -90.0f),
            Camera::ProjectParams {
                60.0f,
                static_cast<float>(width),
                static_cast<float>(height),
                0.1f,
                64.0f
            }
        );

        camera->moveSpeed = 0.005f;
        camera->rotateSpeed = 0.1f;
    }

    void LoadGLTF()
    {
        model = Common::MakeUnique<Model>();
        model->LoadFromFile("../Test/Sample/SSAO/Model/Voyager.gltf");
    }

    void GenerateRenderables()
    {
        for (auto& mesh : model->meshes) {
            renderables.emplace_back(Common::MakeUnique<Renderable>(this, *device, mesh));
        }
    }
};

int main(int argc, char* argv[])
{
    SSAOApplication application("SSAO");
    return application.Run(argc, argv);
}

