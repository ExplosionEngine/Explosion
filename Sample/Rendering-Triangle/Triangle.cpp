//
// Created by johnk on 2024/6/20.
//

#include <Application.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Render/RenderingCache.h>
#include <Render/RenderGraph.h>

using namespace Common;
using namespace Render;
using namespace RHI;

struct Vertex {
    FVec3 position;
    FVec3 color;
};

class TriangleVS : public GlobalShader {
public:
    ShaderInfo(
        "TriangleVS",
        "../Test/Sample/Rendering-Triangle/Triangle.esl",
        "VSMain",
        RHI::ShaderStageBits::sVertex);

    NonVariant;
    DefaultVariantFilter;
};

class TrianglePS : public GlobalShader {
public:
    ShaderInfo(
        "TrianglePS",
        "../Test/Sample/Rendering-Triangle/Triangle.esl",
        "PSMain",
        RHI::ShaderStageBits::sPixel);

    NonVariant;
    DefaultVariantFilter;
};

RegisterGlobalShader(TriangleVS);
RegisterGlobalShader(TrianglePS);

class TriangleApplication final : public Application {
public:
    explicit TriangleApplication(const std::string& inName);
    ~TriangleApplication() override;

    void OnCreate() override;
    void OnDrawFrame() override;
    void OnDestroy() override;

private:
    static constexpr size_t backBufferCount = 2;

    void CreateDevice();
    void CompileAllShaders();
    void CreateSwapChain();
    void CreateTriangleVertexBuffer();
    void CreateSyncObjects();

    PixelFormat swapChainFormat = PixelFormat::max;
    ShaderInstance triangleVS;
    ShaderInstance trianglePS;
    UniqueRef<Device> device;
    UniqueRef<Surface> surface;
    UniqueRef<SwapChain> swapChain;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    UniqueRef<Buffer> triangleVertexBuffer;
    UniqueRef<Semaphore> imageReadySemaphore;
    UniqueRef<Semaphore> renderFinishedSemaphore;
    UniqueRef<Fence> frameFence;
};

TriangleApplication::TriangleApplication(const std::string& inName)
    : Application(inName)
{
}

TriangleApplication::~TriangleApplication() = default;

void TriangleApplication::OnCreate()
{
    CreateDevice();
    CompileAllShaders();
    CreateSwapChain();
    CreateTriangleVertexBuffer();
    CreateSyncObjects();
}

void TriangleApplication::OnDrawFrame()
{
    frameFence->Reset();
    const auto backTextureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());

    auto* pso = PipelineCache::Get(*device).GetOrCreate(
        RasterPipelineStateDesc()
                .SetVertexShader(triangleVS)
                .SetPixelShader(trianglePS)
                .SetVertexState(
                    RVertexState()
                        .AddVertexBufferLayout(
                            RVertexBufferLayout(VertexStepMode::perVertex, sizeof(Vertex))
                                .AddAttribute(RVertexAttribute(RVertexBinding("POSITION", 0), VertexFormat::float32X3, offsetof(Vertex, position)))
                                .AddAttribute(RVertexAttribute(RVertexBinding("COLOR", 0), VertexFormat::float32X3, offsetof(Vertex, color)))))
                .SetFragmentState(
                    RFragmentState()
                        .AddColorTarget(ColorTargetState(swapChainFormat, ColorWriteBits::all, false))));

    RGBuilder builder(*device);
    auto* backTexture = builder.ImportTexture(swapChainTextures[backTextureIndex], TextureState::present);
    auto* backTextureView = builder.CreateTextureView(backTexture, RGTextureViewDesc(TextureViewType::colorAttachment, TextureViewDimension::tv2D));
    auto* vertexBuffer = builder.ImportBuffer(triangleVertexBuffer.Get(), BufferState::shaderReadOnly);
    auto* vertexBufferView = builder.CreateBufferView(vertexBuffer, RGBufferViewDesc(BufferViewType::vertex, vertexBuffer->GetDesc().size, 0, VertexBufferViewInfo(sizeof(Vertex))));

    builder.AddRasterPass(
        "BasePass",
        RGRasterPassDesc()
            .AddColorAttachment(RGColorAttachment(backTextureView, LoadOp::load, StoreOp::store)),
        {},
        [pso, vertexBufferView, backTexture, viewportWidth = GetWindowWidth(), viewportHeight = GetWindowHeight()](const RGBuilder& rg, RasterPassCommandRecorder& recorder) -> void {
            recorder.SetPipeline(pso->GetRHI());
            recorder.SetScissor(0, 0, viewportWidth, viewportHeight);
            recorder.SetViewport(0, 0, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0, 1);
            recorder.SetVertexBuffer(0, rg.GetRHI(vertexBufferView));
            recorder.SetPrimitiveTopology(PrimitiveTopology::triangleList);
            recorder.Draw(3, 1, 0, 0);
            recorder.ResourceBarrier(Barrier::Transition(rg.GetRHI(backTexture), TextureState::renderTarget, TextureState::present));
        });

    RGExecuteInfo executeInfo;
    executeInfo.semaphoresToWait = { imageReadySemaphore.Get() };
    executeInfo.semaphoresToSignal = { renderFinishedSemaphore.Get() };
    executeInfo.inFenceToSignal = frameFence.Get();
    builder.Execute(executeInfo);
    swapChain->Present(renderFinishedSemaphore.Get());
    frameFence->Wait();
}

void TriangleApplication::OnDestroy()
{
    const UniqueRef<Fence> fence = device->CreateFence(false);
    device->GetQueue(QueueType::graphics, 0)->Flush(fence.Get());
    fence->Wait();

    ResourceViewCache::Get(*device).Invalidate();
    PipelineCache::Get(*device).Invalidate();
    GlobalShaderRegistry::Get().InvalidateAll();
}

void TriangleApplication::CreateDevice()
{
    device = GetRHIInstance()
        ->GetGpu(0)
        ->RequestDevice(
            DeviceCreateInfo()
                .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
}

void TriangleApplication::CompileAllShaders()
{
    ShaderCompileOptions options;
    options.includePaths = { "../Test/Sample/ShaderInclude", "../Test/Sample/Rendering-Triangle" };
    options.byteCodeType = GetRHIType() == RHI::RHIType::directX12 ? ShaderByteCodeType::dxil : ShaderByteCodeType::spirv;
    options.withDebugInfo = false;
    auto result = ShaderTypeCompiler::Get().CompileGlobalShaderTypes(options);
    const auto& [success, errorInfo] = result.get();
    Assert(success);

    triangleVS = GlobalShaderMap<TriangleVS>::Get().GetShaderInstance(*device, {});
    trianglePS = GlobalShaderMap<TrianglePS>::Get().GetShaderInstance(*device, {});
}

void TriangleApplication::CreateSwapChain()
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

void TriangleApplication::CreateTriangleVertexBuffer()
{
    const std::vector<Vertex> vertices = {
        {{-.5f, -.5f, 0.f}, {1.f, 0.f, 0.f}},
        {{.5f, -.5f, 0.f}, {0.f, 1.f, 0.f}},
        {{0.f, .5f, 0.f}, {0.f, 0.f, 1.f}},
    };

    const BufferCreateInfo bufferCreateInfo = BufferCreateInfo()
        .SetSize(vertices.size() * sizeof(Vertex))
        .SetUsages(BufferUsageBits::vertex | BufferUsageBits::mapWrite | BufferUsageBits::copySrc)
        .SetInitialState(BufferState::staging)
        .SetDebugName("vertexBuffer");

    triangleVertexBuffer = device->CreateBuffer(bufferCreateInfo);
    if (triangleVertexBuffer != nullptr) {
        auto* data = triangleVertexBuffer->Map(MapMode::write, 0, bufferCreateInfo.size);
        memcpy(data, vertices.data(), bufferCreateInfo.size);
        triangleVertexBuffer->UnMap();
    }
}

void TriangleApplication::CreateSyncObjects()
{
    imageReadySemaphore = device->CreateSemaphore();
    renderFinishedSemaphore = device->CreateSemaphore();
    frameFence = device->CreateFence(true);
}

int main(int argc, char* argv[])
{
    TriangleApplication application("Rendering-PostProcess");
    if (!application.Initialize(argc, argv)) {
        return -1;
    }
    return application.RunLoop();
}
