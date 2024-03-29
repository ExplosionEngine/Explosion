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

        UniqueRef<CommandEncoder> commandEncoder = commandBuffers[nextFrameIndex]->Begin();
        {
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferDepth.texture.Get(), TextureState::depthStencilReadonly, TextureState::depthStencilWrite));

            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(
                GraphicsPassBeginInfo()
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(gBufferPos.rtv.Get()))
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(gBufferNormal.rtv.Get()))
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(gBufferAlbedo.rtv.Get()))
                    .DepthStencilAttachment(
                        GraphicsPassDepthStencilAttachment()
                            .DepthClearValue(0.0f)
                            .DepthLoadOp(LoadOp::clear)
                            .DepthStoreOp(StoreOp::store)
                            .DepthReadOnly(true)
                            .StencilClearValue(0.0f)
                            .View(gBufferDepth.view.Get())));
            {
                graphicsEncoder->SetPipeline(pipelines.gBuffer.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetBindGroup(0, bindGroups.scene.Get());
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(indexBufferView.Get());

                for (auto& renderable : renderables) {
                    graphicsEncoder->SetBindGroup(1, renderable->bindGroup.Get());
                    graphicsEncoder->DrawIndexed(renderable->indexCount, 1, renderable->firstIndex, 0, 0);
                }
            }
            graphicsEncoder->EndPass();

            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferDepth.texture.Get(), TextureState::depthStencilWrite, TextureState::depthStencilReadonly));
        }

        {
            // ssao
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));

            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(
                GraphicsPassBeginInfo()
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(ssaoOutput.rtv.Get())));
            {
                graphicsEncoder->SetPipeline(pipelines.ssao.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetBindGroup(0, bindGroups.ssao.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
        }

        {
            // ssaoBlur
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::shaderReadOnly, TextureState::renderTarget));

            UniqueRef<GraphicsPassCommandEncoder> graphicsEncoder = commandEncoder->BeginGraphicsPass(
                GraphicsPassBeginInfo()
                    .ColorAttachment(
                        GraphicsPassColorAttachment()
                            .ClearValue(Common::ColorConsts::black.ToLinearColor())
                            .LoadOp(LoadOp::clear)
                            .StoreOp(StoreOp::store)
                            .View(ssaoBlurOutput.rtv.Get())));
            {
                graphicsEncoder->SetPipeline(pipelines.ssaoBlur.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetBindGroup(0, bindGroups.ssaoBlur.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::renderTarget, TextureState::shaderReadOnly));
        }

        {
            // composition
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
                graphicsEncoder->SetPipeline(pipelines.composition.Get());
                graphicsEncoder->SetScissor(0, 0, width, height);
                graphicsEncoder->SetViewport(0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::triangleList);
                graphicsEncoder->SetBindGroup(0, bindGroups.composition.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::renderTarget, TextureState::present));
        }

        commandEncoder->End();

        QueueSubmitInfo submitInfo {};
        submitInfo.waitSemaphoreNum = 1;
        submitInfo.waitSemaphores = backBufferReadySemaphores[nextFrameIndex].Get();
        submitInfo.signalSemaphoreNum = 1;
        submitInfo.signalSemaphores = renderFinishedSemaphores[nextFrameIndex].Get();
        submitInfo.signalFence = inflightFences[nextFrameIndex].Get();
        graphicsQueue->Submit(commandBuffers[nextFrameIndex].Get(), submitInfo);

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
                .Size(texData->GetSize())
                .Usages(BufferUsageBits::uniform | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                .InitialState(RHI::BufferState::staging);

            UniqueRef<Buffer> pixelBuffer = app->GetDevice()->CreateBuffer(bufferCreateInfo);
            if (pixelBuffer != nullptr) {
                auto* mapData = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
                memcpy(mapData, texData->buffer.data(), bufferCreateInfo.size);
                pixelBuffer->UnMap();
            }

            TextureCreateInfo texCreateInfo {};
            texCreateInfo.format = PixelFormat::rgba8Unorm;
            texCreateInfo.dimension = TextureDimension::t2D;
            texCreateInfo.mipLevels = 1;
            texCreateInfo.extent = { texData->width, texData->height, 1};
            texCreateInfo.samples = 1;
            texCreateInfo.usages = TextureUsageBits::copyDst | TextureUsageBits::textureBinding;
            diffuseColorMap = app->GetDevice()->CreateTexture(texCreateInfo);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.dimension = TextureViewDimension::tv2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::color;
            viewCreateInfo.type = TextureViewType::textureBinding;
            diffuseColorMapView = diffuseColorMap->CreateTextureView(viewCreateInfo);

            UniqueRef<CommandBuffer> texCommandBuffer = app->GetDevice()->CreateCommandBuffer();

            UniqueRef<CommandEncoder> commandEncoder = texCommandBuffer->Begin();
            {
                UniqueRef<CopyPassCommandEncoder> copyPassEncoder = commandEncoder->BeginCopyPass();
                {
                    copyPassEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::undefined, TextureState::copyDst));
                    TextureSubResourceInfo subResourceInfo {};
                    subResourceInfo.mipLevel = 0;
                    subResourceInfo.arrayLayerNum = 1;
                    subResourceInfo.baseArrayLayer = 0;
                    subResourceInfo.aspect = TextureAspect::color;
                    copyPassEncoder->CopyBufferToTexture(pixelBuffer.Get(), diffuseColorMap.Get(), &subResourceInfo, {texData->width, texData->height, 1});
                    copyPassEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
                }
                copyPassEncoder->EndPass();
            }
            commandEncoder->End();

            Common::UniqueRef<Fence> fence = device.CreateFence(false);
            QueueSubmitInfo submitInfo {};
            submitInfo.signalFence = fence.Get();
            app->GetQueue()->Submit(texCommandBuffer.Get(), submitInfo);
            fence->Wait();

            // per renderable bindGroup
            BindGroupCreateInfo createInfo(app->GetLayout());
            if (app->GetInstance()->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), diffuseColorMapView.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), app->GetSampler()));
            } else {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), diffuseColorMapView.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), app->GetSampler()));
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
                .Queue(QueueRequestInfo(QueueType::graphics, 1)));
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
        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(model->raw_vertex_buffer.size() * sizeof(Vertex))
            .Usages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(RHI::BufferState::staging);

        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(vertexBuffer != nullptr);
        auto* data = vertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_vertex_buffer.data(), bufferCreateInfo.size);
        vertexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::vertex)
            .Size(bufferCreateInfo.size)
            .Offset(0)
            .ExtendVertex(sizeof(Vertex));
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateIndexBuffer()
    {
        BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
            .Size(model->raw_index_buffer.size() * sizeof(uint32_t))
            .Usages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(RHI::BufferState::staging);

        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(indexBuffer != nullptr);
        auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_index_buffer.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::index)
            .Size(bufferCreateInfo.size)
            .Offset(0)
            .ExtendIndex(IndexFormat::uint32);
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
            .Size(vertices.size() * sizeof(QuadVertex))
            .Usages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(RHI::BufferState::staging);

        quadVertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadVertexBuffer != nullptr) {
            auto* data = quadVertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            quadVertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::vertex)
            .Size(vertices.size() * sizeof(QuadVertex))
            .Offset(0)
            .ExtendVertex(sizeof(QuadVertex));
        quadVertexBufferView = quadVertexBuffer->CreateBufferView(bufferViewCreateInfo);

        // index buffer
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        bufferCreateInfo = BufferCreateInfo()
            .Size(indices.size() * sizeof(uint32_t))
            .Usages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(RHI::BufferState::staging);

        quadIndexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadIndexBuffer != nullptr) {
            auto* data = quadIndexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            quadIndexBuffer->UnMap();
        }

        bufferViewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::index)
            .Size(indices.size() * sizeof(uint32_t))
            .Offset(0)
            .ExtendIndex(IndexFormat::uint32);
        quadIndexBufferView = quadIndexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void CreateSampler()
    {
        SamplerCreateInfo samplerCreateInfo {};
        sampler = device->CreateSampler(samplerCreateInfo);
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
        PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};

        //gBuffer
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), ShaderStageBits::sVertex | ShaderStageBits::sPixel));
            } else {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(0)), ShaderStageBits::sVertex | ShaderStageBits::sPixel));
            }
            bindGroupLayouts.gBuffer = device->CreateBindGroupLayout(createInfo);
        }

        // renderable layout
        {
            BindGroupLayoutCreateInfo createInfo(1);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), ShaderStageBits::sPixel));
            } else {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), ShaderStageBits::sPixel));
            }
            renderableLayout = device->CreateBindGroupLayout(createInfo);
        }

        std::vector<BindGroupLayout*> gBufferLayouts { bindGroupLayouts.gBuffer.Get(), renderableLayout.Get() };
        pipelineLayoutCreateInfo.bindGroupLayoutNum = 2;
        pipelineLayoutCreateInfo.bindGroupLayouts = gBufferLayouts.data();
        pipelineLayouts.gBuffer = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        //ssao
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 1)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 1)), ShaderStageBits::sPixel));
            } else {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(3)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(4)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(5)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6)), ShaderStageBits::sPixel));
            }
            bindGroupLayouts.ssao = device->CreateBindGroupLayout(createInfo);
        }

        std::vector<BindGroupLayout*> ssaoLayouts { bindGroupLayouts.ssao.Get() };
        pipelineLayoutCreateInfo.bindGroupLayoutNum = 1;
        pipelineLayoutCreateInfo.bindGroupLayouts = ssaoLayouts.data();
        pipelineLayouts.ssao = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        // ssaoBlur
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), ShaderStageBits::sPixel));
            } else {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), ShaderStageBits::sPixel));
            }
            bindGroupLayouts.ssaoBlur = device->CreateBindGroupLayout(createInfo);
        }

        std::vector<BindGroupLayout*> blurLayouts { bindGroupLayouts.ssaoBlur.Get() };
        pipelineLayoutCreateInfo.bindGroupLayouts = blurLayouts.data();
        pipelineLayouts.ssaoBlur = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        // composition
        {
            BindGroupLayoutCreateInfo createInfo(0);
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 3)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 4)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), ShaderStageBits::sPixel));
            } else {
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(3)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::texture, GlslBinding(4)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::sampler, GlslBinding(5)), ShaderStageBits::sPixel));
                createInfo.Entry(BindGroupLayoutEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6)), ShaderStageBits::sPixel));
            }
            bindGroupLayouts.composition = device->CreateBindGroupLayout(createInfo);
        }

        std::vector<BindGroupLayout*> comLayouts { bindGroupLayouts.composition.Get() };
        pipelineLayoutCreateInfo.bindGroupLayouts = comLayouts.data();
        pipelineLayouts.composition = device->CreatePipelineLayout(pipelineLayoutCreateInfo);
    }

    void CreateBindGroup()
    {
        // GBuffer scene
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.gBuffer.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), uniformBuffers.sceneParams.bufView.Get()));
            } else {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(0)), uniformBuffers.sceneParams.bufView.Get()));
            }
            bindGroups.scene = device->CreateBindGroup(createInfo);
        }

        // ssao generation
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.ssao.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), gBufferPos.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1)), gBufferNormal.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2)), noise.view.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), sampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 1)), noiseSampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), uniformBuffers.ssaoKernel.bufView.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 1)), uniformBuffers.ssaoParams.bufView.Get()));
            } else {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), gBufferPos.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), gBufferNormal.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), noise.view.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(3)), sampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(4)), noiseSampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(5)), uniformBuffers.ssaoKernel.bufView.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6)), uniformBuffers.ssaoParams.bufView.Get()));
            }
            bindGroups.ssao = device->CreateBindGroup(createInfo);
        }

        // ssao blur
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.ssaoBlur.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), ssaoOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), sampler.Get()));
            } else {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), ssaoOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(1)), sampler.Get()));
            }
            bindGroups.ssaoBlur = device->CreateBindGroup(createInfo);
        }

        // composition
        {
            BindGroupCreateInfo createInfo(bindGroupLayouts.composition.Get());
            if (instance->GetRHIType() == RHI::RHIType::directX12) {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 0)), gBufferPos.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 1)), gBufferNormal.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 2)), gBufferAlbedo.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 3)), ssaoOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, HlslBinding(HlslBindingRangeType::texture, 4)), ssaoBlurOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, HlslBinding(HlslBindingRangeType::sampler, 0)), sampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, HlslBinding(HlslBindingRangeType::constantBuffer, 0)), uniformBuffers.ssaoParams.bufView.Get()));
            } else {
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(0)), gBufferPos.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(1)), gBufferNormal.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(2)), gBufferAlbedo.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(3)), ssaoOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::texture, GlslBinding(4)), ssaoBlurOutput.srv.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::sampler, GlslBinding(5)), sampler.Get()));
                createInfo.Entry(BindGroupEntry(ResourceBinding(BindingType::uniformBuffer, GlslBinding(6)), uniformBuffers.ssaoParams.bufView.Get()));
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
            .Size(ssaoNoise.size() * sizeof(FVec4))
            .Usages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
            .InitialState(RHI::BufferState::staging);

        UniqueRef<Buffer> pixelBuffer = device->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
            memcpy(data, ssaoNoise.data(), bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::rgba32Float;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {ssaoNoiseDim, ssaoNoiseDim, 1};
        texCreateInfo.dimension = TextureDimension::t2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::copyDst | TextureUsageBits::textureBinding;
        noise.tex = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::tv2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::color;
        viewCreateInfo.type = TextureViewType::textureBinding;
        noise.view = noise.tex->CreateTextureView(viewCreateInfo);

        SamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.addressModeU = AddressMode::repeat;
        samplerCreateInfo.addressModeV = AddressMode::repeat;
        noiseSampler = device->CreateSampler(samplerCreateInfo);

        UniqueRef<CommandBuffer> texCommandBuffer = device->CreateCommandBuffer();

        UniqueRef<CommandEncoder> commandEncoder = texCommandBuffer->Begin();
        {
            UniqueRef<CopyPassCommandEncoder> copyPassEncoder = commandEncoder->BeginCopyPass();
            {
                copyPassEncoder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::undefined, TextureState::copyDst));
                TextureSubResourceInfo subResourceInfo {};
                subResourceInfo.mipLevel = 0;
                subResourceInfo.arrayLayerNum = 1;
                subResourceInfo.baseArrayLayer = 0;
                subResourceInfo.aspect = TextureAspect::color;
                copyPassEncoder->CopyBufferToTexture(pixelBuffer.Get(), noise.tex.Get(), &subResourceInfo, {ssaoNoiseDim, ssaoNoiseDim, 1});
                copyPassEncoder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            }
            copyPassEncoder->EndPass();
        }
        commandEncoder->End();

        Common::UniqueRef<Fence> fence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        graphicsQueue->Submit(texCommandBuffer.Get(), submitInfo);
        fence->Wait();
    }

    void CreateDepthAttachment() {
        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::d32Float;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {width, height, 1};
        texCreateInfo.dimension = TextureDimension::t2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::depthStencilAttachment;
        texCreateInfo.initialState = TextureState::depthStencilReadonly;
        gBufferDepth.texture = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::tv2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::depth;
        viewCreateInfo.type = TextureViewType::depthStencil;
        gBufferDepth.view = gBufferDepth.texture->CreateTextureView(viewCreateInfo);
    }

    void CreateAttachments(RHI::PixelFormat format, ColorAttachment& attachment)
    {
        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = format;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {width, height, 1};
        texCreateInfo.dimension = TextureDimension::t2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment;
        texCreateInfo.initialState = TextureState::shaderReadOnly;
        attachment.texture = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo rtvCreateInfo {};
        rtvCreateInfo.dimension = TextureViewDimension::tv2D;
        rtvCreateInfo.baseArrayLayer = 0;
        rtvCreateInfo.arrayLayerNum = 1;
        rtvCreateInfo.baseMipLevel = 0;
        rtvCreateInfo.mipLevelNum = 1;
        rtvCreateInfo.aspect = TextureAspect::color;
        rtvCreateInfo.type = TextureViewType::colorAttachment;
        attachment.rtv = attachment.texture->CreateTextureView(rtvCreateInfo);

        TextureViewCreateInfo srvCreateInfo {};
        srvCreateInfo.dimension = TextureViewDimension::tv2D;
        srvCreateInfo.baseArrayLayer = 0;
        srvCreateInfo.arrayLayerNum = 1;
        srvCreateInfo.baseMipLevel = 0;
        srvCreateInfo.mipLevelNum = 1;
        srvCreateInfo.aspect = TextureAspect::color;
        srvCreateInfo.type = TextureViewType::textureBinding;

        attachment.srv = attachment.texture->CreateTextureView(srvCreateInfo);
    }

    ShaderModule* GetShaderModule(std::vector<uint8_t>& byteCode, const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        std::vector<std::string> includePath { "../Test/Sample/SSAO/Shader"};

        CompileShader(byteCode, fileName, entryPoint, shaderStage, includePath);

        ShaderModuleCreateInfo createInfo {};
        createInfo.size = byteCode.size();
        createInfo.byteCode = byteCode.data();

        return device->CreateShaderModule(createInfo);
    }

    void CreateUniformBuffer(RHI::BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data)
    {
        BufferCreateInfo createInfo = BufferCreateInfo()
            .Size(size)
            .Usages(flags)
            .InitialState(RHI::BufferState::staging);

        uBuffer->buf = device->CreateBuffer(createInfo);
        if (uBuffer->buf != nullptr && data != nullptr) {
            auto* mapData = uBuffer->buf->Map(MapMode::write, 0, size);
            memcpy(mapData, data, size);
            uBuffer->buf->UnMap();
        }

        BufferViewCreateInfo viewCreateInfo = BufferViewCreateInfo()
            .Type(BufferViewType::uniformBinding)
            .Size(size)
            .Offset(0);
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
            .StepMode(VertexStepMode::perVertex)
            .Stride(sizeof(Vertex))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X4)
                    .Offset(0)
                    .SemanticName("POSITION")
                    .SemanticIndex(0))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X2)
                    .Offset(offsetof(Vertex, uv))
                    .SemanticName("TEXCOORD")
                    .SemanticIndex(0))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X4)
                    .Offset(offsetof(Vertex, color))
                    .SemanticName("COLOR")
                    .SemanticIndex(0))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X3)
                    .Offset(offsetof(Vertex, normal))
                    .SemanticName("NORMAL")
                    .SemanticIndex(0));

        // quad buffer vertex
        VertexBufferLayout quadVertexBufferLayout = VertexBufferLayout()
            .StepMode(VertexStepMode::perVertex)
            .Stride(sizeof(QuadVertex))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X3)
                    .Offset(0)
                    .SemanticName("POSITION")
                    .SemanticIndex(0))
            .Attribute(
                VertexAttribute()
                    .Format(VertexFormat::float32X2)
                    .Offset(offsetof(QuadVertex, uv))
                    .SemanticName("TEXCOORD")
                    .SemanticIndex(0));

        // General pipeline infos
        GraphicsPipelineCreateInfo createInfo = GraphicsPipelineCreateInfo()
            .PrimitiveState(
                PrimitiveState()
                    .DepthClip(false)
                    .FrontFace(FrontFace::ccw)
                    .CullMode(CullMode::none)
                    .TopologyType(PrimitiveTopologyType::triangle)
                    .StripIndexFormat(IndexFormat::uint32))
            .MultiSampleState(
                MultiSampleState()
                    .Count(1));

        // Gbuffer
        {
            createInfo
                .DepthStencilState(
                    DepthStencilState()
                        .DepthEnabled(true)
                        .DepthComparisonFunc(ComparisonFunc::greaterEqual)
                        .Format(PixelFormat::d32Float))
                .VertexState(
                    VertexState()
                        .VertexBufferLayout(vertexBufferLayout))
                .FragmentState(
                    FragmentState()
                        .ColorTarget(
                            ColorTargetState()
                                .Format(PixelFormat::rgba32Float)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha))
                        .ColorTarget(
                            ColorTargetState()
                                .Format(PixelFormat::rgba8Unorm)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha))
                        .ColorTarget(
                            ColorTargetState()
                                .Format(PixelFormat::rgba8Unorm)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
                .VertexShader(shaderModules.gBufferVs.Get())
                .PixelShader(shaderModules.gBufferPs.Get())
                .Layout(pipelineLayouts.gBuffer.Get());

            pipelines.gBuffer = device->CreateGraphicsPipeline(createInfo);
        }

        // ssao
        {
            createInfo
                .DepthStencilState(DepthStencilState())
                .VertexState(
                    VertexState()
                        .VertexBufferLayout(quadVertexBufferLayout))
                .FragmentState(
                    FragmentState()
                        .ColorTarget(
                            ColorTargetState()
                                .Format(PixelFormat::r8Unorm)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
                .VertexShader(shaderModules.ssaoVs.Get())
                .PixelShader(shaderModules.ssaoPs.Get())
                .Layout(pipelineLayouts.ssao.Get());

            pipelines.ssao = device->CreateGraphicsPipeline(createInfo);
        }

        // ssaoBlur
        {
            createInfo
                .DepthStencilState(DepthStencilState())
                .VertexState(
                    VertexState()
                        .VertexBufferLayout(quadVertexBufferLayout))
                .FragmentState(
                    FragmentState()
                        .ColorTarget(
                            ColorTargetState()
                                .Format(PixelFormat::r8Unorm)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
                .VertexShader(shaderModules.ssaoBlurVs.Get())
                .PixelShader(shaderModules.ssaoBlurPs.Get())
                .Layout(pipelineLayouts.ssaoBlur.Get());

            pipelines.ssaoBlur = device->CreateGraphicsPipeline(createInfo);
        }

        // composition
        {
            createInfo
                .DepthStencilState(DepthStencilState())
                .VertexState(
                    VertexState()
                        .VertexBufferLayout(quadVertexBufferLayout))
                .FragmentState(
                    FragmentState()
                        .ColorTarget(
                            ColorTargetState()
                                .Format(swapChainFormat)
                                .WriteFlags(ColorWriteBits::red | ColorWriteBits::green | ColorWriteBits::blue | ColorWriteBits::alpha)))
                .VertexShader(shaderModules.compositionVs.Get())
                .PixelShader(shaderModules.compositionPs.Get())
                .Layout(pipelineLayouts.composition.Get());

            pipelines.composition = device->CreateGraphicsPipeline(createInfo);
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

