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

    static void Upload3DTexture(
        RHI::Device& inDevice,
        RHI::Texture& inTexture,
        TextureFormat inFormat,
        uint32_t inWidth,
        uint32_t inHeight,
        uint32_t inDepthOrArraySize,
        uint8_t inMipLevels,
        RHI::TextureAspect inAspect,
        const std::vector<Texture::MipPixels>& inMipsData,
        const std::string& inName)
    {
        std::vector<RHI::TextureSubResourceCopyFootprint> copyFootprints;
        copyFootprints.reserve(inMipLevels);
        for (auto i = 0; i < inMipLevels; i++) {
            copyFootprints.emplace_back(inDevice.GetTextureSubResourceCopyFootprint(inTexture, RHI::TextureSubResourceInfo(i, 0, inAspect)));
        }

        size_t totalBytes = 0;
        for (const auto& copyFootprint : copyFootprints) {
            totalBytes += copyFootprint.totalBytes;
        }

        const Common::UniquePtr<RHI::Buffer> stagingBuffer = inDevice.CreateBuffer(
            RHI::BufferCreateInfo()
                .SetSize(totalBytes)
                .SetUsages(RHI::BufferUsageBits::copySrc | RHI::BufferUsageBits::mapWrite)
                .SetInitialState(RHI::BufferState::staging)
                .SetDebugName(std::format("StagingBuffer-{}", inName)));

        size_t dstMipOffset = 0;
        auto* dstData = stagingBuffer->Map(RHI::MapMode::write, 0, totalBytes);
        for (auto m = 0; m < inMipLevels; m++) {
            const auto& srcPixels = inMipsData[m];
            const auto& dstCopyFootprints = copyFootprints[m];
            const auto srcRowPitch = inWidth * RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(inFormat));
            const auto srcSlicePitch = inWidth * inHeight * RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(inFormat));
            for (auto z = 0; z < inDepthOrArraySize; z++) {
                for (auto y = 0; y < inHeight; y++) {
                    const auto* src = srcPixels.data() + srcSlicePitch * z + srcRowPitch * y; // NOLINT
                    auto* dst = dstData + dstMipOffset + dstCopyFootprints.slicePitch * z + dstCopyFootprints.rowPitch * y; // NOLINT
                    memcpy(dst, src, srcRowPitch);
                }
            }
            dstMipOffset += dstCopyFootprints.totalBytes;
        }
        stagingBuffer->UnMap();

        const Common::UniquePtr<RHI::CommandBuffer> cmdBuffer = inDevice.CreateCommandBuffer();
        const auto recoder = cmdBuffer->Begin();
        {
            const auto passRecoder = recoder->BeginCopyPass();

            dstMipOffset = 0;
            for (auto m = 0; m < inMipLevels; m++) {
                passRecoder->CopyBufferToTexture(
                    stagingBuffer.Get(),
                    &inTexture,
                    RHI::BufferTextureCopyInfo()
                        .SetBufferOffset(dstMipOffset)
                        .SetTextureSubResource(RHI::TextureSubResourceInfo(m, 0, inAspect))
                        .SetTextureOrigin({ 0, 0, 0 })
                        .SetCopyRegion({ inWidth, inHeight, inDepthOrArraySize }));
                dstMipOffset += copyFootprints[m].totalBytes;
            }
            passRecoder->EndPass();
        }
        recoder->End();

        const Common::UniquePtr<RHI::Fence> fence = inDevice.CreateFence(false);
        inDevice.GetQueue(RHI::QueueType::transfer, 0)
            ->Submit(cmdBuffer.Get(), RHI::QueueSubmitInfo().SetSignalFence(fence.Get()));
        fence->Wait();
    }

    static void UploadTextureOrArray(
        RHI::Device& inDevice,
        RHI::Texture& inTexture,
        TextureFormat inFormat,
        uint32_t inWidth,
        uint32_t inHeight,
        uint32_t inDepthOrArraySize,
        uint8_t inMipLevels,
        RHI::TextureAspect inAspect,
        const std::vector<Texture::MipPixels>& inMipsData,
        const std::string& inName)
    {
        std::vector<RHI::TextureSubResourceCopyFootprint> copyFootprints;
        copyFootprints.reserve(inMipLevels * inDepthOrArraySize);

        for (auto m = 0; m < inMipLevels; m++) {
            for (auto a = 0; a < inDepthOrArraySize; a++) {
                copyFootprints.emplace_back(inDevice.GetTextureSubResourceCopyFootprint(inTexture, RHI::TextureSubResourceInfo(m, a, inAspect)));
            }
        }

        // TODO
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

    Texture::MipPixels& Texture::GetMipPixels(uint8_t inMipLevel)
    {
        return mipsData[inMipLevel];
    }

    const Texture::MipPixels& Texture::GetMipPixels(uint8_t inMipLevel) const
    {
        return mipsData[inMipLevel];
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

    void Texture::UpdateMips()
    {
        mipsData.clear();
        mipsData.resize(mipLevels);

        const auto bytesPerPixel = RHI::GetBytesPerPixel(static_cast<RHI::PixelFormat>(format));
        for (auto i = 0; i < mipLevels; i++) {
            mipsData[i].resize(width * height * depthOrArraySize * bytesPerPixel);
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
            mipsData = mipsData,
            name = name]() -> void {
            if (type == TextureType::t3D) {
                Internal::Upload3DTexture(*device, *texturePtr, format, width, height, depthOrArraySize, mipLevels, aspect, mipsData, name);
            } else {
                Internal::UploadTextureOrArray(*device, *texturePtr, format, width, height, depthOrArraySize, mipLevels, aspect, mipsData, name);
            }
        });
    }
}
