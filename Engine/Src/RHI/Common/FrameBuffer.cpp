//
// Created by John Kindem on 2021/5/16 0016.
//

#include <utility>

#include <Explosion/RHI/Common/FrameBuffer.h>

namespace Explosion::RHI {
    FrameBuffer::FrameBuffer(Config config) : config(std::move(config)) {}

    FrameBuffer::~FrameBuffer() = default;
}
