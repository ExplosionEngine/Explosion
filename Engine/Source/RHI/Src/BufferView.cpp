//
// Created by johnk on 20/3/2022.
//

#include <RHI/BufferView.h>

namespace RHI {
    BufferViewCreateInfo::BufferViewCreateInfo(
        BufferViewType inType, uint32_t inSize, uint32_t inOffset, const std::variant<VertexBufferViewInfo, IndexBufferViewInfo>& inExtent)
        : type(inType)
        , size(inSize)
        , offset(inOffset)
        , extend(inExtent)
    {
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetType(BufferViewType inType)
    {
        type = inType;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetOffset(uint32_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetSize(uint32_t inSize)
    {
        size = inSize;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetExtendVertex(uint32_t inStride)
    {
        extend = VertexBufferViewInfo { inStride };
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetExtendIndex(IndexFormat inFormat)
    {
        extend = IndexBufferViewInfo { inFormat };
        return *this;
    }

    size_t BufferViewCreateInfo::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(BufferViewCreateInfo));
    }

    BufferView::BufferView(const BufferViewCreateInfo& createInfo) {}

    BufferView::~BufferView() = default;
}
