//
// Created by johnk on 20/3/2022.
//

#include <RHI/BufferView.h>

namespace RHI {
    VertexBufferViewInfo::VertexBufferViewInfo(uint32_t inStride)
        : stride(inStride)
    {
    }

    IndexBufferViewInfo::IndexBufferViewInfo(IndexFormat inFormat)
        : format(inFormat)
    {
    }

    BufferViewCreateInfo::BufferViewCreateInfo(
        const BufferViewType inType,
        const uint32_t inSize,
        const uint32_t inOffset,
        const std::variant<VertexBufferViewInfo, IndexBufferViewInfo, StorageBufferViewInfo>& inExtent)
        : type(inType)
        , size(inSize)
        , offset(inOffset)
        , extend(inExtent)
    {
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetType(const BufferViewType inType)
    {
        type = inType;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetOffset(const uint32_t inOffset)
    {
        offset = inOffset;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetSize(const uint32_t inSize)
    {
        size = inSize;
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetExtendVertex(const uint32_t inStride)
    {
        extend = VertexBufferViewInfo { inStride };
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetExtendIndex(const IndexFormat inFormat)
    {
        extend = IndexBufferViewInfo { inFormat };
        return *this;
    }

    BufferViewCreateInfo& BufferViewCreateInfo::SetExtendStorage(StorageFormat inFormat)
    {
        extend = StorageBufferViewInfo { inFormat };
        return *this;
    }


    size_t BufferViewCreateInfo::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(BufferViewCreateInfo));
    }

    BufferView::BufferView(const BufferViewCreateInfo&) {}

    BufferView::~BufferView() = default;
}
