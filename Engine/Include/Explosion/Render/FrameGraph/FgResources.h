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
        using HandleType = uint32_t;
        using IndexType = uint16_t;
        using VersionType = uint16_t;
        static constexpr uint16_t INVALID_INDEX = std::numeric_limits<IndexType>::max();

        static IndexType Index(HandleType handle) { return handle & 0xFFFF; }
        static VersionType Version(HandleType handle) { return (handle >> 16) & 0xFFFF; }
        static void Reset(HandleType& handle) { handle = INVALID_INDEX; }
        static void Inc(HandleType& handle) { handle = Index(handle) | ((Version(handle) + 1) << 16); }
        static bool Invalid(HandleType handle) { return Index(handle) == INVALID_INDEX; }
    };

    template <>
    struct FgResHandleTrait<uint64_t> {
        using HandleType = uint64_t;
        using IndexType = uint32_t;
        using VersionType = uint32_t;
        static constexpr uint32_t INVALID_INDEX = std::numeric_limits<IndexType>::max();

        static IndexType Index(HandleType handle) { return handle & 0xFFFFFFFF; }
        static VersionType Version(HandleType handle) { return (handle >> 32) & 0xFFFFFFFF; }
        static void Reset(HandleType& handle) { handle = INVALID_INDEX; }
        static void Inc(HandleType& handle) { handle = Index(handle) | (((HandleType)Version(handle) + 1) << 32); }
        static bool Invalid(HandleType handle) { return Index(handle) == INVALID_INDEX; }
    };

    template <typename Type = uint32_t>
    class FgResourceHandle {
    public:
        using HandleType = Type;
        using HandleTrait = FgResHandleTrait<HandleType>;

        FgResourceHandle() : handle(HandleTrait::INVALID_INDEX) {}
        explicit FgResourceHandle(typename HandleTrait::HandleType idx) : handle(idx) {}
        ~FgResourceHandle() = default;

        explicit operator bool() const noexcept { return !HandleTrait::Invalid(handle); }

        typename HandleTrait::VersionType Version() const noexcept { return HandleTrait::Version(handle); }

        typename HandleTrait::IndexType Index() const noexcept { return HandleTrait::Index(handle); }

        void Reset() { HandleTrait::Reset(handle); }

        FgResourceHandle& operator++() noexcept { HandleTrait::Inc(handle); return *this; }

    private:
        HandleType handle;
    };

    class FgVirtualResource : public FgNode {
    public:
        explicit FgVirtualResource(const char* name) : FgNode(name) {}
        ~FgVirtualResource() override = default;
    };

    using PixelFormat = uint32_t;

    class FgTexture : public FgVirtualResource {
    public:
        struct Descriptor {
            uint32_t width;
            uint32_t height;
            PixelFormat format;
        };

        FgTexture(const char* name, const Descriptor& desc) : FgVirtualResource(name), descriptor(desc) {}
        ~FgTexture() override = default;

    private:
        Descriptor descriptor;
    };
}

#endif //EXPLOSION_FGRESOURCES_H
