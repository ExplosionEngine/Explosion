//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_RENDER_PASS_H
#define EXPLOSION_RHI_RENDER_PASS_H

#include <Common/Utility.h>
#include <RHI/Enum.h>

namespace RHI {
    struct AttachmentInfo {
        size_t index;
        AttachmentType type;
        PixelFormat format;
        size_t samples;
        AttachmentLoadOp loadOp;
        AttachmentStoreOp storeOp;
        AttachmentLoadOp stencilLoadOp;
        AttachmentStoreOp stencilStoreOp;
        ImageLayoutFlags initialLayout;
        ImageLayoutFlags finalLayout;
    };

    struct RenderPassCreateInfo {
        size_t attachmentNum;
        const AttachmentInfo* attachments;
    };

    class RenderPass {
    public:
        NON_COPYABLE(RenderPass)
        virtual ~RenderPass();

    protected:
        explicit RenderPass(const RenderPassCreateInfo* createInfo);
    };
}

#endif // EXPLOSION_RHI_RENDER_PASS_H
