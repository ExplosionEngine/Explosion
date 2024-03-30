//
// Created by johnk on 2022/1/23.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>
#include <string>

namespace RHI {
    struct TextureViewCreateInfo;
    class TextureView;
    class Device;

    struct TextureCreateInfo {
        TextureDimension dimension;
        Common::UVec3 extent;
        PixelFormat format;
        TextureUsageFlags usages;
        uint8_t mipLevels;
        uint8_t samples;
        TextureState initialState;
        std::string debugName;

        TextureCreateInfo();
        TextureCreateInfo& Dimension(TextureDimension inDimension);
        TextureCreateInfo& Extent(const Common::UVec3& inExtent);
        TextureCreateInfo& Format(PixelFormat inFormat);
        TextureCreateInfo& Usages(TextureUsageFlags inUsages);
        TextureCreateInfo& MipLevels(uint8_t inMipLevels);
        TextureCreateInfo& Samples(uint8_t inSamples);
        TextureCreateInfo& InitialState(TextureState inState);
        TextureCreateInfo& DebugName(std::string inDebugName);

        bool operator==(const TextureCreateInfo& rhs) const;
    };

    class Texture {
    public:
        NonCopyable(Texture)
        virtual ~Texture();

        const TextureCreateInfo& GetCreateInfo() const;
        virtual TextureView* CreateTextureView(const TextureViewCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

    protected:
        Texture();
        explicit Texture(const TextureCreateInfo& inCreateInfo);

        TextureCreateInfo createInfo;
    };
}
