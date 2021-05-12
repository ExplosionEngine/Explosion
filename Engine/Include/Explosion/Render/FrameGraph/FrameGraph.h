//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FRAMEGRAPH_H
#define EXPLOSION_FRAMEGRAPH_H

#include <vector>
#include <memory>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Render/FrameGraph/FgRenderPass.h>
#include <Explosion/Render/FrameGraph/FgResources.h>

namespace Explosion {
    class FrameGraph;
    using FgHandle = FgResourceHandle<uint32_t>;

    class FrameGraphBuilder {
    public:
        explicit FrameGraphBuilder(FrameGraph& g, FgRenderPassBase* pass) : graph(g), renderPass(pass) {}

        ~FrameGraphBuilder() = default;

        template <typename Resource>
        FgHandle Create(const char* name, const typename Resource::Descriptor& desc);

        FgHandle Read(FgHandle handle);

        FgHandle Write(FgHandle handle);

    private:
        FrameGraph& graph;
        FgRenderPassBase* renderPass;
    };

    class FrameGraph : public NonCopy {
    public:
        FrameGraph() {}

        ~FrameGraph() override {}

        template<typename Data, typename Setup, typename Execute>
        FgRenderPass<Data> &AddCallbackPass(const char* name, Setup &&s, Execute &&e)
        {
            auto pass = new FgRenderPass<Data>(name, std::forward<Execute>(e));
            passes.template emplace_back(pass);
            s(FrameGraphBuilder(*this, pass), pass->data);
            return *pass;
        }

        FrameGraph& Compile();

        FrameGraph& Execute(Driver&);

    private:
        struct Edge {
            FgNode* from;
            FgNode* to;
        };

        friend class FrameGraphBuilder;
        std::vector<std::unique_ptr<FgRenderPassBase>> passes;
        std::vector<std::unique_ptr<FgVirtualResource>> resources;
        std::vector<FgHandle> resHandles;
        std::vector<Edge> edges;
    };


    template <typename Resource>
    FgHandle FrameGraphBuilder::Create(const char* name, const typename Resource::Descriptor& desc)
    {
        auto res = new Resource(name, desc);
        FgResourceHandle handle(static_cast<FgHandle::HandleType>(graph.resources.size()));
        graph.resources.template emplace_back(res);
        graph.resources.template emplace_back(handle);
        return handle;
    }
}

#endif //EXPLOSION_FRAMEGRAPH_H
