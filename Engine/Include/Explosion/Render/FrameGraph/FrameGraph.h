//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FRAMEGRAPH_H
#define EXPLOSION_FRAMEGRAPH_H

#include <vector>
#include <memory>
#include <unordered_map>
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

        void SideEffect();

    private:
        FrameGraph& graph;
        FgRenderPassBase* renderPass;
    };

    class FrameGraph : public NonCopy {
    public:
        FrameGraph() = default;

        ~FrameGraph() override = default;

        template<typename Data, typename Setup, typename Execute>
        void AddCallbackPass(const char* name, Setup &&s, Execute &&e)
        {
            auto pass = new FgRenderPass<Data>(name, std::forward<Execute>(e));
            passes.template emplace_back(pass);
            s(FrameGraphBuilder(*this, pass), pass->data);
        }

        FrameGraph& Compile();

        FrameGraph& Execute(RHI::Driver&);

        using PassVector = std::vector<std::unique_ptr<FgRenderPassBase>>;
        using ResourceVector = std::vector<std::unique_ptr<FgVirtualResource>>;

        const PassVector& GetPasses() const { return passes; }

        const ResourceVector& GetResources() const { return resources; }

    private:
        struct Edge {
            FgNode* from;
            FgNode* to;
        };

        void Cull();

        friend class FrameGraphBuilder;
        PassVector passes;
        ResourceVector resources;
        std::vector<FgHandle> resHandles;
        std::vector<Edge> edges;
        std::unordered_map<FgNode*, std::vector<FgNode*>> incomingEdgeMap;
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
