//
// Created by johnk on 2022/1/23.
//

#include <RHI/Texture.h>

namespace RHI {
    TextureCreateInfo::TextureCreateInfo()
        : dimension(TextureDimension::max)
        , width(0)
        , height(0)
        , depthOrArraySize(0)
        , format(PixelFormat::max)
        , usages(TextureUsageFlags::null)
        , mipLevels(0)
        , samples(1)
        , initialState(TextureState::max)
        , debugName()
    {
    }

    TextureCreateInfo& TextureCreateInfo::SetDimension(TextureDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetWidth(uint32_t inWidth)
    {
        width = inWidth;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetHeight(uint32_t inHeight)
    {
        height = inHeight;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetDepthOrArraySize(uint32_t inDepthOrArraySize)
    {
        depthOrArraySize = inDepthOrArraySize;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetFormat(PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetUsages(TextureUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetMipLevels(uint8_t inMipLevels)
    {
        mipLevels = inMipLevels;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetSamples(uint8_t inSamples)
    {
        samples = inSamples;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetInitialState(TextureState inState)
    {
        initialState = inState;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetDebugName(std::string inDebugName)
    {
        debugName = std::move(inDebugName);
        return *this;
    }

    bool TextureCreateInfo::operator==(const TextureCreateInfo& rhs) const
    {
        return dimension == rhs.dimension
            && width == rhs.width
            && height == rhs.height
            && depthOrArraySize == rhs.depthOrArraySize
            && format == rhs.format
            && usages == rhs.usages
            && mipLevels == rhs.mipLevels
            && samples == rhs.samples
            && initialState == rhs.initialState;
    }

    Texture::Texture(const TextureCreateInfo& inCreateInfo)
        : createInfo(inCreateInfo)
    {
    }

    Texture::~Texture() = default;

    const TextureCreateInfo& Texture::GetCreateInfo() const
    {
        return createInfo;
    }
}
