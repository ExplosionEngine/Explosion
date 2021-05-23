//
// Created by John Kindem on 2021/5/16 0016.
//

#ifndef EXPLOSION_RENDERPASS_H
#define EXPLOSION_RENDERPASS_H

#include <Explosion/RHI/Common/Enum.h>

namespace Explosion::RHI {
    class RenderPass {
    public:
        struct AttachmentConfig {
            AttachmentType type;
            Format format;
            AttachmentLoadOp loadOp;
            AttachmentStoreOp storeOp;
        };

        struct Config {
            std::vector<AttachmentConfig> attachmentConfigs;
        };

        virtual ~RenderPass();

    protected:
        explicit RenderPass(Config config);

        Config config;
    };
}

#endif //EXPLOSION_RENDERPASS_H
