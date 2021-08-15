//
// Created by John Kindem on 2021/5/16 0016.
//

#include <utility>

#include <Engine/RHI/Common/RenderPass.h>

namespace Explosion::RHI {
    RenderPass::RenderPass(Config config) : config(std::move(config)) {}

    RenderPass::~RenderPass() = default;
}
