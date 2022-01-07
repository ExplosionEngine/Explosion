//
// Created by johnk on 8/1/2022.
//

#ifndef EXPLOSION_RHI_FRAME_BUFFER_H
#define EXPLOSION_RHI_FRAME_BUFFER_H

#include <Common/Utility.h>

namespace RHI {
    class ImageView;
    class RenderPass;

    struct FrameBufferCreateInfo {
        const RenderPass* renderPass;
        size_t attachmentNum;
        const ImageView* attachments;
        size_t width;
        size_t height;
        size_t layers;
    };

    class FrameBuffer {
    public:
        NON_COPYABLE(FrameBuffer)
        virtual ~FrameBuffer();

    protected:
        explicit FrameBuffer(const FrameBufferCreateInfo* createInfo);
    };
}

#endif //EXPLOSION_RHI_FRAME_BUFFER_H
