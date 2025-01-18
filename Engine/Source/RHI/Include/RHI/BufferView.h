//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <cstddef>
#include <variant>

#include <Common/Utility.h>
#include <Common/Hash.h>
#include <RHI/Common.h>

namespace RHI {
    struct VertexBufferViewInfo {
        uint32_t stride;

        explicit VertexBufferViewInfo(uint32_t inStride = 0);
    };

    struct IndexBufferViewInfo {
        IndexFormat format;

        explicit IndexBufferViewInfo(IndexFormat inFormat = IndexFormat::max);
    };

    struct StorageBufferViewInfo {
        uint32_t stride;

        explicit StorageBufferViewInfo(uint32_t inStride = 0);
    };

    struct BufferViewCreateInfo {
        BufferViewType type;
        uint32_t size;
        uint32_t offset;
        std::variant<VertexBufferViewInfo, IndexBufferViewInfo, StorageBufferViewInfo> extend;

        explicit BufferViewCreateInfo(
            BufferViewType inType = BufferViewType::max,
            uint32_t inSize = 0,
            uint32_t inOffset = 0,
            const std::variant<VertexBufferViewInfo, IndexBufferViewInfo, StorageBufferViewInfo>& inExtent = {});

        BufferViewCreateInfo& SetType(BufferViewType inType);
        BufferViewCreateInfo& SetOffset(uint32_t inOffset);
        BufferViewCreateInfo& SetSize(uint32_t inSize);
        BufferViewCreateInfo& SetExtendVertex(uint32_t inStride);
        BufferViewCreateInfo& SetExtendIndex(IndexFormat inFormat);
        BufferViewCreateInfo& SetExtendStorage(uint32_t inStride);

        size_t Hash() const;
    };

    class BufferView {
    public:
        NonCopyable(BufferView)
        virtual ~BufferView();

    protected:
        explicit BufferView(const BufferViewCreateInfo& createInfo);
    };
}
