//
// Created by johnk on 5/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_IMAGE_VIEW_H
#define EXPLOSION_RHI_DX12_IMAGE_VIEW_H

#include <RHI/ImageView.h>

namespace RHI::DirectX12 {
    class DX12Image;

    class DX12ImageView : public ImageView {
    public:
        NON_COPYABLE(DX12ImageView)
        explicit DX12ImageView(const ImageViewCreateInfo* createInfo);
        ~DX12ImageView() override;

        DX12Image* GetImage();
        ImageViewType GetType();
        PixelFormat GetFormat();
        const ComponentMapping& GetComponentMapping();
        const SubResourceRange& GetSubResourceRange();

    private:
        DX12Image* image;
        ImageViewType type;
        PixelFormat format;
        ComponentMapping componentMapping;
        SubResourceRange subResourceRange;
    };
}

#endif //EXPLOSION_RHI_DX12_IMAGE_VIEW_H
