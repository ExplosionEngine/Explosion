//
// Created by Zach Lee on 2021/5/7.
//

#ifndef EXPLOSION_MODULE_PASSDEFINES_H
#define EXPLOSION_MODULE_PASSDEFINES_H

#include <cstdint>
#include <vector>

using AttachmentFormat = uint32_t;

struct TargetInfo {
    AttachmentFormat format;
    uint32_t width;
    uint32_t height;
};

struct AttachmentInfo {
    AttachmentFormat format;
    uint32_t loadOp;
    uint32_t storeOp;
};

struct PassInfo {
    std::vector<AttachmentFormat> attachments;
};

struct FrameBufferInfo {
    std::vector<uint32_t> attachments;
    std::vector<uint32_t> targets;
};

struct GraphInfo {
    std::vector<TargetInfo> targets;
    std::vector<PassInfo> passes;
};

#endif