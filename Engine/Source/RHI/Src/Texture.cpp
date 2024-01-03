//
// Created by johnk on 2022/1/23.
//

#include <RHI/Texture.h>

namespace RHI {
    Texture::Texture() = default;

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
