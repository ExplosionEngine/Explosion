//
// Created by johnk on 4/1/2022.
//

#include <RHI/DirectX12/Image.h>

namespace RHI::DirectX12 {
    DX12Image::DX12Image(DX12DeviceMemory* dm)
        : Image(),
        deviceMemory(dm),
        isSwapChainImage(true),
        extent({}),
        type(ImageType::MAX),
        format(PixelFormat::MAX),
        usage(ImageUsage::MAX),
        mipLevels(0),
        arrayLayers(0),
        samples(0) {}

    DX12Image::DX12Image(const ImageCreateInfo* createInfo)
        : Image(createInfo),
        deviceMemory(nullptr),
        isSwapChainImage(false),
        extent(createInfo->extent),
        type(createInfo->type),
        format(createInfo->format),
        usage(createInfo->usage),
        mipLevels(createInfo->mipLevels),
        arrayLayers(createInfo->arrayLayers),
        samples(createInfo->samples) {}

    DX12Image::~DX12Image() = default;

    bool DX12Image::IsSwapChainImage()
    {
        return isSwapChainImage;
    }

    void DX12Image::BindMemory(DX12DeviceMemory* dm)
    {
        deviceMemory = dm;
    }

    DX12DeviceMemory* DX12Image::GetDeviceMemory()
    {
        return deviceMemory;
    }
}
