//
// Created by johnk on 4/1/2022.
//

#include <RHI/DirectX12/Image.h>

namespace RHI::DirectX12 {
    DX12Image::DX12Image(DX12DeviceMemory* dm) : Image(), deviceMemory(dm) {}

    DX12Image::DX12Image(const ImageCreateInfo* createInfo) : Image(createInfo), deviceMemory(nullptr) {}

    DX12Image::~DX12Image() = default;
}
