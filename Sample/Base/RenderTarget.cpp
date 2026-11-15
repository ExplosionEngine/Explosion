#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <RenderTarget.h>

using namespace Common;
using namespace Render;
using namespace RHI;

namespace Sample::Internal {
    static std::string GetLowercaseExtension(const std::string& path)
    {
        auto extension = std::filesystem::path(path).extension().string();
        std::ranges::transform(extension, extension.begin(), [](const unsigned char character) -> char { return static_cast<char>(std::tolower(character)); });
        return extension;
    }

    static int WriteImage(const std::string& path, const int width, const int height, const uint8_t* pixels)
    {
        const auto extension = GetLowercaseExtension(path);
        if (extension == ".png") {
            return stbi_write_png(path.c_str(), width, height, 4, pixels, width * 4);
        }
        if (extension == ".bmp") {
            return stbi_write_bmp(path.c_str(), width, height, 4, pixels);
        }
        if (extension == ".tga") {
            return stbi_write_tga(path.c_str(), width, height, 4, pixels);
        }
        if (extension == ".jpg" || extension == ".jpeg") {
            return stbi_write_jpg(path.c_str(), width, height, 4, pixels, 90);
        }
        return 0;
    }
}

SampleRenderTarget::SampleRenderTarget(Device& inDevice, const uint32_t inWidth, const uint32_t inHeight)
    : device(inDevice)
    , width(inWidth)
    , height(inHeight)
{
}

SampleRenderTarget::~SampleRenderTarget() = default;

SwapChainRenderTarget::SwapChainRenderTarget(Device& inDevice, const uint32_t inWidth, const uint32_t inHeight, void* inNativeWindowHandle)
    : SampleRenderTarget(inDevice, inWidth, inHeight)
    , format(PixelFormat::max)
    , surface(device.CreateSurface(SurfaceCreateInfo(inNativeWindowHandle)))
    , textures()
    , textureStates()
{
}

SwapChainRenderTarget::~SwapChainRenderTarget() = default;

void SwapChainRenderTarget::Initialize()
{
    static const std::array formatQualifiers = {
        PixelFormat::rgba8Unorm,
        PixelFormat::bgra8Unorm
    };

    for (const auto candidate : formatQualifiers) {
        if (device.CheckSwapChainFormatSupport(surface.Get(), candidate, ColorSpace::srgbNonLinear)) {
            format = candidate;
            break;
        }
    }
    Assert(format != PixelFormat::max);

    swapChain = device.CreateSwapChain(
        SwapChainCreateInfo()
            .SetFormat(format)
            .SetPresentMode(PresentMode::immediately)
            .SetTextureNum(backBufferCount)
            .SetWidth(width)
            .SetHeight(height)
            .SetSurface(surface.Get())
            .SetPresentQueue(device.GetQueue(QueueType::graphics, 0)));

    for (auto i = 0; i < backBufferCount; i++) {
        textures[i] = swapChain->GetTexture(i);
        textureStates[i] = textures[i]->GetCreateInfo().initialState;
        renderFinishedSemaphores[i] = device.CreateSemaphore();
    }
    imageReadySemaphore = device.CreateSemaphore();
    frameFence = device.CreateFence(true);
}

SampleRenderTarget::Frame SwapChainRenderTarget::Acquire()
{
    frameFence->Reset();
    const auto textureIndex = swapChain->AcquireBackTexture(imageReadySemaphore.Get());
    return { textures[textureIndex], textureStates[textureIndex], textureIndex };
}

void SwapChainRenderTarget::FinishRenderPass(const RGBuilder& builder, CommandRecorder& recorder, const RGTextureRef outputTexture) const
{
    recorder.ResourceBarrier(Barrier::Transition(builder.GetRHI(outputTexture), TextureState::renderTarget, TextureState::present));
}

void SwapChainRenderTarget::PrepareForSubmit(RGBuilder&, RGTextureRef)
{
}

void SwapChainRenderTarget::Execute(RGBuilder& builder, const Frame& frame)
{
    RGExecuteInfo executeInfo;
    executeInfo.semaphoresToWait = { imageReadySemaphore.Get() };
    executeInfo.semaphoresToSignal = { renderFinishedSemaphores[frame.textureIndex].Get() };
    executeInfo.inFenceToSignal = frameFence.Get();
    builder.Execute(executeInfo);

    swapChain->Present(renderFinishedSemaphores[frame.textureIndex].Get());
    textureStates[frame.textureIndex] = TextureState::present;
    frameFence->Wait();
}

PixelFormat SwapChainRenderTarget::GetFormat() const
{
    return format;
}

HeadlessRenderTarget::HeadlessRenderTarget(Device& inDevice, const uint32_t inWidth, const uint32_t inHeight, std::string inOutputPath)
    : SampleRenderTarget(inDevice, inWidth, inHeight)
    , outputPath(std::move(inOutputPath))
    , textureState(TextureState::undefined)
    , copyFootprint()
{
}

HeadlessRenderTarget::~HeadlessRenderTarget() = default;

void HeadlessRenderTarget::Initialize()
{
    texture = device.CreateTexture(
        TextureCreateInfo()
            .SetType(TextureType::t2D)
            .SetWidth(width)
            .SetHeight(height)
            .SetDepthOrArraySize(1)
            .SetFormat(GetFormat())
            .SetUsages(TextureUsageBits::renderAttachment | TextureUsageBits::copySrc)
            .SetMipLevels(1)
            .SetSamples(1)
            .SetInitialState(TextureState::renderTarget)
            .SetDebugName("HeadlessOutput"));
    textureState = TextureState::renderTarget;
    copyFootprint = device.GetTextureSubResourceCopyFootprint(*texture, TextureSubResourceInfo());
    readbackBuffer = device.CreateBuffer(
        BufferCreateInfo()
            .SetSize(copyFootprint.totalBytes)
            .SetUsages(BufferUsageBits::mapRead | BufferUsageBits::copyDst)
            .SetInitialState(BufferState::copyDst)
            .SetDebugName("HeadlessOutputReadback"));
    frameFence = device.CreateFence(true);
}

SampleRenderTarget::Frame HeadlessRenderTarget::Acquire()
{
    frameFence->Reset();
    return { texture.Get(), textureState, 0 };
}

void HeadlessRenderTarget::FinishRenderPass(const RGBuilder&, CommandRecorder&, RGTextureRef) const
{
}

void HeadlessRenderTarget::PrepareForSubmit(RGBuilder& builder, const RGTextureRef outputTexture)
{
    auto* readback = builder.ImportBuffer(readbackBuffer.Get(), BufferState::copyDst);
    RGCopyPassDesc copyDesc;
    copyDesc.copySrcs = { outputTexture };
    copyDesc.copyDsts = { readback };
    builder.AddCopyPass(
        "ReadbackOutput",
        copyDesc,
        [outputTexture, readback, copyRegion = UVec3(width, height, 1), rowPitch = copyFootprint.rowPitch, slicePitch = copyFootprint.slicePitch](const RGBuilder& rg, CopyPassCommandRecorder& recorder) -> void {
            recorder.CopyTextureToBuffer(rg.GetRHI(outputTexture), rg.GetRHI(readback), BufferTextureCopyInfo(0, TextureSubResourceInfo(), UVec3Consts::zero, copyRegion, rowPitch, slicePitch));
        });
}

void HeadlessRenderTarget::Execute(RGBuilder& builder, const Frame&)
{
    RGExecuteInfo executeInfo;
    executeInfo.inFenceToSignal = frameFence.Get();
    builder.Execute(executeInfo);

    textureState = TextureState::copySrc;
    frameFence->Wait();
    SaveOutput();
}

PixelFormat HeadlessRenderTarget::GetFormat() const
{
    return PixelFormat::rgba8Unorm;
}

void HeadlessRenderTarget::SaveOutput() const
{
    const auto rowSize = width * copyFootprint.bytesPerPixel;
    std::vector<uint8_t> pixels(rowSize * height);
    const auto* mapped = static_cast<const uint8_t*>(readbackBuffer->Map(MapMode::read, 0, copyFootprint.totalBytes));
    for (uint32_t row = 0; row < height; row++) {
        std::memcpy(pixels.data() + row * rowSize, mapped + row * copyFootprint.rowPitch, rowSize);
    }
    readbackBuffer->Unmap();

    const std::filesystem::path path(outputPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    Assert(Sample::Internal::WriteImage(outputPath, static_cast<int>(width), static_cast<int>(height), pixels.data()) != 0);
    std::cout << "Saved headless output to " << std::filesystem::absolute(path).string() << std::endl;
}

bool IsSupportedSampleImageOutputPath(const std::string& path)
{
    const auto extension = Sample::Internal::GetLowercaseExtension(path);
    return extension == ".png" || extension == ".bmp" || extension == ".tga" || extension == ".jpg" || extension == ".jpeg";
}
