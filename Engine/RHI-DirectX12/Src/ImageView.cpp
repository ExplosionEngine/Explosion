//
// Created by johnk on 5/1/2022.
//

#include <RHI/DirectX12/ImageView.h>
#include <RHI/DirectX12/Image.h>

namespace RHI::DirectX12 {
    DX12ImageView::DX12ImageView(const ImageViewCreateInfo* createInfo)
        : ImageView(createInfo),
        image(static_cast<DX12Image*>(createInfo->image)),
        type(createInfo->type),
        format(createInfo->format),
        componentMapping(createInfo->componentMapping),
        subResourceRange(createInfo->subResourceRange) {}

    DX12ImageView::~DX12ImageView() = default;

    DX12Image* DX12ImageView::GetImage()
    {
        return image;
    }

    ImageViewType DX12ImageView::GetType()
    {
        return type;
    }

    PixelFormat DX12ImageView::GetFormat()
    {
        return format;
    }

    const ComponentMapping& DX12ImageView::GetComponentMapping()
    {
        return componentMapping;
    }

    const SubResourceRange& DX12ImageView::GetSubResourceRange()
    {
        return subResourceRange;
    }
}
