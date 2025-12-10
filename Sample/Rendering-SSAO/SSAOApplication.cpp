#include <vector>
#include <array>
#include <random>
#include <GLTFParser.h>
#include <Application.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Render/RenderGraph.h>
#include <Render/RenderThread.h>

using namespace Common;
using namespace Render;
using namespace RHI;

struct QuadVertex {
    FVec3 pos;
    FVec2 uv;
};

struct UBOSceneParams {
    FMat4x4 projection;
    FMat4x4 model;
    FMat4x4 view;
    float nearPlane = 0.1f;
    float farPlane = 64.0f;
};

struct UBOSSAOParams {
    FMat4x4 projection;
    int32_t ssao = 1;
    int32_t ssaoOnly = 0;
    int32_t ssaoBlur = 1;
};

struct RenderMaterial {
    UniquePtr<Texture> diffuseTexture;
};

// Shader类型定义
class GBufferVS final : public StaticShaderType<GBufferVS> {
    ShaderTypeInfo(
        GBufferVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Gbuffer.esl",
        "VSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class GBufferPS final : public StaticShaderType<GBufferPS> {
    ShaderTypeInfo(
        GBufferPS,
        RHI::ShaderStageBits::sPixel,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Gbuffer.esl",
        "PSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class SSAOVS final : public StaticShaderType<SSAOVS> {
    ShaderTypeInfo(
        SSAOVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Test/Sample/Rendering-SSAO/Shader/SSAO.esl",
        "VSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class SSAOPS final : public StaticShaderType<SSAOPS> {
    ShaderTypeInfo(
        SSAOPS,
        RHI::ShaderStageBits::sPixel,
        "Engine/Test/Sample/Rendering-SSAO/Shader/SSAO.esl",
        "PSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class BlurVS final : public StaticShaderType<BlurVS> {
    ShaderTypeInfo(
        BlurVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Blur.esl",
        "VSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class BlurPS final : public StaticShaderType<BlurPS> {
    ShaderTypeInfo(
        BlurPS,
        RHI::ShaderStageBits::sPixel,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Blur.esl",
        "PSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class CompositionVS final : public StaticShaderType<CompositionVS> {
    ShaderTypeInfo(
        CompositionVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Composition.esl",
        "VSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class CompositionPS final : public StaticShaderType<CompositionPS> {
    ShaderTypeInfo(
        CompositionPS,
        RHI::ShaderStageBits::sPixel,
        "Engine/Test/Sample/Rendering-SSAO/Shader/Composition.esl",
        "PSMain")
        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

ImplementStaticShaderType(GBufferVS);
ImplementStaticShaderType(GBufferPS);
ImplementStaticShaderType(SSAOVS);
ImplementStaticShaderType(SSAOPS);
ImplementStaticShaderType(BlurVS);
ImplementStaticShaderType(BlurPS);
ImplementStaticShaderType(CompositionVS);
ImplementStaticShaderType(CompositionPS);

class SSAOApp final : public Application {
public:
    NonCopyable(SSAOApp)
        explicit SSAOApp(const std::string& n) : Application(n) {}
    ~SSAOApp() override = default;

protected:
    void OnCreate() override
    {
        InitCamera();
        LoadGLTF();
        CompileAllShaders();
        RenderThread::Get().Start();
        RenderWorkerThreads::Get().Start();

        CreateDevice();
        CreateSurface();

        RenderThread::Get().EmplaceTask([this]() -> void {
            FetchShaderInstances();
            CreateSwapChain();
            CreateVertexAndIndexBuffer();
            CreateQuadBuffer();
            CreateSamplers();
            CreateSyncObjects();
            PrepareGBuffer();
            PrepareSSAOTextures();
            PrepareUniformBuffers();
            GenerateNoiseTexture();
            GenerateRenderables();
        });
        RenderThread::Get().Flush();
    }

    void OnDrawFrame() override
    {
        uboSceneParams.view = GetCamera().GetViewMatrix();

        RenderThread::Get().EmplaceTask([this]() -> void {
            frameFence->Reset();
            const auto backTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());

            // 更新Uniform Buffer
            if (uniformBuffers.sceneParams) {
                auto* uboData = uniformBuffers.sceneParams->Map(MapMode::write, 0, sizeof(UBOSceneParams));
                memcpy(uboData, &uboSceneParams, sizeof(UBOSceneParams));
                uniformBuffers.sceneParams->UnMap();
            }

            RGBuilder builder(*device);

            // 导入交换链纹理
            auto* backTexture = builder.ImportTexture(swapChainTextures[backTextureIndex], TextureState::present);
            auto* backTextureView = builder.CreateTextureView(backTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));

            // 导入几何缓冲区纹理
            auto* gBufferPos = builder.ImportTexture(gBufferPosTex.Get(), TextureState::shaderReadOnly);
            auto* gBufferNormal = builder.ImportTexture(gBufferNormalTex.Get(), TextureState::shaderReadOnly);
            auto* gBufferAlbedo = builder.ImportTexture(gBufferAlbedoTex.Get(), TextureState::shaderReadOnly);
            auto* gBufferDepth = builder.ImportTexture(gBufferDepthTex.Get(), TextureState::depthStencilReadonly);

            // 导入SSAO纹理
            auto* ssaoTexture = builder.ImportTexture(ssaoTex.Get(), TextureState::shaderReadOnly);
            auto* ssaoBlurTexture = builder.ImportTexture(ssaoBlurTex.Get(), TextureState::shaderReadOnly);
            auto* noiseTexture = builder.ImportTexture(noiseTex.Get(), TextureState::shaderReadOnly);

            // 导入缓冲区
            auto* vBuffer = builder.ImportBuffer(vertexBuffer.Get(), BufferState::shaderReadOnly);
            auto* iBuffer = builder.ImportBuffer(indexBuffer.Get(), BufferState::shaderReadOnly);
            auto* quadVBuffer = builder.ImportBuffer(quadVertexBuffer.Get(), BufferState::shaderReadOnly);
            auto* quadIBuffer = builder.ImportBuffer(quadIndexBuffer.Get(), BufferState::shaderReadOnly);
            auto* sceneParamsBuffer = builder.ImportBuffer(uniformBuffers.sceneParams.Get(), BufferState::shaderReadOnly);
            auto* ssaoParamsBuffer = builder.ImportBuffer(uniformBuffers.ssaoParams.Get(), BufferState::shaderReadOnly);
            auto* ssaoKernelBuffer = builder.ImportBuffer(uniformBuffers.ssaoKernel.Get(), BufferState::shaderReadOnly);

            // 创建视图
            auto* gBufferPosRTV = builder.CreateTextureView(gBufferPos, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
            auto* gBufferNormalRTV = builder.CreateTextureView(gBufferNormal, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
            auto* gBufferAlbedoRTV = builder.CreateTextureView(gBufferAlbedo, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
            auto* gBufferDepthView = builder.CreateTextureView(gBufferDepth, RGTextureViewDesc(TextureViewType::depthStencil, TextureViewDimension::tv2D, TextureAspect::depth));

            auto* gBufferPosSRV = builder.CreateTextureView(gBufferPos, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));
            auto* gBufferNormalSRV = builder.CreateTextureView(gBufferNormal, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));
            auto* gBufferAlbedoSRV = builder.CreateTextureView(gBufferAlbedo, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));

            auto* ssaoRTV = builder.CreateTextureView(ssaoTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
            auto* ssaoSRV = builder.CreateTextureView(ssaoTexture, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));
            auto* ssaoBlurRTV = builder.CreateTextureView(ssaoBlurTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
            auto* ssaoBlurSRV = builder.CreateTextureView(ssaoBlurTexture, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));
            auto* noiseSRV = builder.CreateTextureView(noiseTexture, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));

            auto* vBufferView = builder.CreateBufferView(vBuffer, RGBufferViewDesc(BufferViewType::vertex, vBuffer->GetDesc().size, 0, VertexBufferViewInfo(sizeof(Vertex))));
            auto* iBufferView = builder.CreateBufferView(iBuffer, RGBufferViewDesc(BufferViewType::index, iBuffer->GetDesc().size, 0, IndexBufferViewInfo(IndexFormat::uint32)));
            auto* quadVBufferView = builder.CreateBufferView(quadVBuffer, RGBufferViewDesc(BufferViewType::vertex, quadVBuffer->GetDesc().size, 0, VertexBufferViewInfo(sizeof(QuadVertex))));
            auto* quadIBufferView = builder.CreateBufferView(quadIBuffer, RGBufferViewDesc(BufferViewType::index, quadIBuffer->GetDesc().size, 0, IndexBufferViewInfo(IndexFormat::uint32)));

            auto* sceneParamsView = builder.CreateBufferView(sceneParamsBuffer, RGBufferViewDesc(BufferViewType::uniformBinding, sizeof(UBOSceneParams)));
            auto* ssaoParamsView = builder.CreateBufferView(ssaoParamsBuffer, RGBufferViewDesc(BufferViewType::uniformBinding, sizeof(UBOSSAOParams)));
            auto* ssaoKernelView = builder.CreateBufferView(ssaoKernelBuffer, RGBufferViewDesc(BufferViewType::uniformBinding, ssaoKernelSize * sizeof(FVec4)));

            // 1. G-Buffer Pass
            auto* gBufferPipeline = PipelineCache::Get(*device).GetOrCreate(
                RasterPipelineStateDesc()
                    .SetVertexShader(gBufferVS)
                    .SetPixelShader(gBufferPS)
                    .SetVertexState(
                        RVertexState()
                            .AddVertexBufferLayout(
                                RVertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X4, offsetof(Vertex, pos)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(Vertex, uv)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("COLOR", 0), VertexFormat::float32X4, offsetof(Vertex, color)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("NORMAL", 0), VertexFormat::float32X3, offsetof(Vertex, normal)))))
                    .SetFragmentState(
                        RFragmentState()
                            .AddColorTarget(ColorTargetState(PixelFormat::rgba32Float, ColorWriteBits::all, false))
                            .AddColorTarget(ColorTargetState(PixelFormat::rgba8Unorm, ColorWriteBits::all, false))
                            .AddColorTarget(ColorTargetState(PixelFormat::rgba8Unorm, ColorWriteBits::all, false)))
                    .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint32, FrontFace::ccw, CullMode::none, false))
                    .SetDepthStencilState(DepthStencilState(true, false, PixelFormat::d32Float, CompareFunc::greaterEqual)));

            auto* gBindGroup = builder.AllocateBindGroup(
                RGBindGroupDesc::Create(gBufferPipeline->GetPipelineLayout()->GetBindGroupLayout(0))
                    .UniformBuffer("passParams", sceneParamsView));

            std::vector<RGBindGroupRef> gGroups = { gBindGroup };
            for (const auto& renderable: renderables) {
                auto* diffuseTexture = builder.ImportTexture(renderable->diffuseTexture.Get(), TextureState::shaderReadOnly);
                auto* diffuseTextureView = builder.CreateTextureView(diffuseTexture, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));

                auto* group = builder.AllocateBindGroup(
                    RGBindGroupDesc::Create(gBufferPipeline->GetPipelineLayout()->GetBindGroupLayout(1))
                        .Sampler("colorSampler", sampler.Get())
                        .Texture("colorTex", diffuseTextureView));

                gGroups.emplace_back(group);
            }

            builder.AddRasterPass(
                "GBufferPass",
                RGRasterPassDesc()
                    .AddColorAttachment(RGColorAttachment(gBufferPosRTV, LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .AddColorAttachment(RGColorAttachment(gBufferNormalRTV, LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .AddColorAttachment(RGColorAttachment(gBufferAlbedoRTV, LoadOp::clear, StoreOp::store, LinearColorConsts::black))
                    .SetDepthStencilAttachment(RGDepthStencilAttachment(gBufferDepthView, true, LoadOp::clear, StoreOp::store, 0.0f)),
                gGroups,
                [gBufferPipeline, vBufferView, iBufferView, gGroups, this](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                    recorder.SetPipeline(gBufferPipeline->GetRHI());
                    recorder.SetScissor(0, 0, GetWindowWidth(), GetWindowHeight());
                    recorder.SetViewport(0, 0, static_cast<float>(GetWindowWidth()), static_cast<float>(GetWindowHeight()), 0, 1);
                    recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                    recorder.SetVertexBuffer(0, rg.GetRHI(vBufferView));
                    recorder.SetIndexBuffer(rg.GetRHI(iBufferView));
                    recorder.SetBindGroup(0, rg.GetRHI(gGroups[0]));

                    for (int i = 0; i < renderables.size(); i++) {
                        recorder.SetBindGroup(1, rg.GetRHI(gGroups[i + 1]));
                        recorder.DrawIndexed(renderables[i]->indexCount, 1, renderables[i]->firstIndex, 0, 0);
                    }
                });

            // 2. SSAO Pass
            auto* ssaoPipeline = PipelineCache::Get(*device).GetOrCreate(
                RasterPipelineStateDesc()
                    .SetVertexShader(ssaoVS)
                    .SetPixelShader(ssaoPS)
                    .SetVertexState(
                        RVertexState()
                            .AddVertexBufferLayout(
                                RVertexBufferLayout(VertexStepMode::perVertex, sizeof(QuadVertex))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X3, offsetof(QuadVertex, pos)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(QuadVertex, uv)))))
                    .SetFragmentState(
                        RFragmentState()
                            .AddColorTarget(ColorTargetState(PixelFormat::r8Unorm, ColorWriteBits::all, false)))
                    .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint32, FrontFace::ccw, CullMode::none)));

            auto* ssaoBindGroup = builder.AllocateBindGroup(
                RGBindGroupDesc::Create(ssaoPipeline->GetPipelineLayout()->GetBindGroupLayout(0))
                    .Texture("posDepthTex", gBufferPosSRV)
                    .Texture("normalTex", gBufferNormalSRV)
                    .Texture("ssaoNoiseTex", noiseSRV)
                    .Sampler("texSampler", sampler.Get())
                    .Sampler("ssaoNoiseSampler", noiseSampler.Get())
                    .UniformBuffer("kernalParams", ssaoKernelView)
                    .UniformBuffer("passParams", ssaoParamsView));

            builder.AddRasterPass(
                "SSAOPass",
                RGRasterPassDesc()
                    .AddColorAttachment(RGColorAttachment(ssaoRTV, LoadOp::clear, StoreOp::store, LinearColorConsts::black)),
                { ssaoBindGroup },
                [ssaoPipeline, quadVBufferView, quadIBufferView, ssaoBindGroup, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                    recorder.SetPipeline(ssaoPipeline->GetRHI());
                    recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
                    recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
                    recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                    recorder.SetVertexBuffer(0, rg.GetRHI(quadVBufferView));
                    recorder.SetIndexBuffer(rg.GetRHI(quadIBufferView));
                    recorder.SetBindGroup(0, rg.GetRHI(ssaoBindGroup));
                    recorder.DrawIndexed(6, 1, 0, 0, 0);
                });

            // 3. SSAO Blur Pass
            auto* ssaoBlurPipeline = PipelineCache::Get(*device).GetOrCreate(
                RasterPipelineStateDesc()
                    .SetVertexShader(blurVS)
                    .SetPixelShader(blurPS)
                    .SetVertexState(
                        RVertexState()
                            .AddVertexBufferLayout(
                                RVertexBufferLayout(VertexStepMode::perVertex, sizeof(QuadVertex))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X3, offsetof(QuadVertex, pos)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(QuadVertex, uv)))))
                    .SetFragmentState(
                        RFragmentState()
                            .AddColorTarget(ColorTargetState(PixelFormat::r8Unorm, ColorWriteBits::all, false)))
                    .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint32, FrontFace::ccw, CullMode::none)));

            auto* ssaoBlurBindGroup = builder.AllocateBindGroup(
                RGBindGroupDesc::Create(ssaoBlurPipeline->GetPipelineLayout()->GetBindGroupLayout(0))
                    .Texture("ssaoTex", ssaoSRV)
                    .Sampler("ssaoSampler", sampler.Get()));

            builder.AddRasterPass(
                "SSAOBlurPass",
                RGRasterPassDesc()
                    .AddColorAttachment(RGColorAttachment(ssaoBlurRTV, LoadOp::clear, StoreOp::store, LinearColorConsts::black)),
                { ssaoBlurBindGroup },
                [ssaoBlurPipeline, quadVBufferView, quadIBufferView, ssaoBlurBindGroup, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                    recorder.SetPipeline(ssaoBlurPipeline->GetRHI());
                    recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
                    recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
                    recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                    recorder.SetVertexBuffer(0, rg.GetRHI(quadVBufferView));
                    recorder.SetIndexBuffer(rg.GetRHI(quadIBufferView));
                    recorder.SetBindGroup(0, rg.GetRHI(ssaoBlurBindGroup));
                    recorder.DrawIndexed(6, 1, 0, 0, 0);
                });

            // 4. Composition Pass
            auto* compositionPipeline = PipelineCache::Get(*device).GetOrCreate(
                RasterPipelineStateDesc()
                    .SetVertexShader(compositionVS)
                    .SetPixelShader(compositionPS)
                    .SetVertexState(
                        RVertexState()
                            .AddVertexBufferLayout(
                                RVertexBufferLayout(VertexStepMode::perVertex, sizeof(QuadVertex))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X3, offsetof(QuadVertex, pos)))
                                    .AddAttribute(RVertexAttribute(RVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(QuadVertex, uv)))))
                    .SetFragmentState(
                        RFragmentState()
                            .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all, false)))
                    .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint32, FrontFace::ccw, CullMode::none)));

            auto* compositionBindGroup = builder.AllocateBindGroup(
                RGBindGroupDesc::Create(compositionPipeline->GetPipelineLayout()->GetBindGroupLayout(0))
                    .Texture("posTex", gBufferPosSRV)
                    .Texture("normalTex", gBufferNormalSRV)
                    .Texture("albedoTex", gBufferAlbedoSRV)
                    .Texture("ssaoTex", ssaoSRV)
                    .Texture("ssaoBluredTex", ssaoBlurSRV)
                    .Sampler("texSampler", sampler.Get())
                    .UniformBuffer("passParams", ssaoParamsView));

            builder.AddRasterPass(
                "CompositionPass",
                RGRasterPassDesc()
                    .AddColorAttachment(RGColorAttachment(backTextureView, LoadOp::clear, StoreOp::store, LinearColorConsts::black)),
                { compositionBindGroup },
                [compositionPipeline, quadVBufferView, quadIBufferView, compositionBindGroup, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                    recorder.SetPipeline(compositionPipeline->GetRHI());
                    recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
                    recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
                    recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                    recorder.SetVertexBuffer(0, rg.GetRHI(quadVBufferView));
                    recorder.SetIndexBuffer(rg.GetRHI(quadIBufferView));
                    recorder.SetBindGroup(0, rg.GetRHI(compositionBindGroup));
                    recorder.DrawIndexed(6, 1, 0, 0, 0);
                },
                {},
                [backTexture](const RGBuilder& rg, CommandRecorder& recorder) -> void {
                    recorder.ResourceBarrier(Barrier::Transition(rg.GetRHI(backTexture), TextureState::renderTarget, TextureState::present));
                });

            // 执行渲染图
            RGExecuteInfo executeInfo;
            executeInfo.semaphoresToWait = { imageReadySemaphore.Get() };
            executeInfo.semaphoresToSignal = { renderFinishedSemaphore.Get() };
            executeInfo.inFenceToSignal = frameFence.Get();
            builder.Execute(executeInfo);

            swapChain->Present(renderFinishedSemaphore.Get());
            frameFence->Wait();

            Core::ThreadContext::IncFrameNumber();
            BufferPool::Get(*device).Forfeit();
            TexturePool::Get(*device).Forfeit();
            ResourceViewCache::Get(*device).Forfeit();
            BindGroupCache::Get(*device).Forfeit();
        });

        RenderThread::Get().Flush();
    }

    void OnDestroy() override
    {
        RenderThread::Get().EmplaceTask([this]() -> void {
            const UniquePtr<Fence> fence = device->CreateFence(false);
            device->GetQueue(QueueType::graphics, 0)->Flush(fence.Get());
            fence->Wait();

            BindGroupCache::Get(*device).Invalidate();
            PipelineCache::Get(*device).Invalidate();
            BufferPool::Get(*device).Invalidate();
            TexturePool::Get(*device).Invalidate();
            ShaderMap::Get(*device).Invalidate();
        });
        RenderThread::Get().Flush();

        RenderWorkerThreads::Get().Stop();
        RenderThread::Get().Stop();
    }

private:
    static constexpr uint8_t ssaoKernelSize = 64;
    static constexpr uint8_t ssaoNoiseDim = 16;
    static constexpr size_t backBufferCount = 2;

    PixelFormat swapChainFormat = PixelFormat::max;

    // Shader instances
    ShaderInstance gBufferVS;
    ShaderInstance gBufferPS;
    ShaderInstance ssaoVS;
    ShaderInstance ssaoPS;
    ShaderInstance blurVS;
    ShaderInstance blurPS;
    ShaderInstance compositionVS;
    ShaderInstance compositionPS;

    // Resources
    UniquePtr<Device> device;
    UniquePtr<Surface> surface;
    UniquePtr<SwapChain> swapChain;
    std::array<Texture*, backBufferCount> swapChainTextures;

    UniquePtr<Buffer> vertexBuffer;
    UniquePtr<Buffer> indexBuffer;
    UniquePtr<Buffer> quadVertexBuffer;
    UniquePtr<Buffer> quadIndexBuffer;

    UniquePtr<Texture> gBufferPosTex;
    UniquePtr<Texture> gBufferNormalTex;
    UniquePtr<Texture> gBufferAlbedoTex;
    UniquePtr<Texture> gBufferDepthTex;
    UniquePtr<Texture> ssaoTex;
    UniquePtr<Texture> ssaoBlurTex;
    UniquePtr<Texture> noiseTex;

    UniquePtr<RHI::Sampler> sampler;
    UniquePtr<RHI::Sampler> noiseSampler;

    UniquePtr<Semaphore> imageReadySemaphore;
    UniquePtr<Semaphore> renderFinishedSemaphore;
    UniquePtr<Fence> frameFence;

    // Uniform buffers
    struct {
        UniquePtr<Buffer> sceneParams;
        UniquePtr<Buffer> ssaoKernel;
        UniquePtr<Buffer> ssaoParams;
    } uniformBuffers;

    UBOSceneParams uboSceneParams;
    UBOSSAOParams ubossaoParams;

    // Model data
    UniquePtr<Model> model;

    struct Renderable {
        uint32_t indexCount;
        uint32_t firstIndex;
        UniquePtr<Texture> diffuseTexture;

        Renderable(uint32_t idxCount, uint32_t firstIdx, UniquePtr<Texture>&& tex)
            : indexCount(idxCount)
            , firstIndex(firstIdx)
            , diffuseTexture(std::move(tex))
        {}
    };

    std::vector<UniquePtr<Renderable>> renderables;
    std::vector<RenderMaterial> materials;

    // Helper methods
    void CompileAllShaders() const
    {
        ShaderCompileOptions options;
        options.includeDirectories = {"../Test/Sample/ShaderInclude", "../Test/Sample/Rendering-SSAO/Shader"};
        options.byteCodeType = GetRHIType() == RHI::RHIType::directX12 ? ShaderByteCodeType::dxil : ShaderByteCodeType::spirv;
        options.withDebugInfo = false;
        auto result = ShaderTypeCompiler::Get().CompileAll(options);
        const auto& [success, errorInfo] = result.get();
        Assert(success);
    }

    void CreateDevice()
    {
        device = GetRHIInstance()
                     ->GetGpu(0)
                     ->RequestDevice(
                         DeviceCreateInfo()
                             .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
    }

    void CreateSurface()
    {
        surface = device->CreateSurface(SurfaceCreateInfo(GetPlatformWindow()));
    }

    void FetchShaderInstances()
    {
        ShaderArtifactRegistry::Get().PerformThreadCopy();
        gBufferVS = ShaderMap::Get(*device).GetShaderInstance(GBufferVS::Get(), {});
        gBufferPS = ShaderMap::Get(*device).GetShaderInstance(GBufferPS::Get(), {});
        ssaoVS = ShaderMap::Get(*device).GetShaderInstance(SSAOVS::Get(), {});
        ssaoPS = ShaderMap::Get(*device).GetShaderInstance(SSAOPS::Get(), {});
        blurVS = ShaderMap::Get(*device).GetShaderInstance(BlurVS::Get(), {});
        blurPS = ShaderMap::Get(*device).GetShaderInstance(BlurPS::Get(), {});
        compositionVS = ShaderMap::Get(*device).GetShaderInstance(CompositionVS::Get(), {});
        compositionPS = ShaderMap::Get(*device).GetShaderInstance(CompositionPS::Get(), {});
    }

    void CreateSwapChain()
    {
        static std::vector swapChainFormatQualifiers = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm
        };

        for (const auto format : swapChainFormatQualifiers) {
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
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetSurface(surface.Get())
                .SetPresentQueue(device->GetQueue(QueueType::graphics, 0)));

        for (auto i = 0; i < backBufferCount; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);
        }
    }

    void CreateVertexAndIndexBuffer()
    {
        if (!model) return;

        // Vertex buffer
        const BufferCreateInfo vBufferInfo = BufferCreateInfo()
                                                 .SetSize(model->rawVertBuffer.size() * sizeof(Vertex))
                                                 .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                 .SetInitialState(BufferState::staging)
                                                 .SetDebugName("vertexBuffer");

        vertexBuffer = device->CreateBuffer(vBufferInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::write, 0, vBufferInfo.size);
            memcpy(data, model->rawVertBuffer.data(), vBufferInfo.size);
            vertexBuffer->UnMap();
        }

        // Index buffer
        const BufferCreateInfo iBufferInfo = BufferCreateInfo()
                                                 .SetSize(model->rawIndBuffer.size() * sizeof(uint32_t))
                                                 .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                 .SetInitialState(BufferState::staging)
                                                 .SetDebugName("indexBuffer");

        indexBuffer = device->CreateBuffer(iBufferInfo);
        if (indexBuffer != nullptr) {
            auto* data = indexBuffer->Map(MapMode::write, 0, iBufferInfo.size);
            memcpy(data, model->rawIndBuffer.data(), iBufferInfo.size);
            indexBuffer->UnMap();
        }
    }

    void CreateQuadBuffer()
    {
        // Quad vertex buffer
        const std::vector<QuadVertex> vertices = {
            {{-1.0f, -1.0f, .0f}, {0.f, 1.0f}},
            {{1.0f, -1.0f, .0f}, {1.0f, 1.0f}},
            {{1.0f, 1.0f, .0f}, {1.0f, .0f}},
            {{-1.0f, 1.0f, .0f}, {0.f, .0f}},
        };

        const BufferCreateInfo vBufferInfo = BufferCreateInfo()
                                                 .SetSize(vertices.size() * sizeof(QuadVertex))
                                                 .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                 .SetInitialState(BufferState::staging)
                                                 .SetDebugName("quadVertexBuffer");

        quadVertexBuffer = device->CreateBuffer(vBufferInfo);
        if (quadVertexBuffer != nullptr) {
            auto* data = quadVertexBuffer->Map(MapMode::write, 0, vBufferInfo.size);
            memcpy(data, vertices.data(), vBufferInfo.size);
            quadVertexBuffer->UnMap();
        }

        // Quad index buffer
        const std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        const BufferCreateInfo iBufferInfo = BufferCreateInfo()
                                                 .SetSize(indices.size() * sizeof(uint32_t))
                                                 .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                 .SetInitialState(BufferState::staging)
                                                 .SetDebugName("quadIndexBuffer");

        quadIndexBuffer = device->CreateBuffer(iBufferInfo);
        if (quadIndexBuffer != nullptr) {
            auto* data = quadIndexBuffer->Map(MapMode::write, 0, iBufferInfo.size);
            memcpy(data, indices.data(), iBufferInfo.size);
            quadIndexBuffer->UnMap();
        }
    }

    void CreateSamplers()
    {
        sampler = device->CreateSampler(SamplerCreateInfo());
        noiseSampler = device->CreateSampler(
            SamplerCreateInfo()
                .SetAddressModeU(AddressMode::repeat)
                .SetAddressModeV(AddressMode::repeat));
    }

    void CreateSyncObjects()
    {
        imageReadySemaphore = device->CreateSemaphore();
        renderFinishedSemaphore = device->CreateSemaphore();
        frameFence = device->CreateFence(true);
    }

    void PrepareGBuffer()
    {
        // Position buffer (RGBA32F)
        gBufferPosTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba32Float)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));

        // Normal buffer (RGBA8Unorm)
        gBufferNormalTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba8Unorm)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));

        // Albedo buffer (RGBA8Unorm)
        gBufferAlbedoTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba8Unorm)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));

        // Depth buffer
        gBufferDepthTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::d32Float)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::depthStencilAttachment)
                .SetInitialState(TextureState::depthStencilWrite));
    }

    void PrepareSSAOTextures()
    {
        // SSAO texture
        ssaoTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::r8Unorm)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));

        // SSAO blur texture
        ssaoBlurTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::r8Unorm)
                .SetMipLevels(1)
                .SetWidth(GetWindowWidth())
                .SetHeight(GetWindowHeight())
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::textureBinding | TextureUsageBits::renderAttachment)
                .SetInitialState(TextureState::shaderReadOnly));
    }

    void PrepareUniformBuffers()
    {
        // Scene parameters
        const auto aixsTransMat = FMat4x4 {
            0, 0, -1, 0,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 0, 1
        };

        uboSceneParams.projection = GetCamera().GetProjectionMatrix();
        uboSceneParams.view = GetCamera().GetViewMatrix();
        uboSceneParams.model = aixsTransMat;

        const BufferCreateInfo sceneInfo = BufferCreateInfo()
                                               .SetSize(sizeof(UBOSceneParams))
                                               .SetUsages(BufferUsageBits::uniform | BufferUsageBits::mapWrite)
                                               .SetInitialState(BufferState::staging)
                                               .SetDebugName("sceneParams");

        uniformBuffers.sceneParams = device->CreateBuffer(sceneInfo);
        if (uniformBuffers.sceneParams != nullptr) {
            auto* data = uniformBuffers.sceneParams->Map(MapMode::write, 0, sizeof(UBOSceneParams));
            memcpy(data, &uboSceneParams, sizeof(UBOSceneParams));
            uniformBuffers.sceneParams->UnMap();
        }

        // SSAO parameters
        ubossaoParams.projection = GetCamera().GetProjectionMatrix();

        const BufferCreateInfo ssaoParamsInfo = BufferCreateInfo()
                                                    .SetSize(sizeof(UBOSSAOParams))
                                                    .SetUsages(BufferUsageBits::uniform | BufferUsageBits::mapWrite)
                                                    .SetInitialState(BufferState::staging)
                                                    .SetDebugName("ssaoParams");

        uniformBuffers.ssaoParams = device->CreateBuffer(ssaoParamsInfo);
        if (uniformBuffers.ssaoParams != nullptr) {
            auto* data = uniformBuffers.ssaoParams->Map(MapMode::write, 0, sizeof(UBOSSAOParams));
            memcpy(data, &ubossaoParams, sizeof(UBOSSAOParams));
            uniformBuffers.ssaoParams->UnMap();
        }

        // SSAO kernel
        std::default_random_engine rndEngine(static_cast<unsigned>(time(nullptr)));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
        std::vector<FVec4> ssaoKernel(ssaoKernelSize);

        auto lerp = [](float a, float b, float f) ->float {
            return a + f * (b - a);
        };

        for (uint32_t i = 0; i < ssaoKernelSize; ++i) {
            FVec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
            sample.Normalize();
            sample *= rndDist(rndEngine);
            float scale = static_cast<float>(i) / static_cast<float>(ssaoKernelSize);
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample = sample * scale;
            ssaoKernel[i] = FVec4(sample.x, sample.y, sample.z, 0.0f);
        }

        const BufferCreateInfo kernelInfo = BufferCreateInfo()
                                                .SetSize(ssaoKernel.size() * sizeof(FVec4))
                                                .SetUsages(BufferUsageBits::uniform | BufferUsageBits::mapWrite)
                                                .SetInitialState(BufferState::staging)
                                                .SetDebugName("ssaoKernel");

        uniformBuffers.ssaoKernel = device->CreateBuffer(kernelInfo);
        if (uniformBuffers.ssaoKernel != nullptr) {
            auto* data = uniformBuffers.ssaoKernel->Map(MapMode::write, 0, kernelInfo.size);
            memcpy(data, ssaoKernel.data(), kernelInfo.size);
            uniformBuffers.ssaoKernel->UnMap();
        }
    }

    void GenerateNoiseTexture()
    {
        std::default_random_engine rndEngine(static_cast<unsigned>(time(nullptr)));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

        std::vector<FVec4> ssaoNoise(ssaoNoiseDim * ssaoNoiseDim);
        for (auto& randomVec : ssaoNoise) {
            randomVec = FVec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
        }

        const BufferCreateInfo bufferInfo = BufferCreateInfo()
                                                .SetSize(ssaoNoise.size() * sizeof(FVec4))
                                                .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                .SetInitialState(BufferState::staging)
                                                .SetDebugName("noiseStaging");

        const UniquePtr<Buffer> stagingBuffer = device->CreateBuffer(bufferInfo);
        if (stagingBuffer != nullptr) {
            auto* data = stagingBuffer->Map(MapMode::write, 0, bufferInfo.size);
            memcpy(data, ssaoNoise.data(), bufferInfo.size);
            stagingBuffer->UnMap();
        }

        noiseTex = device->CreateTexture(
            TextureCreateInfo()
                .SetFormat(PixelFormat::rgba32Float)
                .SetMipLevels(1)
                .SetWidth(ssaoNoiseDim)
                .SetHeight(ssaoNoiseDim)
                .SetDepthOrArraySize(1)
                .SetDimension(TextureDimension::t2D)
                .SetSamples(1)
                .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
                .SetInitialState(TextureState::undefined));

        // Copy data
        auto copyCmdBuffer = device->CreateCommandBuffer();
        const UniquePtr<CommandRecorder> commandRecorder = copyCmdBuffer->Begin();
        {
            const UniquePtr<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
            {
                copyRecorder->ResourceBarrier(Barrier::Transition(noiseTex.Get(), TextureState::undefined, TextureState::copyDst));
                copyRecorder->CopyBufferToTexture(
                    stagingBuffer.Get(),
                    noiseTex.Get(),
                    BufferTextureCopyInfo(0, TextureSubResourceInfo(), UVec3Consts::zero, UVec3(ssaoNoiseDim, ssaoNoiseDim, 1)));
                copyRecorder->ResourceBarrier(Barrier::Transition(noiseTex.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
            }
            copyRecorder->EndPass();
        }
        commandRecorder->End();

        const UniquePtr<Fence> fence = device->CreateFence(false);
        QueueSubmitInfo submitInfo {};
        submitInfo.signalFence = fence.Get();
        device->GetQueue(QueueType::graphics, 0)->Submit(copyCmdBuffer.Get(), submitInfo);
        fence->Wait();
    }

    void GenerateRenderables()
    {
        if (!model) return;

        for (const auto& mesh : model->meshes) {
            UniquePtr<Texture> diffuseTex = nullptr;

            // 加载材质纹理
            if (mesh->materialData && mesh->materialData->baseColorTexture) {
                const auto& texData = mesh->materialData->baseColorTexture;

                diffuseTex = device->CreateTexture(
                    TextureCreateInfo()
                        .SetFormat(PixelFormat::rgba8Unorm)
                        .SetDimension(TextureDimension::t2D)
                        .SetMipLevels(1)
                        .SetWidth(texData->width)
                        .SetHeight(texData->height)
                        .SetDepthOrArraySize(1)
                        .SetSamples(1)
                        .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
                        .SetInitialState(TextureState::undefined));

                const auto copyFootprint = device->GetTextureSubResourceCopyFootprint(*diffuseTex, TextureSubResourceInfo());
                const BufferCreateInfo bufferInfo = BufferCreateInfo()
                                                        .SetSize(copyFootprint.totalBytes)
                                                        .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
                                                        .SetInitialState(BufferState::staging);

                const UniquePtr<Buffer> stagingBuffer = device->CreateBuffer(bufferInfo);
                if (stagingBuffer != nullptr) {
                    auto* data = stagingBuffer->Map(MapMode::write, 0, bufferInfo.size);
                    for (auto i = 0; i < texData->height; i++) {
                        const auto srcRowPitch = texData->width * copyFootprint.bytesPerPixel;
                        const auto* src = texData->buffer.data() + i * srcRowPitch;
                        auto* dst = static_cast<uint8_t*>(data) + i * copyFootprint.rowPitch;
                        memcpy(dst, src, srcRowPitch);
                    }
                    stagingBuffer->UnMap();
                }

                auto copyCmdBuffer = device->CreateCommandBuffer();
                const UniquePtr<CommandRecorder> commandRecorder = copyCmdBuffer->Begin();
                {
                    const UniquePtr<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
                    {
                        copyRecorder->ResourceBarrier(Barrier::Transition(diffuseTex.Get(), TextureState::undefined, TextureState::copyDst));
                        copyRecorder->CopyBufferToTexture(
                            stagingBuffer.Get(),
                            diffuseTex.Get(),
                            BufferTextureCopyInfo(0, TextureSubResourceInfo(), UVec3Consts::zero, UVec3(texData->width, texData->height, 1)));
                        copyRecorder->ResourceBarrier(Barrier::Transition(diffuseTex.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
                    }
                    copyRecorder->EndPass();
                }
                commandRecorder->End();

                const UniquePtr<Fence> fence = device->CreateFence(false);
                QueueSubmitInfo submitInfo {};
                submitInfo.signalFence = fence.Get();
                device->GetQueue(QueueType::graphics, 0)->Submit(copyCmdBuffer.Get(), submitInfo);
                fence->Wait();
            }

            auto renderable = MakeUnique<Renderable>(mesh->indexCount, mesh->firstIndex, std::move(diffuseTex));
            renderables.push_back(std::move(renderable));
        }
    }

    void InitCamera()
    {
        auto* camera = new Camera(
            FVec3(.0f, -5.0f, 2.0f),
            FVec3(.0f, .0f, -90.0f),
            Camera::ProjectionParams {
                60.0f,
                static_cast<float>(GetWindowWidth()),
                static_cast<float>(GetWindowHeight()),
                0.1f,
                64.0f
            });

        camera->SetMoveSpeed(5.f);
        camera->SetRotateSpeed(0.1f);
        SetCamera(camera);
    }

    void LoadGLTF()
    {
        model = MakeUnique<Model>();
        model->LoadFromFile("../Test/Sample/Rendering-SSAO/Model/Voyager.gltf");
    }
};

int main(int argc, char* argv[])
{
    SSAOApp application("Rendering-SSAO");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    return application.RunLoop();
}