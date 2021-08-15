//
// Created by John Kindem on 2021/5/26.
//

#include <RHI/DescriptorPool.h>

#include <utility>

namespace Explosion::RHI {
    DescriptorPool::DescriptorPool(DescriptorPool::Config config) : config(std::move(config)) {}

    DescriptorPool::~DescriptorPool() = default;
}
