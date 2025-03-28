//
// Created by johnk on 2025/3/24.
//

#include <Runtime/Asset/Texture.h>

namespace Runtime::Internal {
    static RHI::TextureDimension GetTextureDimension(TextureType inType)
    {
        static std::unordered_map<TextureType, RHI::TextureDimension> map = {
            { TextureType::t1D, RHI::TextureDimension::t1D },
            { TextureType::t2D, RHI::TextureDimension::t2D },
            { TextureType::t2DArray, RHI::TextureDimension::t2D },
            { TextureType::tCube, RHI::TextureDimension::t2D },
            { TextureType::tCubeArray, RHI::TextureDimension::t2D },
            { TextureType::t3D, RHI::TextureDimension::t3D }
        };
        return map.at(inType);
    }

    static bool IsDepthOnlyFormat(TextureFormat inFormat)
    {
        return inFormat == TextureFormat::d16Unorm
            || inFormat == TextureFormat::d32Float;
    }

    static bool IsDepthAndStencilFormat(TextureFormat inFormat)
    {
        return inFormat == TextureFormat::d24UnormS8Uint
            || inFormat == TextureFormat::d32FloatS8Uint;
    }

    static bool IsDepthOrStencilFormat(TextureFormat inFormat)
    {
        return IsDepthOnlyFormat(inFormat)
            || IsDepthAndStencilFormat(inFormat);
    }

    static RHI::TextureAspect GetTextureAspect(TextureFormat inFormat)
    {
        if (IsDepthOnlyFormat(inFormat)) {
            return RHI::TextureAspect::depth;
        }
        if (IsDepthAndStencilFormat(inFormat)) {
            return RHI::TextureAspect::depthStencil;
        }
        return RHI::TextureAspect::color;
    }

    static uint32_t GetSubResourceIndex(uint8_t inMipLevel, uint8_t inArrayLayer, uint8_t inTotalArrayLayer)
    {
        return inMipLevel * inTotalArrayLayer + inArrayLayer; // NOLINT
    }
}

namespace Runtime {
    Texture::Texture(Core::Uri inUri)
        : Asset(std::move(inUri))
        , type(TextureType::max)
        , format(TextureFormat::max)
        , width(1)
        , height(1)
        , depthOrArraySize(1)
        , mipLevels(1)
        , samples(1)
    {
    }

    Texture::~Texture() = default;

    void Texture::PostLoad()
    {
        UpdateRHI();
    }

    TextureType Texture::GetType() const
    {
        return type;
    }

    TextureFormat Texture::GetFormat() const
    {
        return format;
    }

    uint32_t Texture::GetWidth() const
    {
        return width;
    }

    uint32_t Texture::GetHeight() const
    {
        return height;
    }

    uint32_t Texture::GetDepthOrArraySize() const
    {
        return depthOrArraySize;
    }

    uint8_t Texture::GetMipLevels() const
    {
        return mipLevels;
    }

    uint8_t Texture::GetSamples() const
    {
        return samples;
    }

    const std::string& Texture::GetName() const
    {
        return name;
    }

    Texture::Pixels& Texture::GetSubResourcePixels(uint8_t inMipLevel, uint8_t inArrayLayer)
    {
        if (type == TextureType::t3D) {
            Assert(inArrayLayer == 0);
            return subResourcePixelsData[Internal::GetSubResourceIndex(inMipLevel, 0, 1)];
        }
        return subResourcePixelsData[Internal::GetSubResourceIndex(inMipLevel, inArrayLayer, depthOrArraySize)];
    }

    const Texture::Pixels& Texture::GetSubResourcePixels(uint8_t inMipLevel, uint8_t inArrayLayer) const
    {
        if (type == TextureType::t3D) {
            Assert(inArrayLayer == 0);
            return subResourcePixelsData[Internal::GetSubResourceIndex(inMipLevel, 0, 1)];
        }
        return subResourcePixelsData[Internal::GetSubResourceIndex(inMipLevel, inArrayLayer, depthOrArraySize)];
    }

    void Texture::SetType(TextureType inType)
    {
        type = inType;
    }

    void Texture::SetFormat(TextureFormat inFormat)
    {
        format = inFormat;
    }

    void Texture::SetWidth(uint32_t inWidth)
    {
        width = inWidth;
    }

    void Texture::SetHeight(uint32_t inHeight)
    {
        height = inHeight;
    }

    void Texture::SetDepthOrArraySize(uint32_t inDepthOrArraySize)
    {
        depthOrArraySize = inDepthOrArraySize;
    }

    void Texture::SetMipLevels(uint8_t inMipLevels)
    {
        mipLevels = inMipLevels;
    }

    void Texture::SetSamples(uint8_t inSamples)
    {
        samples = inSamples;
    }

    void Texture::SetName(const std::string& inName)
    {
        name = inName;
    }

    RHI::Texture* Texture::GetRHI() const
    {
        return texture.Get();
    }

    RHI::TextureView* Texture::GetViewRHI() const
    {
        return textureView.Get();
    }

    void Texture::UpdateMips()
    {
        const auto arraySize = type == TextureType::t3D ? 1 : depthOrArraySize;
        const auto bytesPerPixel = RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(format));

        subResourcePixelsData.clear();
        subResourcePixelsData.resize(mipLevels * arraySize);

        for (auto m = 0; m < mipLevels; m++) {
            const auto mipWidth = std::max(width >> m, 1u);
            const auto mipHeight = std::max(height >> m, 1u);

            for (auto a = 0; a < arraySize; a++) {
                subResourcePixelsData[Internal::GetSubResourceIndex(m, a, arraySize)].resize(mipWidth * mipHeight * bytesPerPixel);
            }
        }
    }

    void Texture::UpdateRHI()
    {
        const auto& renderModule = EngineHolder::Get().GetRenderModule();
        auto* device = renderModule.GetDevice();

        texture = device->CreateTexture(
            RHI::TextureCreateInfo()
                .SetDimension(Internal::GetTextureDimension(type))
                .SetWidth(width)
                .SetHeight(height)
                .SetDepthOrArraySize(depthOrArraySize)
                .SetFormat(static_cast<RHI::PixelFormat>(format))
                .SetUsages(RHI::TextureUsageBits::copyDst | RHI::TextureUsageBits::textureBinding)
                .SetMipLevels(mipLevels)
                .SetSamples(samples)
                .SetInitialState(RHI::TextureState::shaderReadOnly)
                .SetDebugName(name));

        textureView = texture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetType(Internal::IsDepthOrStencilFormat(format) ? RHI::TextureViewType::depthStencil : RHI::TextureViewType::textureBinding)
                .SetDimension(static_cast<RHI::TextureViewDimension>(type))
                .SetAspect(Internal::GetTextureAspect(format))
                .SetMipLevels(0, mipLevels)
                .SetArrayLayers(0, type == TextureType::t3D ? 1 : depthOrArraySize));

        renderModule.GetRenderThread().EmplaceTask([
            device,
            texturePtr = texture.Get(),
            type = type,
            format = format,
            width = width,
            height = height,
            depthOrArraySize = depthOrArraySize,
            mipLevels = mipLevels,
            aspect = Internal::GetTextureAspect(format),
            subResourcePixelsData = subResourcePixelsData,
            name = name
        ]() -> void {
            const auto arraySize = type == TextureType::t3D ? 1 : depthOrArraySize;
            const auto depth = type == TextureType::t3D ? depthOrArraySize : 1;

            std::vector<RHI::TextureSubResourceCopyFootprint> copyFootprints;
            copyFootprints.reserve(mipLevels * arraySize);
            for (auto m = 0; m < mipLevels; m++) {
                for (auto a = 0; a < arraySize; a++) {
                    copyFootprints.emplace_back(device->GetTextureSubResourceCopyFootprint(*texturePtr, RHI::TextureSubResourceInfo(m, a, aspect)));
                }
            }

            size_t totalBytes = 0;
            for (const auto& copyFootprint : copyFootprints) {
                totalBytes += copyFootprint.totalBytes;
            }

            const Common::UniquePtr<RHI::Buffer> stagingBuffer = device->CreateBuffer(
                RHI::BufferCreateInfo()
                    .SetSize(totalBytes)
                    .SetUsages(RHI::BufferUsageBits::copySrc | RHI::BufferUsageBits::mapWrite)
                    .SetInitialState(RHI::BufferState::staging)
                    .SetDebugName(std::format("StagingBuffer-{}", name)));

            const auto srcRowPitch = width * RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(format));
            const auto srcSlicePitch = width * height * RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(format));

            size_t dstSubResourceOffset = 0;
            auto* dstData = static_cast<uint8_t*>(stagingBuffer->Map(RHI::MapMode::write, 0, totalBytes));
            for (auto m = 0; m < mipLevels; m++) {
                for (auto a = 0; a < arraySize; a++) {
                    const auto subResourceIndex = Internal::GetSubResourceIndex(m, a, arraySize);
                    const auto& srcPixels = subResourcePixelsData[subResourceIndex];
                    const auto& dstCopyFootprint = copyFootprints[subResourceIndex];

                    for (auto z = 0; z < depthOrArraySize; z++) {
                        for (auto y = 0; y < height; y++) {
                            const auto* src = srcPixels.data() + srcSlicePitch * z + srcRowPitch * y;
                            auto* dst = dstData + dstSubResourceOffset + dstCopyFootprint.slicePitch * z + dstCopyFootprint.rowPitch * y;
                            memcpy(dst, src, srcRowPitch);
                        }
                    }
                    dstSubResourceOffset += dstCopyFootprint.totalBytes;
                }
            }
            stagingBuffer->UnMap();

            const Common::UniquePtr<RHI::CommandBuffer> cmdBuffer = device->CreateCommandBuffer();
            const auto recoder = cmdBuffer->Begin();
            {
                const auto passRecoder = recoder->BeginCopyPass();
                {
                    dstSubResourceOffset = 0;
                    for (auto m = 0; m < mipLevels; m++) {
                        for (auto a = 0; a < arraySize; a++) {
                            const auto subResourceIndex = Internal::GetSubResourceIndex(m, a, arraySize);
                            passRecoder->CopyBufferToTexture(
                                stagingBuffer.Get(),
                                texturePtr,
                                RHI::BufferTextureCopyInfo()
                                    .SetBufferOffset(dstSubResourceOffset)
                                    .SetTextureSubResource(RHI::TextureSubResourceInfo(m, a, aspect))
                                    .SetTextureOrigin({ 0, 0, 0 })
                                    .SetCopyRegion({ width, height, depth }));
                            dstSubResourceOffset += copyFootprints[subResourceIndex].totalBytes;
                        }
                    }
                }
                passRecoder->EndPass();
            }
            recoder->End();

            const Common::UniquePtr<RHI::Fence> fence = device->CreateFence(false);
            device
                ->GetQueue(RHI::QueueType::transfer, 0)
                ->Submit(cmdBuffer.Get(), RHI::QueueSubmitInfo().SetSignalFence(fence.Get()));
            fence->Wait();
        });
    }

    RenderTarget::RenderTarget(Core::Uri inUri)
        : Asset(std::move(inUri))
        , type(TextureType::max)
        , format(TextureFormat::max)
        , width(1)
        , height(1)
        , depthOrArraySize(1)
        , mipLevels(1)
        , samples(1)
    {
    }

    RenderTarget::~RenderTarget() = default;

    void RenderTarget::PostLoad()
    {
        UpdateRHI();
    }

    TextureType RenderTarget::GetType() const
    {
        return type;
    }

    TextureFormat RenderTarget::GetFormat() const
    {
        return format;
    }

    uint32_t RenderTarget::GetWidth() const
    {
        return width;
    }

    uint32_t RenderTarget::GetHeight() const
    {
        return height;
    }

    uint32_t RenderTarget::GetDepthOrArraySize() const
    {
        return depthOrArraySize;
    }

    uint8_t RenderTarget::GetMipLevels() const
    {
        return mipLevels;
    }

    uint8_t RenderTarget::GetSamples() const
    {
        return samples;
    }

    const std::string& RenderTarget::GetName() const
    {
        return name;
    }

    void RenderTarget::SetType(TextureType inType)
    {
        type = inType;
    }

    void RenderTarget::SetFormat(TextureFormat inFormat)
    {
        format = inFormat;
    }

    void RenderTarget::SetWidth(uint32_t inWidth)
    {
        width = inWidth;
    }

    void RenderTarget::SetHeight(uint32_t inHeight)
    {
        height = inHeight;
    }

    void RenderTarget::SetDepthOrArraySize(uint32_t inDepthOrArraySize)
    {
        depthOrArraySize = inDepthOrArraySize;
    }

    void RenderTarget::SetMipLevels(uint8_t inMipLevels)
    {
        mipLevels = inMipLevels;
    }

    void RenderTarget::SetSamples(uint8_t inSamples)
    {
        samples = inSamples;
    }

    void RenderTarget::SetName(const std::string& inName)
    {
        name = inName;
    }

    RHI::Texture* RenderTarget::GetRHI() const
    {
        return texture.Get();
    }

    RHI::TextureView* RenderTarget::GetRenderTargetViewRHI() const
    {
        return renderTargetView.Get();
    }

    RHI::TextureView* RenderTarget::GetShaderResourceViewRHI() const
    {
        return shaderResourceView.Get();
    }

    void RenderTarget::UpdateRHI()
    {
        const auto& renderModule = EngineHolder::Get().GetRenderModule();
        auto* device = renderModule.GetDevice();

        texture = device->CreateTexture(
            RHI::TextureCreateInfo()
                .SetDimension(Internal::GetTextureDimension(type))
                .SetWidth(width)
                .SetHeight(height)
                .SetDepthOrArraySize(depthOrArraySize)
                .SetFormat(static_cast<RHI::PixelFormat>(format))
                .SetUsages(RHI::TextureUsageBits::textureBinding | (Internal::IsDepthOrStencilFormat(format) ? RHI::TextureUsageBits::depthStencilAttachment : RHI::TextureUsageBits::renderAttachment))
                .SetMipLevels(mipLevels)
                .SetSamples(samples)
                .SetInitialState(RHI::TextureState::shaderReadOnly)
                .SetDebugName(name));

        renderTargetView = texture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetType(Internal::IsDepthOrStencilFormat(format) ? RHI::TextureViewType::depthStencil : RHI::TextureViewType::colorAttachment)
                .SetDimension(static_cast<RHI::TextureViewDimension>(type))
                .SetAspect(Internal::GetTextureAspect(format))
                .SetMipLevels(0, mipLevels)
                .SetArrayLayers(0, type == TextureType::t3D ? 1 : depthOrArraySize));

        shaderResourceView = texture->CreateTextureView(
            RHI::TextureViewCreateInfo()
                .SetType(Internal::IsDepthOrStencilFormat(format) ? RHI::TextureViewType::depthStencil : RHI::TextureViewType::textureBinding)
                .SetDimension(static_cast<RHI::TextureViewDimension>(type))
                .SetAspect(Internal::GetTextureAspect(format))
                .SetMipLevels(0, mipLevels)
                .SetArrayLayers(0, type == TextureType::t3D ? 1 : depthOrArraySize));
    }
}
