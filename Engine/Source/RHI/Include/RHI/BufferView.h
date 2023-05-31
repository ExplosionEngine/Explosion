//
// Created by johnk on 20/3/2022.
//

#pragma once

#include <cstddef>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    struct VertexBufferViewInfo {
        size_t stride;
    };

    struct IndexBufferViewInfo {
        IndexFormat format;
    };

    struct BufferViewCreateInfo {
        BufferViewType type;
        size_t offset;
        size_t size;
        union {
            VertexBufferViewInfo vertex;
            IndexBufferViewInfo index;
        };
    };

    class BufferView {
    public:
        NON_COPYABLE(BufferView)
        virtual ~BufferView();

        virtual void Destroy() = 0;

    protected:
        explicit BufferView(const BufferViewCreateInfo& createInfo);
    };
}
