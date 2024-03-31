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
    };

    struct IndexBufferViewInfo {
        IndexFormat format;
    };

    template <typename Derived>
    struct BufferViewCreateInfoBase {
        BufferViewType type;
        uint32_t offset;
        uint32_t size;
        std::variant<VertexBufferViewInfo, IndexBufferViewInfo> extend;

        BufferViewCreateInfoBase();
        Derived& Type(BufferViewType inType);
        Derived& Offset(uint32_t inOffset);
        Derived& Size(uint32_t inSize);
        Derived& ExtendVertex(uint32_t inStride);
        Derived& ExtendIndex(IndexFormat inFormat);

        size_t Hash() const;
    };

    struct BufferViewCreateInfo : public BufferViewCreateInfoBase<BufferViewCreateInfo> {
        BufferViewCreateInfo();
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

namespace RHI {
    template<typename Derived>
    BufferViewCreateInfoBase<Derived>::BufferViewCreateInfoBase() = default;

    template<typename Derived>
    Derived& BufferViewCreateInfoBase<Derived>::Type(BufferViewType inType)
    {
        type = inType;
        return *static_cast<Derived*>(this);
    }

    template<typename Derived>
    Derived& BufferViewCreateInfoBase<Derived>::Offset(uint32_t inOffset)
    {
        offset = inOffset;
        return *static_cast<Derived*>(this);
    }

    template<typename Derived>
    Derived& BufferViewCreateInfoBase<Derived>::Size(uint32_t inSize)
    {
        size = inSize;
        return *static_cast<Derived*>(this);
    }

    template<typename Derived>
    Derived& BufferViewCreateInfoBase<Derived>::ExtendVertex(uint32_t inStride)
    {
        extend = VertexBufferViewInfo { inStride };
        return *static_cast<Derived*>(this);
    }

    template<typename Derived>
    Derived& BufferViewCreateInfoBase<Derived>::ExtendIndex(IndexFormat inFormat)
    {
        extend = IndexBufferViewInfo { inFormat };
        return *static_cast<Derived*>(this);
    }

    template<typename Derived>
    size_t BufferViewCreateInfoBase<Derived>::Hash() const
    {
        return Common::HashUtils::CityHash(this, sizeof(BufferViewCreateInfo));
    }
}
