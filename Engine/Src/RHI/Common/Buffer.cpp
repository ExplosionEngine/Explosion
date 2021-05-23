//
// Created by John Kindem on 2021/5/16 0016.
//

#include <Explosion/RHI/Common/Buffer.h>

namespace Explosion::RHI {
    Buffer::~Buffer() = default;

    Buffer::Buffer(Buffer::Config config) : config(config) {}
}
