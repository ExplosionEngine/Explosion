//
// Created by John Kindem on 2021/5/26.
//

#include <Engine/RHI/Common/DescriptorPool.h>

#include <utility>

namespace Explosion::RHI {
    DescriptorPool::DescriptorPool(DescriptorPool::Config config) : config(std::move(config)) {}

    DescriptorPool::~DescriptorPool() = default;
}
