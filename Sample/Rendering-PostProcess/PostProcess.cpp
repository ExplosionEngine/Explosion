//
// Created by johnk on 2024/6/20.
//

#include <Application.h>
#include <RHI/RHI.h>
#include <Render/ShaderCompiler.h>
#include <Rendering/RenderGraph.h>

using namespace Common;
using namespace Render;
using namespace Rendering;
using namespace RHI;

struct Vertex {
    FVec3 position;
    FVec3 color;
};

class TriangleVS : public GlobalShader {
public:
    ShaderInfo(
        "TriangleVS",
        "../Test/Sample/Rendering-PostProcess/Triangle.esl",
        "VSMain",
        RHI::ShaderStageBits::sVertex);

    NonVariant;
    DefaultVariantFilter;
};

class TrianglePS : public GlobalShader {
public:
    ShaderInfo(
        "TrianglePS",
        "../Test/Sample/Rendering-PostProcess/Triangle.esl",
        "PSMain",
        RHI::ShaderStageBits::sPixel);

    NonVariant;
    DefaultVariantFilter;
};

RegisterGlobalShader(TriangleVS);
RegisterGlobalShader(TrianglePS);

class PostProcessApplication final : public Application {
public:
    explicit PostProcessApplication(const std::string& inName);
    ~PostProcessApplication() override;

    void OnCreate() override;
    void OnDrawFrame() override;
    void OnDestroy() override;

private:
    static constexpr size_t backBufferCount = 2;

    void CreateDevice();
    void CompileAllShaders();
    void CreateSwapChain();
    void CreateTriangleVertexBuffer();

    PixelFormat swapChainFormat = PixelFormat::max;
    ShaderInstance triangleVS;
    ShaderInstance trianglePS;
    UniqueRef<Device> device;
    UniqueRef<Surface> surface;
    UniqueRef<SwapChain> swapChain;
    std::array<Texture*, backBufferCount> swapChainTextures {};
    std::array<UniqueRef<TextureView>, backBufferCount> swapChainTextureViews {};
    UniqueRef<Buffer> triangleVertexBuffer;
    UniqueRef<BufferView> triangleVertexBufferView;
};

PostProcessApplication::PostProcessApplication(const std::string& inName)
    : Application(inName)
{
}

PostProcessApplication::~PostProcessApplication() = default;

void PostProcessApplication::OnCreate()
{
    CreateDevice();
    CompileAllShaders();
    CreateSwapChain();
    CreateTriangleVertexBuffer();
}

void PostProcessApplication::OnDrawFrame()
{
    RGBuilder builder(*device);
    // TODO
}

void PostProcessApplication::OnDestroy()
{
    GlobalShaderRegistry::Get().InvalidateAll();
}

void PostProcessApplication::CreateDevice()
{
    device = GetRHIInstance()
        ->GetGpu(0)
        ->RequestDevice(
            DeviceCreateInfo()
                .AddQueueRequest(QueueRequestInfo(QueueType::graphics, 1)));
}

void PostProcessApplication::CompileAllShaders()
{
    ShaderCompileOptions options;
    options.includePaths = { "../Test/Sample/ShaderInclude", "../Test/Sample/Rendering-PostProcess" };
    options.byteCodeType = GetRHIType() == RHI::RHIType::directX12 ? ShaderByteCodeType::dxil : ShaderByteCodeType::spirv;
    options.withDebugInfo = false;
    auto result = ShaderTypeCompiler::Get().CompileGlobalShaderTypes(options);
    const auto& [success, errorInfo] = result.get();
    Assert(success);

    triangleVS = GlobalShaderMap<TriangleVS>::Get().GetShaderInstance(*device, {});
    trianglePS = GlobalShaderMap<TrianglePS>::Get().GetShaderInstance(*device, {});
}

void PostProcessApplication::CreateSwapChain()
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

        swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(
            TextureViewCreateInfo()
                .SetDimension(TextureViewDimension::tv2D)
                .SetMipLevels(0, 1)
                .SetArrayLayers(0, 1)
                .SetAspect(TextureAspect::color)
                .SetType(TextureViewType::colorAttachment));
    }
}

void PostProcessApplication::CreateTriangleVertexBuffer()
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

    const BufferViewCreateInfo bufferViewCreateInfo = BufferViewCreateInfo()
        .SetType(BufferViewType::vertex)
        .SetSize(vertices.size() * sizeof(Vertex))
        .SetOffset(0)
        .SetExtendVertex(sizeof(Vertex));
    triangleVertexBufferView = triangleVertexBuffer->CreateBufferView(bufferViewCreateInfo);
}

int main(int argc, char* argv[])
{
    PostProcessApplication application("Rendering-PostProcess");
    return application.Run(argc, argv);
}
