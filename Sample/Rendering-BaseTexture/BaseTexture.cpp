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
    FVec3 position;
    FVec2 uv;
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

struct PsUniform {
    FVec3 pixelColor;
};

class BaseTexApp final : public Application {
public:
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
    void CreateSyncObjects();

    PixelFormat swapChainFormat;
    ShaderInstance vs;
    ShaderInstance ps;
    UniquePtr<Device> device;
    UniquePtr<Surface> surface;
    UniquePtr<SwapChain> swapChain;
    std::array<Texture*, backBufferCount> swapChainTextures;
    UniquePtr<Buffer> vertexBuffer;
    UniquePtr<Buffer> indexBuffer;
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
                                .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X3, offsetof(Vertex, position)))))
                .SetFragmentState(
                    RFragmentState()
                        .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all, false))));

        RGBuilder builder(*device);
        auto* backTexture = builder.ImportTexture(swapChainTextures[backTextureIndex], TextureState::present);
        auto* backTextureView = builder.CreateTextureView(backTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
        auto* vBuffer = builder.ImportBuffer(vertexBuffer.Get(), BufferState::shaderReadOnly);
        auto* vBufferView = builder.CreateBufferView(vBuffer, RGBufferViewDesc(BufferViewType::vertex, vBuffer->GetDesc().size, 0, VertexBufferViewInfo(sizeof(Vertex))));
        auto* psUniformBuffer = builder.CreateBuffer(RGBufferDesc(sizeof(PsUniform), BufferUsageBits::uniform | BufferUsageBits::mapWrite, BufferState::staging, "psUniform"));
        auto* psUniformBufferView = builder.CreateBufferView(psUniformBuffer, RGBufferViewDesc(BufferViewType::uniformBinding, sizeof(PsUniform)));

        auto* bindGroup = builder.AllocateBindGroup(
            RGBindGroupDesc::Create(pso->GetPipelineLayout()->GetBindGroupLayout(0))
                .UniformBuffer("psUniform", psUniformBufferView));

        PsUniform psUniform {};
        psUniform.pixelColor = FVec3(
            (std::sin(GetCurrentTimeSeconds()) + 1) / 2,
            (std::cos(GetCurrentTimeSeconds()) + 1) / 2,
            std::abs(std::sin(GetCurrentTimeSeconds())));

        builder.QueueBufferUpload(
            psUniformBuffer,
            RGBufferUploadInfo(&psUniform, sizeof(PsUniform)));

        builder.AddRasterPass(
            "BasePass",
            RGRasterPassDesc()
                .AddColorAttachment(RGColorAttachment(backTextureView, LoadOp::clear, StoreOp::store)),
            { bindGroup },
            [pso, vBufferView, bindGroup, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
                recorder.SetPipeline(pso->GetRHI());
                recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
                recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
                recorder.SetVertexBuffer(0, rg.GetRHI(vBufferView));
                recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
                recorder.SetBindGroup(0, rg.GetRHI(bindGroup));
                recorder.Draw(3, 1, 0, 0);
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
    options.includeDirectories = {"../Test/Sample/ShaderInclude", "../Test/Sample/Rendering-Triangle"};
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
        {{-.5f, -.5f, .0f}, {.0f, 1.0f}},
        {{.5f, -.5f, .0f}, {1.0f, 1.0f}},
        {{.5f, .5f, .0f}, {1.0f, .0f}},
        {{-.5f, .5f, .0f}, {0.f, .0f}},
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
        auto* data = indexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, indices.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();
    }
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
