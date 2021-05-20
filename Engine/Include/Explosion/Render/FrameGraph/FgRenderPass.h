//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRENDERPASS_H
#define EXPLOSION_FGRENDERPASS_H

#include <functional>
#include <utility>
#include <Explosion/Render/FrameGraph/FgResources.h>
#include <Explosion/Render/FrameGraph/FgNode.h>

namespace Explosion {
    class FrameGraph;
    namespace RHI {
        class Driver;
    }

    class FgRenderPassBase : public FgNode {
    public:
        explicit FgRenderPassBase(const char*& n) : FgNode(n) {}

        ~FgRenderPassBase() override = default;

        virtual void Execute(const FrameGraph&, RHI::Driver&) = 0;
    };

    template<typename Data>
    class FgRenderPass : public FgRenderPassBase {
    public:
        using ExecuteType = std::function<void(const Data &, const FrameGraph&, RHI::Driver&)>;

        FgRenderPass(const char*& name, ExecuteType &&e) : FgRenderPassBase(name), execute(std::move(e)) {}

        ~FgRenderPass() override = default;

        void Execute(const FrameGraph& graph, RHI::Driver& driver) override {
            if (execute) execute(data, graph, driver);
        }

    private:
        friend class FrameGraph;
        ExecuteType execute;
        Data data;
    };
}

#endif //EXPLOSION_FGRENDERPASS_H
