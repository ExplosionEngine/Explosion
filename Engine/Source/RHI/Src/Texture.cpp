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
    {
    }

    TextureCreateInfo& TextureCreateInfo::SetDimension(const TextureDimension inDimension)
    {
        dimension = inDimension;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetWidth(const uint32_t inWidth)
    {
        width = inWidth;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetHeight(const uint32_t inHeight)
    {
        height = inHeight;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetDepthOrArraySize(const uint32_t inDepthOrArraySize)
    {
        depthOrArraySize = inDepthOrArraySize;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetFormat(const PixelFormat inFormat)
    {
        format = inFormat;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetUsages(const TextureUsageFlags inUsages)
    {
        usages = inUsages;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetMipLevels(const uint8_t inMipLevels)
    {
        mipLevels = inMipLevels;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetSamples(const uint8_t inSamples)
    {
        samples = inSamples;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetInitialState(const TextureState inState)
    {
        initialState = inState;
        return *this;
    }

    TextureCreateInfo& TextureCreateInfo::SetDebugName(std::string inDebugName)
    {
        debugName = std::move(inDebugName);
        return *this;
    }

    uint64_t TextureCreateInfo::Hash() const
    {
        const std::vector<uint64_t> values = {
            static_cast<uint64_t>(dimension),
            static_cast<uint64_t>(width),
            static_cast<uint64_t>(height),
            static_cast<uint64_t>(depthOrArraySize),
            static_cast<uint64_t>(format),
            static_cast<uint64_t>(usages.Value()),
            static_cast<uint64_t>(mipLevels),
            static_cast<uint64_t>(samples),
            static_cast<uint64_t>(initialState),
        };
        return Common::HashUtils::CityHash(values.data(), values.size() * sizeof(uint64_t));
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
