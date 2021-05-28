//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRESOURCES_H
#define EXPLOSION_FGRESOURCES_H

#include <limits>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Common/Exception.h>
#include <Explosion/Common/Template/RefObject.h>
#include <Explosion/Render/FrameGraph/FgNode.h>
#include <Explosion/Render/FrameGraph/FgHandle.h>

namespace Explosion {

    namespace RHI {
        class Driver;
    }

    class FgVirtualResource : public FgNode {
    public:
        explicit FgVirtualResource(const char* name) : FgNode(name), isVirtual(true) {}
        ~FgVirtualResource() override = default;

        virtual void Init() {}

        virtual void DeVirtualize(RHI::Driver&) {}

        virtual void Release(RHI::Driver&) {}

    protected:
        bool isVirtual;
    };

    template <typename Resource>
    class FgResource : public FgVirtualResource {
    public:
        using Descriptor = typename Resource::Descriptor;

        explicit FgResource(const char* name, const Descriptor& desc)
                : FgVirtualResource(name),
                  descriptor(desc) {}

        ~FgResource() override
        {
            EXPLOSION_ASSERT(isVirtual, "gpu resource not released.");
        }

        void Init()
        {
            resource = std::make_unique<Resource>(descriptor);
        }

        void DeVirtualize(RHI::Driver& driver)
        {
            if (isVirtual) return;
            resource->Submit(driver);
            isVirtual = false;
        }

        void Release(RHI::Driver& driver)
        {
            if (!isVirtual) return;
            resource->Release(driver);
            isVirtual = true;
        }

    private:
        using ResourcePtr = std::unique_ptr<Resource>;

        ResourcePtr resource;
        Descriptor descriptor;
    };

}

#endif //EXPLOSION_FGRESOURCES_H
