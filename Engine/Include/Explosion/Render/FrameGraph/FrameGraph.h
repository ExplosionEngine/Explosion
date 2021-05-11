//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FRAMEGRAPH_H
#define EXPLOSION_FRAMEGRAPH_H

#include <string>
#include <vector>
#include <memory>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Render/FrameGraph/FgRenderPass.h>

namespace Explosion {
    class FrameGraph;
    class FrameGraphBuilder {
    public:
        FrameGraphBuilder(FrameGraph& g) : graph(g) {}

        ~FrameGraphBuilder() {}
    private:
        FrameGraph& graph;
    };

    class FrameGraph : public NonCopy {
    public:
        FrameGraph() {}

        ~FrameGraph() {}

        template<typename Data, typename Setup, typename Execute>
        FgRenderPass<Data> &AddCallbackPass(const std::string &name, Setup &&s, Execute &&e)
        {
            FgRenderPass<Data>* pass = new FgRenderPass<Data>(name, std::move(e));
            passes.template emplace_back(pass);
            s(FrameGraphBuilder(*this), pass->data);
            return *pass;
        }
    private:
        std::vector<std::unique_ptr<FgRenderPassBase>> passes;
    };
}

#endif //EXPLOSION_FRAMEGRAPH_H
