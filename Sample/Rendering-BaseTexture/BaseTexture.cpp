//
// Created by swtpotato on 2022/10/21.
//

#include <Application.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Render/RenderGraph.h>
#include <Render/RenderThread.h>

using namespace Common;
using namespace Render;
using namespace RHI;

struct Vertex {
    FVec4 position;
    FVec2 uv;
};

struct VertUniform {
    FMat4x4 modelMatrix;
};

class BaseTexVS final : public StaticShaderType<BaseTexVS> {
    ShaderTypeInfo(
        BaseTexVS,
        RHI::ShaderStageBits::sVertex,
        "Engine/Test/Sample/Rendering-BaseTexture/BaseTexture.esl",
        "VSMain")

        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

class BaseTexPS final : public StaticShaderType<BaseTexPS> {
public:
    ShaderTypeInfo(
        BaseTexPS,
        RHI::ShaderStageBits::sPixel,
        "Engine/Test/Sample/Rendering-BaseTexture/BaseTexture.esl",
        "PSMain")

        EmptyIncludeDirectories
        EmptyVariantFieldVec
};

ImplementStaticShaderType(BaseTexVS);
ImplementStaticShaderType(BaseTexPS);

class BaseTexApp final : public Application {
public:
    NonCopyable(BaseTexApp)
    explicit BaseTexApp(const std::string& inName);
    ~BaseTexApp() override;

    void OnCreate() override;
    void OnDrawFrame() override;
    void OnDestroy() override;

private:
    static constexpr size_t backBufferCount = 2;

    void CreateDevice();
    void CompileAllShaders() const;
    void FetchShaderInstances();
    void CreateSwapChain();
    void CreateVertexAndIndexBuffer();
    void CreateTextureAndSampler();
    void CreateSyncObjects();

    PixelFormat swapChainFormat;
    ShaderInstance vs;
    ShaderInstance ps;
    UniquePtr<Device> device;
    UniquePtr<Surface> surface;
    UniquePtr<SwapChain> swapChain;
    UniquePtr<RHI::Sampler> sampler;
    UniquePtr<Buffer> vertexBuffer;
    UniquePtr<Buffer> indexBuffer;
    UniquePtr<Buffer> uniformBuffer;
    UniquePtr<Texture> texture;
    UniquePtr<Buffer> imageBuffer;
    std::array<Texture*, backBufferCount> swapChainTextures;
    UniquePtr<RasterPipeline> pipeline;
    UniquePtr<Semaphore> imageReadySemaphore;
    UniquePtr<Semaphore> renderFinishedSemaphore;
    UniquePtr<Fence> frameFence;
};

BaseTexApp::BaseTexApp(const std::string& inName)
    : Application(inName)
    , swapChainFormat(PixelFormat::max)
    , swapChainTextures()
{
}

BaseTexApp::~BaseTexApp() = default;

void BaseTexApp::OnCreate()
{
    CompileAllShaders();
    RenderThread::Get().Start();
    RenderWorkerThreads::Get().Start();

    RenderThread::Get().EmplaceTask([this]() -> void {
        CreateDevice();
        FetchShaderInstances();
        CreateSwapChain();
        CreateVertexAndIndexBuffer();
        CreateTextureAndSampler();
        CreateSyncObjects();
    });
}

void BaseTexApp::OnDrawFrame()
{
    RenderThread::Get().EmplaceTask([this]() -> void {
        frameFence->Reset();
        const auto backTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());

        auto* pso = PipelineCache::Get(*device).GetOrCreate(
            RasterPipelineStateDesc()
                .SetVertexShader(vs)
                .SetPixelShader(ps)
                .SetVertexState(
                    RVertexState()
                        .AddVertexBufferLayout(
                            RVertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                                .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X4, offsetof(Vertex, position)))
                                .AddAttribute(RVertexAttribute(RVertexBinding("TEXCOORD", 0), VertexFormat::float32X2, offsetof(Vertex, uv)))))
                .SetFragmentState(
                    RFragmentState()
                        .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all, false)))
                .SetPrimitiveState(PrimitiveState(PrimitiveTopologyType::triangle, FillMode::solid, IndexFormat::uint16, FrontFace::ccw, CullMode::none)));

        RGBuilder builder(*device);
        auto* backTexture = builder.ImportTexture(swapChainTextures[backTextureIndex], TextureState::present);
        auto* backTextureView = builder.CreateTextureView(backTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
        auto* vBuffer = builder.ImportBuffer(vertexBuffer.Get(), BufferState::shaderReadOnly);
        auto* vBufferView = builder.CreateBufferView(vBuffer, RGBufferViewDesc(BufferViewType::vertex, vBuffer->GetDesc().size, 0, VertexBufferViewInfo(sizeof(Vertex))));
        auto* iBuffer = builder.ImportBuffer(indexBuffer.Get(), BufferState::shaderReadOnly);
        auto* iBufferView = builder.CreateBufferView(iBuffer, RGBufferViewDesc(BufferViewType::index, iBuffer->GetDesc().size, 0, IndexBufferViewInfo(IndexFormat::uint32)));
        auto* uBuffer = builder.CreateBuffer(RGBufferDesc(sizeof(VertUniform), BufferUsageBits::uniform | BufferUsageBits::mapWrite, BufferState::staging, "psUniform"));
        auto* uBufferView = builder.CreateBufferView(uBuffer, RGBufferViewDesc(BufferViewType::uniformBinding, sizeof(VertUniform)));
        auto* rgTexture = builder.ImportTexture(texture.Get(), TextureState::undefined);
        auto* rgTextureView = builder.CreateTextureView(rgTexture, RGTextureViewDesc(TextureViewType::textureBinding, TextureViewDimension::tv2D));

        auto* bindGroup = builder.AllocateBindGroup(
            RGBindGroupDesc::Create(pso->GetPipelineLayout()->GetBindGroupLayout(0))
                .UniformBuffer("constantBuffer", uBufferView)
                .Sampler("colorSampler", sampler.Get())
                .Texture("colorTex", rgTextureView));

        VertUniform vertUniform {};
        vertUniform.modelMatrix = MatConsts<float, 4, 4>::identity;

        builder.QueueBufferUpload(
            uBuffer,
            RGBufferUploadInfo(&vertUniform, sizeof(VertUniform)));

        builder.AddRasterPass(
            "BasePass",
            RGRasterPassDesc()
                .AddColorAttachment(RGColorAttachment(backTextureView, LoadOp::clear, StoreOp::store)),
            { bindGroup },
            [pso, vBufferView, iBufferView, bindGroup, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                recorder.SetPipeline(pso->GetRHI());
                recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
                recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
                recorder.SetVertexBuffer(0, rg.GetRHI(vBufferView));
                recorder.SetIndexBuffer(rg.GetRHI(iBufferView));
                recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                recorder.SetBindGroup(0, rg.GetRHI(bindGroup));
                recorder.DrawIndexed(6, 1, 0, 0, 0);
            },
            {},
            [backTexture](const RGBuilder& rg, CommandRecorder& recorder) -> void {
                recorder.ResourceBarrier(Barrier::Transition(rg.GetRHI(backTexture), TextureState::renderTarget, TextureState::present));
            });

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

    // TODO in sample, just sync with render thread every frame, maybe later need a better render-thread based application class
    RenderThread::Get().Flush();
}

void BaseTexApp::OnDestroy()
{
    RenderThread::Get().EmplaceTask([this]() -> void {
        const UniquePtr<Fence> fence = device->CreateFence(false);
        device->GetQueue(QueueType::graphics, 0)->Flush(fence.Get());
        fence->Wait();

        BindGroupCache::Get(*device).Invalidate();
        PipelineCache::Get(*device).Invalidate();
        BufferPool::Get(*device).Invalidate();
        TexturePool::Get(*device).Invalidate();
    });
    RenderThread::Get().Flush();

    RenderWorkerThreads::Get().Stop();
    RenderThread::Get().Stop();
}

void BaseTexApp::CreateDevice()
{
    device = GetRHIInstance()
                 ->GetGpu(0)
                 ->RequestDevice(
                     DeviceCreateInfo()
                         .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
}

void BaseTexApp::CompileAllShaders() const
{
    ShaderCompileOptions options;
    options.includeDirectories = {"../Test/Sample/ShaderInclude", "../Test/Sample/Rendering-BaseTexture"};
    options.byteCodeType = GetRHIType() == RHI::RHIType::directX12 ? ShaderByteCodeType::dxil : ShaderByteCodeType::spirv;
    options.withDebugInfo = false;
    auto result = ShaderTypeCompiler::Get().CompileAll(options);
    const auto& [success, errorInfo] = result.get();
    Assert(success);
}

void BaseTexApp::FetchShaderInstances()
{
    ShaderArtifactRegistry::Get().PerformThreadCopy();
    vs = ShaderMap::Get(*device).GetShaderInstance(BaseTexVS::Get(), {});
    ps = ShaderMap::Get(*device).GetShaderInstance(BaseTexPS::Get(), {});
}

void BaseTexApp::CreateSwapChain()
{
    static std::vector swapChainFormatQualifiers = {
        PixelFormat::rgba8Unorm,
        PixelFormat::bgra8Unorm
    };

    surface = device->CreateSurface(SurfaceCreateInfo(GetPlatformWindow()));

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

void BaseTexApp::CreateVertexAndIndexBuffer()
{
    const std::vector<Vertex> vertices = {
        {{-.5f, -.5f, .0f, 1.f}, {.0f, 1.0f}},
        {{.5f, -.5f, .0f,1.f}, {1.0f, 1.0f}},
        {{.5f, .5f, .0f, 1.f}, {1.0f, .0f}},
        {{-.5f, .5f, .0f, 1.f}, {0.f, .0f}},
    };

    const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
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

    const std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
    const BufferCreateInfo indexInfo = BufferCreateInfo()
          .SetSize(indices.size() * sizeof(uint32_t))
          .SetUsages(BufferUsageBits::index | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
          .SetInitialState(BufferState::staging)
          .SetDebugName("indexBuffer");

    indexBuffer = device->CreateBuffer(indexInfo);
    if (indexBuffer != nullptr) {
        auto* data = indexBuffer->Map(MapMode::write, 0, indexInfo.size);
        memcpy(data, indices.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();
    }
}

void BaseTexApp::CreateTextureAndSampler()
{
    int width, height, channel = 0;
    stbi_uc* imgData = stbi_load("../Test/Sample/Rendering-BaseTexture/Awesomeface.png", &width, &height, &channel, STBI_rgb_alpha);
    Assert(imgData != nullptr);

    texture = device->CreateTexture(
        TextureCreateInfo()
            .SetFormat(PixelFormat::rgba8Unorm)
            .SetMipLevels(1)
            .SetWidth(width)
            .SetHeight(height)
            .SetDepthOrArraySize(1)
            .SetDimension(TextureDimension::t2D)
            .SetSamples(1)
            .SetUsages(TextureUsageBits::copyDst | TextureUsageBits::textureBinding)
            .SetInitialState(TextureState::undefined)
        );

    const auto copyFootprint = device->GetTextureSubResourceCopyFootprint(*texture, TextureSubResourceInfo());

    const BufferCreateInfo info = BufferCreateInfo()
        .SetSize(copyFootprint.totalBytes)
        .SetUsages(BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
        .SetInitialState(BufferState::staging)
        .SetDebugName("texStagingBuffer");

    imageBuffer = device->CreateBuffer(info);
    if (imageBuffer != nullptr) {
        auto* data = imageBuffer->Map(MapMode::write, 0, info.size);
        for (auto i = 0; i < height; i++) {
            const auto srcRowPitch = width * copyFootprint.bytesPerPixel;
            const auto* src = imgData + i * srcRowPitch;
            auto* dst = static_cast<uint8_t*>(data) + i * copyFootprint.rowPitch;
            memcpy(dst, src, srcRowPitch);
        }
        imageBuffer->UnMap();
    }
    stbi_image_free(imgData);

    sampler = device->CreateSampler(SamplerCreateInfo());

    // perform buffer->texture copy
    auto copyCmdBuffer = device->CreateCommandBuffer();
    const UniquePtr<CommandRecorder> commandRecorder = copyCmdBuffer->Begin();
    {
        const UniquePtr<CopyPassCommandRecorder> copyRecorder = commandRecorder->BeginCopyPass();
        {
            copyRecorder->ResourceBarrier(Barrier::Transition(texture.Get(), TextureState::undefined, TextureState::copyDst));
            copyRecorder->CopyBufferToTexture(
                imageBuffer.Get(),
                texture.Get(),
                BufferTextureCopyInfo(0, TextureSubResourceInfo(), UVec3Consts::zero, UVec3(static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1)));
            copyRecorder->ResourceBarrier(Barrier::Transition(texture.Get(), TextureState::copyDst, TextureState::shaderReadOnly));
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

void BaseTexApp::CreateSyncObjects()
{
    imageReadySemaphore = device->CreateSemaphore();
    renderFinishedSemaphore = device->CreateSemaphore();
    frameFence = device->CreateFence(true);
}

int main(int argc, char* argv[])
{
    BaseTexApp application("Rendering-BaseTex");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    return application.RunLoop();
}
