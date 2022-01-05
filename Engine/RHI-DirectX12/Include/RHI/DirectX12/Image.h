//
// Created by johnk on 4/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_IMAGE_H
#define EXPLOSION_RHI_DX12_IMAGE_H

#include <d3d12.h>
#include <wrl/client.h>

#include <RHI/Image.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12DeviceMemory;

    class DX12Image : public Image {
    public:
        NON_COPYABLE(DX12Image)
        explicit DX12Image(DX12DeviceMemory* deviceMemory);
        explicit DX12Image(const ImageCreateInfo* createInfo);
        ~DX12Image() override;

        bool IsSwapChainImage();
        void BindMemory(DX12DeviceMemory* deviceMemory);
        DX12DeviceMemory* GetDeviceMemory();

    private:
        DX12DeviceMemory* deviceMemory;
        bool isSwapChainImage;
        Extent2D extent;
        ImageType type;
        PixelFormat format;
        ImageUsageFlags usage;
        size_t mipLevels;
        size_t arrayLayers;
        size_t samples;
    };
}

#endif //EXPLOSION_RHI_DX12_IMAGE_H
