//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRESOURCES_H
#define EXPLOSION_FGRESOURCES_H

#include <limits>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Common/Template/RefObject.h>
#include <Explosion/Render/FrameGraph/FgNode.h>

namespace Explosion {
    template <typename>
    struct FgResHandleTrait {};

    template <>
    struct FgResHandleTrait<uint32_t> {
        using IndexType = uint16_t;
        using VersionType = uint16_t;
        static constexpr uint16_t INVALID_INDEX = std::numeric_limits<uint16_t>::max();
    };

    template <>
    struct FgResHandleTrait<uint64_t> {
        using IndexType = uint32_t;
        using VersionType = uint32_t;
        static constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
    };

    template <typename HandleType = uint32_t>
    class FgResourceHandle {
    public:
        using HandleTrait = FgResHandleTrait<HandleType>;
        FgResourceHandle() : index(HandleTrait::INVALID_INDEX), version(0) {}
        FgResourceHandle(typename HandleTrait::IndexType idx) : index(idx), version(0) {}
        ~FgResourceHandle() = default;

        operator bool() const noexcept { return index == HandleTrait::INVALID_INDEX; }

    private:
        typename HandleTrait::IndexType index;
        typename HandleTrait::VersionType version;
    };

    template <typename Res>
    class FgResource : public FgNode {
    public:
        FgResource() {}
        ~FgResource() {}
    };
}

#endif //EXPLOSION_FGRESOURCES_H
