#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include <Common/Utility.h>
#include <RHI/RHI.h>
#include <Render/RenderGraph.h>

class SampleRenderTarget {
public:
    struct Frame {
        RHI::Texture* texture;
        RHI::TextureState initialState;
        uint8_t textureIndex;
    };

    NonCopyable(SampleRenderTarget)
    virtual ~SampleRenderTarget();

    virtual void Initialize() = 0;
    virtual Frame Acquire() = 0;
    virtual void FinishRenderPass(const Render::RGBuilder& builder, RHI::CommandRecorder& recorder, Render::RGTextureRef outputTexture) const = 0;
    virtual void PrepareForSubmit(Render::RGBuilder& builder, Render::RGTextureRef outputTexture) = 0;
    virtual void Execute(Render::RGBuilder& builder, const Frame& frame) = 0;
    virtual RHI::PixelFormat GetFormat() const = 0;

protected:
    SampleRenderTarget(RHI::Device& inDevice, uint32_t inWidth, uint32_t inHeight);

    RHI::Device& device;
    uint32_t width;
    uint32_t height;
};

class SwapChainRenderTarget final : public SampleRenderTarget {
public:
    NonCopyable(SwapChainRenderTarget)
    SwapChainRenderTarget(RHI::Device& inDevice, uint32_t inWidth, uint32_t inHeight, void* inNativeWindowHandle);
    ~SwapChainRenderTarget() override;

    void Initialize() override;
    Frame Acquire() override;
    void FinishRenderPass(const Render::RGBuilder& builder, RHI::CommandRecorder& recorder, Render::RGTextureRef outputTexture) const override;
    void PrepareForSubmit(Render::RGBuilder& builder, Render::RGTextureRef outputTexture) override;
    void Execute(Render::RGBuilder& builder, const Frame& frame) override;
    RHI::PixelFormat GetFormat() const override;

private:
    static constexpr size_t backBufferCount = 2;

    RHI::PixelFormat format;
    Common::UniquePtr<RHI::Surface> surface;
    Common::UniquePtr<RHI::SwapChain> swapChain;
    std::array<RHI::Texture*, backBufferCount> textures;
    std::array<RHI::TextureState, backBufferCount> textureStates;
    Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
    std::array<Common::UniquePtr<RHI::Semaphore>, backBufferCount> renderFinishedSemaphores;
    Common::UniquePtr<RHI::Fence> frameFence;
};

class HeadlessRenderTarget final : public SampleRenderTarget {
public:
    NonCopyable(HeadlessRenderTarget)
    HeadlessRenderTarget(RHI::Device& inDevice, uint32_t inWidth, uint32_t inHeight, std::string inOutputPath);
    ~HeadlessRenderTarget() override;

    void Initialize() override;
    Frame Acquire() override;
    void FinishRenderPass(const Render::RGBuilder& builder, RHI::CommandRecorder& recorder, Render::RGTextureRef outputTexture) const override;
    void PrepareForSubmit(Render::RGBuilder& builder, Render::RGTextureRef outputTexture) override;
    void Execute(Render::RGBuilder& builder, const Frame& frame) override;
    RHI::PixelFormat GetFormat() const override;

private:
    void SaveOutput() const;

    std::string outputPath;
    Common::UniquePtr<RHI::Texture> texture;
    RHI::TextureState textureState;
    RHI::TextureSubResourceCopyFootprint copyFootprint;
    Common::UniquePtr<RHI::Buffer> readbackBuffer;
    Common::UniquePtr<RHI::Fence> frameFence;
};

bool IsSupportedSampleImageOutputPath(const std::string& path);
