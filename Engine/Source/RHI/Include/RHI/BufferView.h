//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <cstddef>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct VertexBufferViewInfo {
        uint32_t stride;
    };

    struct IndexBufferViewInfo {
        IndexFormat format;
    };

    struct BufferViewCreateInfo {
        BufferViewType type;
        uint32_t offset;
        uint32_t size;
        union {
            VertexBufferViewInfo vertex;
            IndexBufferViewInfo index;
        };
    };

    class BufferView {
    public:
        NonCopyable(BufferView)
        virtual ~BufferView();

        virtual void Destroy() = 0;

    protected:
        explicit BufferView(const BufferViewCreateInfo& createInfo);
    };
}
